
extern "C" {
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <getopt.h>
}

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>

#include "tcanetpp_ip.h"
#include "Socket.h"
#include "EventManager.h"
#include "CidrUtils.h"
#include "StringUtils.h"
#include "CircularBuffer.h"
#include "LogFacility.h"
using namespace tcanetpp;


#define SEQTIMEOUT      2
#define MAXCONLOSS      3

const char* Version = "v0.13";
bool Alarm = false;
int  Pid   = 0;


struct data_ts {
    uint32_t  secs;
    uint32_t  usecs;
};


struct PathData {
    uint16_t  hop;
    uint16_t  proto;
    uint32_t  ipaddr;
    uint32_t  seq;
    uint32_t  cnt;
    float     rtt;
    float     rtt_min;
    float     rtt_max;
    float     rtt_total;
    float     rtd;
    float     rtd_min;
    float     rtd_max;
    float     rtd_total;
    timeval   tv;

    PathData() : hop(0), proto(SOCKET_UDP), ipaddr(0), seq(0), cnt(0), 
               rtt(0.0), rtt_min(0.0), rtt_max(0.0), rtt_total(0.0),
               rtd(0.0), rtd_min(0.0), rtd_max(0.0), rtd_total(0.0)
    { }
};

typedef std::vector<PathData> PathVector;
typedef std::set<int>         HopIndex;


struct IcmpResponse {
    netip_h    iph;
    neticmp_h  icmph;
};



void usage()
{
    std::cout << "Usage: gktrace [-cdihv] <host|ip>" << std::endl
              << "    -c | --count  <num>  : Number of test iterations" << std::endl
              << "    -d | --debug         : Enable debug output" << std::endl
              << "    -h | --help          : Print help info and exit" << std::endl
              << "    -i | --interval <ms> : milliseconds between test iterations" << std::endl
              << "                            (default = 1000 or 1 second)" << std::endl
              << "    -n | --nodns         : do not resolve the results" << std::endl
              << "    -v | --version       : Print version info and exit" << std::endl;
    exit(0);
}


void version()
{
    std::cout << "gktrace version: " << Version << std::endl;
    exit(0);
}


void errorOut ( const std::string & err )
{
    std::cout << "Error: " << err << std::endl;
    exit(-1);
}


void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM ) {
        Alarm = true;
    }
    return;
}


void
dropPriv()
{
    uid_t  uid;
    gid_t  gid;

    if ( geteuid() != 0 )
        return;

    uid  = getuid();
    gid  = getgid();
    setegid(gid);
    seteuid(uid);

    if ( geteuid() != uid )
        throw Exception("dropPrivileges() failed");

    return;
}


ssize_t readIPHeader ( CircularBuffer * buff, netip_h * iph )
{
    size_t len  = sizeof(netip_h);

    if ( buff->readAvailable() < len )
        return -1;

    return buff->read(iph, len);
}


ssize_t readIcmpHeader ( CircularBuffer * buff, neticmp_h * icmph )
{
    size_t len = sizeof(neticmp_h);

    if ( buff->readAvailable() < len )
        return -1;

    return buff->read(icmph, len);
}


size_t  readIcmpResponse ( CircularBuffer * buff, IcmpResponse & response )
{
    ssize_t   rd;

    rd   = readIPHeader(buff, &response.iph);
    if ( rd <= 0 )
        return rd;

    rd   = readIcmpHeader(buff, &response.icmph);

    if ( rd <= 0 ) {
        std::cout << "Invalid header" << std::endl;
        return -1;
    }

    return rd;
}


ssize_t readUdpHeader ( CircularBuffer * buff, netudp_h * udph )
{
    size_t len = sizeof(netudp_h);

    if ( buff->readAvailable() < len )
        return -1;

    return buff->read(udph, len);
}


void printStatHeader()
{
    std::cout << std::endl;
    std::cout << std::setw(3) << "hop" 
              << std::setw(15) << " address" 
              << std::setw(4) << "   seq" 
              << std::setw(8) << " rtt(ms)" 
              << std::setw(8) << " avg(ms)"
              << std::setw(8) << " ipdv"
              << std::setw(6) << "   type"
              << std::endl;
    std::cout << "----------------------------------------------------------------" 
              << std::endl;
}



int main ( int argc, char ** argv )
{
    char      optChar;
    char    * target;
    int       optindx  = 0;
    int       interval = 1000;
    int       count    = 100;
    bool      debug    = false;
    bool      resolve  = true;
    timeval   tvin, tvo;

    static struct option l_opts[] = { {"debug", no_argument, 0, 'd'},
                                      {"count", required_argument, 0, 'c'},
                                      {"nodns", no_argument, 0, 'n'},
                                      {"interval", required_argument, 0, 'i'},
                                      {"help", no_argument, 0, 'h'},
                                      {"version", no_argument, 0, 'v'},
                                      {0,0,0,0}
                                    };

    if ( argc < 2 )
        usage();

    while ( (optChar = getopt_long(argc, argv, "c:dhi:nv", l_opts, &optindx)) != EOF )
    {
        switch ( optChar ) {
            case 'c':
                count = StringUtils::fromString<int>(optarg);
                break;
            case 'd':
                debug = true;
                break;
            case 'i':
                interval = StringUtils::fromString<int>(optarg);
                break;
            case 'h':
                usage();
                break;
            case 'n':
                resolve = false;
                break;
            case 'v':
                version();
                break;
            default:
                break;
        }
    }

    target    = ::strdup(argv[argc-1]);
    Pid       = ::getpid() & 0xFFFF;

    std::string host = target;
    ::free(target);

    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGINT,  &sigHandler);
    ::memset(&tvin, 0, sizeof(tvin));
    ::memset(&tvo, 0, sizeof(tvo));

    ipv4addr_t  srcaddr = CidrUtils::GetHostAddr();
    ipv4addr_t  dstaddr = CidrUtils::GetHostAddr(host);
    std::string srcname = CidrUtils::ntop(srcaddr);
    std::string dstname = CidrUtils::ntop(dstaddr);

    if ( dstaddr == 0 ) {
        std::cout << "Invalid target " << host << std::endl;
        usage();
    }

    Socket * udps  = new Socket(dstaddr, SOCKET_UDP, SOCKTYPE_RAW, SOCKET_UDP);
    Socket * icmps = new Socket(dstaddr, SOCKET_ICMP, SOCKTYPE_RAW, SOCKET_ICMP);

    udps->init(false);
    udps->setSocketOption(SocketOption::SetNoFragment(0));
    icmps->init(false);

    dropPriv();

    /* init buffers */
    sockaddr_in   csock;
    ipv4addr_t    addr;
    netudp_h    * udph;
    PathData    * udata;

    char        * wptr    = NULL;
    char        * wbuff   = NULL;
    bool          send    = true;
    bool          timeout = false;
    bool          pathd   = false;
    size_t        sz, buflen, idsz;
    ssize_t       wt, rd;

    uint16_t port = 5995;  // port mask for ttl counting
    buflen        = 2048;
    idsz          = sizeof(netudp_h) + sizeof(PathData);

    CircularBuffer * rbuff = new CircularBuffer(buflen);
    wbuff         = (char*) ::malloc(idsz);
    udph          = (netudp_h*) wbuff;
    udata         = (PathData*) wbuff + sizeof(netudp_h);

    udph->srcport = 0;
    udph->dstport = htons(port);
    udph->length  = htons(idsz);
    udph->chksum  = 0;

    udata->hop    = 0;
    udata->proto  = SOCKET_UDP;
    udata->ipaddr = 0;
    udata->seq    = 0;
    udata->cnt    = 0;

    std::cout << "Sending UDP datagrams to " << dstname << std::endl;

    uint16_t    ttl     = 0;
    uint16_t    maxhops = 30;
    uint16_t    phops   = maxhops;
    int         lostctr = 0;

    PathVector  pathseq(maxhops);
    HopIndex    lostq;

    printStatHeader();

    while ( ! Alarm )
    {
        if ( send || timeout ) 
        {
            if ( timeout )
                ttl--;
            if ( ttl > pathseq.size() )
                ttl = 0;
            if ( lostctr > MAXCONLOSS )
                break;

            if ( pathd && ttl == phops ) {
                ttl = 0;
                if ( (udata->seq + 1) > (uint16_t) count )
                    break;
                ::usleep(interval * 1000);
                printStatHeader();
            }

            if ( lostq.size() > 3 ) {
                HopIndex::iterator hIter = lostq.begin();
                phops   = *hIter;
                ttl     = 0;
                send    = true;
                timeout = false;
                pathd   = true;
                lostq.clear();
                if ( debug )
                    std::cout << "timeout limit reached at hop " << --phops << std::endl;
                continue;
            }

            PathData & pdata = pathseq.at(ttl++);
            EventManager::GetTimeOfDay(&tvin);

            pdata.tv  = tvin;

            if ( pathd )
                pdata.seq++;

            *udata = pdata;

            if ( timeout || pdata.proto == SOCKET_ICMP) 
            {
                if ( timeout ) {
                    timeout = false;
                    if ( pdata.proto == SOCKET_ICMP || pathd ) {  //already tried
                        lostctr++;
                        tvo.tv_sec = tvin.tv_sec;
                        send  = true;
                        continue;
                    }
                    pdata.proto = SOCKET_ICMP;
                    lostq.insert(ttl);
                }

                neticmp_h  req;
                req.type   = ICMP_ECHO;
                req.code   = 0;
                req.chksum = 0;
                req.id     = port + ttl;
                req.seq    = 0;
                req.chksum = Socket::IpChkSum(((uint16_t*)&req), sizeof(neticmp_h));

                if ( icmps->setSocketOption(IPPROTO_IP, IP_TTL, ttl) < 0 )
                    errorOut("error in setsockopt" + udps->getErrorString());

                wt = icmps->write(&req, sizeof(neticmp_h));
            } 
            else 
            {
                udph->srcport = htons(port+ttl);
                udph->chksum  = 0;

                if ( udps->setSocketOption(IPPROTO_IP, IP_TTL, ttl) < 0 )
                    errorOut("error in setsockopt" + udps->getErrorString());

                wt = udps->write(wbuff, idsz);
            }

            send    = false;
            timeout = false;
            tvo.tv_sec = tvin.tv_sec;
        }

        // read
        rbuff->reset();
        sz   = rbuff->writePtrAvailable();
        wptr = rbuff->getWritePtr(&sz);
        rd   = icmps->readFrom(wptr, sz, csock);
        addr = csock.sin_addr.s_addr;
        if ( rd < 0 )
            errorOut("Error in readFrom " + icmps->getErrorString());
        rbuff->setWritePtr(rd);

        IcmpResponse  response;

        if ( rd > 0 )
            rd = readIcmpResponse(rbuff, response);

        EventManager::GetTimeOfDay(&tvin);

        if ( rd > 0 && (response.icmph.type == ICMP_TIME_EXCEEDED
                     || response.icmph.type == ICMP_ECHOREPLY) )
        {
            netip_h   iph;
            neticmp_h icmph;
            netudp_h  udph;
            uint16_t  hop = 0;
            float     rtt = 0.0;
            float     avg = 0.0;
            float     rtd = 0.0;

            sz   = rbuff->readAvailable();
           
            if ( response.icmph.type == ICMP_ECHOREPLY )
            {
                hop = response.icmph.id - port;
                lostq.erase(hop);
                // echo reply from dst
                if ( response.iph.srcaddr == dstaddr ) {
                    pathd  = true;
                    phops  = hop;
                }
            }
            else 
            {
                if ( ! readIPHeader(rbuff, &iph) )
                    continue;

                if ( iph.dstaddr != dstaddr )  // not our packet
                    continue;

                if ( iph.protocol == SOCKET_ICMP ) {
                    if ( ! readIcmpHeader(rbuff, &icmph) )
                        continue;
                    hop = icmph.id - port; //unmask hop id
                    lostq.erase(hop);
                } else if ( iph.protocol == SOCKET_UDP ) {
                    if ( ! readUdpHeader(rbuff, &udph) )
                        continue;
                    hop  = ntohs(udph.srcport) - port; // unmask hop id
                }
            }

            if ( hop == 0 )
                continue;
            lostctr = 0;

            if ( hop >= pathseq.size() ) { 
                std::cout << " Invalid ttl " << hop
                    << " from " << CidrUtils::ntop(iph.dstaddr) << std::endl;
                continue;
            }
                
            // path vector
            PathData & pdata = pathseq.at(hop-1);

            if ( pdata.seq == 0 || pdata.hop == 0 ) {
                pdata.hop    = hop;   
                pdata.ipaddr = response.iph.srcaddr;
            } else {
                if ( hop == phops && ! pathd )
                    phops++;
            }

            rtt = EventManager::TimevalDiffMs(&tvin, &pdata.tv);

            if ( pathd && pdata.seq > 0 )
            {
                if ( pdata.rtt > 0.0 )
                    rtd = ::fabs(rtt - pdata.rtt);

                if ( rtt < pdata.rtt_min || pdata.rtt_min == 0 )
                    pdata.rtt_min = rtt;
                if ( rtt > pdata.rtt_max )
                    pdata.rtt_max = rtt;
                if ( rtd > 0.0 ) {
                    pdata.rtd = rtd;
                    if ( rtd > pdata.rtd_max )
                        pdata.rtd_max = rtd;
                    if ( rtd < pdata.rtd_min || pdata.rtd_min == 0 )
                        pdata.rtd_min = rtd;
                    pdata.rtd_total += rtd;
                }

                pdata.rtt = rtt;
                pdata.rtt_total += rtt;
                pdata.proto = iph.protocol;
                pdata.cnt++;

                avg = (pdata.rtt_total / pdata.cnt);
            }

            std::cout << std::setw(2) << hop << ": " 
                << std::setw(15) << CidrUtils::ntop(response.iph.srcaddr) 
                << "  <" << pdata.seq << ">"
                << std::setw(8) << std::setprecision(3) << rtt
                << std::setw(8) << std::setprecision(3) << avg
                << std::setw(8) << std::setprecision(3) << rtd;
            if ( iph.protocol == SOCKET_ICMP )
                std::cout << std::setw(4) << "    <i>";
            else
                std::cout << std::setw(4) << "    <u>";
            std::cout << std::endl;

            send  = true;
        }

        if ( tvo.tv_sec == 0 || send  )
            tvo.tv_sec = tvin.tv_sec;
        else if ( (tvin.tv_sec - tvo.tv_sec) > SEQTIMEOUT )
            timeout = true;
    }


    std::cout << std::endl << std::endl
        << "Destination: " << CidrUtils::ntop(dstaddr);
    if ( resolve )
        std::cout << " <" << CidrUtils::GetHostName(dstaddr) << ">  ";
    std::cout << " Hop count: " << phops << std::endl;

    std::cout << std::endl;
    std::cout << std::setw(3) << " hop" 
              << std::setw(18) << "  address  " 
              << std::setw(8) << "    loss%    " 
              << std::setw(10)  << "rtt (ms)" 
              << std::setw(8) << "min" 
              << std::setw(8) << "max" 
              << std::setw(10) << "ipdv"
              << std::setw(8) << "min"
              << std::setw(8) << "max";
    if ( resolve )
        std::cout << "     dns";
    std::cout << std::endl
        << "------------------------------------------------------------------------------------------";
    if ( resolve )
        std::cout << "---------------------------";
    std::cout << std::endl;



    PathVector::iterator  pIter;

    for ( pIter = pathseq.begin(); pIter != pathseq.end(); ++pIter )
    {
        float        loss, rttavg, rtdavg;
        std::string  hopname;

        PathData   & pdata = *pIter;

        if ( pdata.hop == 0 )
            break;

        loss    = ( 100.0 - ( ((float)pdata.cnt / pdata.seq) * 100.0) );
        rttavg  = ( pdata.rtt_total / pdata.cnt );
        rtdavg  = ( pdata.rtd_total / (float) (pdata.cnt - 1) );
        hopname = CidrUtils::ntop(pdata.ipaddr);

        std::cout << std::setw(2)  << pdata.hop << ": " 
                  << std::setw(18) << std::setiosflags(std::ios_base::right) <<  hopname 
                  << std::setw(5)  << loss << "% (" << pdata.cnt << "/" << pdata.seq << ")"
                  << std::setw(10) << std::setprecision(3) << rttavg
                  << std::setw(8) << std::setprecision(3) << pdata.rtt_min
                  << std::setw(8) << std::setprecision(3) << pdata.rtt_max
                  << std::setw(10) << std::setprecision(3) << rtdavg
                  << std::setw(8) << std::setprecision(3) << pdata.rtd_min
                  << std::setw(8) << std::setprecision(3) << pdata.rtd_max;
        if ( resolve )
            std::cout << "    <" << CidrUtils::GetHostName(pdata.ipaddr) << "> ";
        
        std::cout << std::endl;
    }

    std::cout << "Finished." << std::endl;

    return 0;
}



