
extern "C" {
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <getopt.h>
}

#include <cstdlib>
#include <iostream>

#include "tcanetpp_ip.h"
#include "Socket.h"
#include "Exception.hpp"
#include "CidrUtils.h"
#include "StringUtils.h"
#include "CircularBuffer.h"
using namespace tcanetpp;


#define MAX_ICMPDATA_SIZE  2048
#define ICMP_TIMEOUT_SECS  3

bool Alarm = false;
int  Pid   = 0;

static std::string Version = "v0.90";

struct icmp_ts {
    uint32_t  secs;
    uint32_t  usecs;

    icmp_ts() : secs(0), usecs(0) {}
};


struct IcmpResponse {
    netip_h    iph;
    neticmp_h  icmph;
};



void usage()
{
    std::cout << "Usage: pingt [-c:dhi:s:] <host>" << std::endl;
    std::cout << "  -c <count>  :   send <count> echo requests" << std::endl;
    std::cout << "  -d          :   enable debug" << std::endl;
    std::cout << "  -h          :   display help and exit" << std::endl;
    std::cout << "  -i <ms>     :   interval between requests in milliseconds" << std::endl;
    std::cout << "  -s <nbytes> :   send echo requests of <n> bytes in size" << std::endl;
    exit(0);
}

void version()
{
    std::cout << "pingt version: " << Version << std::endl;
    exit(0);
}


void errorOut ( const std::string & err )
{
    std::cout << "Error: " << err << std::endl;
    exit(-1);
}


void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM )
        Alarm = true;
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


bool getTimeOfDay ( timeval & tv )
{
    int r = gettimeofday(&tv, NULL);
    if ( r == 0 )
        return true;
    return false;
}


float timeval_to_ms ( const timeval & tv )
{
    return( ((float)tv.tv_sec * 1000.0) + ((float)tv.tv_usec / 1000.0) );
}


void timeval_diff ( const timeval * t2, const timeval * t1, timeval * result )
{
    result->tv_sec  = t2->tv_sec - t1->tv_sec;
    result->tv_usec = t2->tv_usec - t1->tv_usec;
    if ( result->tv_usec < 0 ) {
        --result->tv_sec;
        result->tv_usec += 1000000;
    }
}

float timeDiff( const timeval & t2, const timeval & t1 )
{
    timeval  rtv;

    timeval_diff(&t2, &t1, &rtv);

    return timeval_to_ms(rtv);
}



ssize_t  readIcmpHeader ( CircularBuffer * buff, IcmpResponse & response )
{
    char      * rptr, *ptr;
    size_t      rd, len;

    len  = sizeof(netip_h) + sizeof(neticmp_h);
    rptr = buff->getReadPtr(&rd);

    if ( rptr == NULL )
        return -1;
    if ( rd < len ) {
        std::cout << "Invalid header" << std::endl;
        buff->setReadPtr(rd);
        return -1;
    }

    ptr  = rptr;
    response.iph = *((netip_h*) ptr);
    ptr += sizeof(netip_h);
    response.icmph  = *((neticmp_h*) ptr);

    buff->setReadPtr(len);

    return len;
}

neticmp_h* readHeader ( CircularBuffer * buff )
{
    char      * rptr, *ptr;
    size_t      rd, len;

    netip_h   * iph   = NULL;
    neticmp_h * hdr   = NULL;

    len  = sizeof(netip_h) + sizeof(neticmp_h);
    rptr = buff->getReadPtr(&rd);

    if ( rptr == NULL )
        return hdr;

    if ( rd < len ) {
        std::cout << "Invalid header" << std::endl;
        buff->setReadPtr(rd);
        return hdr;
    }

    ptr  = rptr;
    iph  = (netip_h*) ptr;
    ptr += sizeof(netip_h);
    hdr  = (neticmp_h*) ptr;

    buff->setReadPtr(len);

    if ( hdr->id == Pid )
        return hdr;

    return NULL;
}

int main ( int argc, char ** argv )
{
    char      optChar;
    char    * target;
    int       optindx  = 0;
    int       interval = 0;
    int       count    = -1;
    int       timeout  = ICMP_TIMEOUT_SECS;
    bool      debug    = false;
    size_t    size     = 0;

    timeval   tvin, tvsnt, tvo;

    static struct option l_opts[] = { {"count", required_argument, 0, 'c'},
                                      {"debug", no_argument, 0, 'd'},
                                      {"interval", required_argument, 0, 'i'},
                                      {"help", no_argument, 0, 'h'},
                                      {"size", required_argument, 0, 's'},
                                      {"version", no_argument, 0, 'v'},
                                      {0,0,0,0}
                                    };

    if ( argc < 2 )
        usage();

    while ( (optChar = getopt_long(argc, argv, "c:di:hs:v", l_opts, &optindx)) != EOF )
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
            case 's':
                size = StringUtils::fromString<size_t>(optarg);
                break;
            case 'v':
                version();
                break;
            default:
                usage();
                break;
        }
    }

    target = strdup(argv[argc-1]);
    ::memset(&tvsnt, 0, sizeof(tvsnt));
    ::memset(&tvin, 0, sizeof(tvin));
    ::memset(&tvo, 0, sizeof(tvo));

    if ( interval == 0 )
        interval = 1000;

    std::string host = target;
    ::free(target);

    Pid  = ::getpid() & 0xFFFF;
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGINT,  &sigHandler);

    ipv4addr_t srcaddr = CidrUtils::GetHostAddr();
    ipv4addr_t dstaddr = CidrUtils::GetHostAddr(host);

    if ( dstaddr == 0 ) {
        std::cout << "Invalid target host: " << host << std::endl;
        exit(-1);
    }

    Socket * icmps  = new Socket(dstaddr, SOCKET_ICMP, SOCKTYPE_RAW, SOCKET_ICMP);
    icmps->init(false);

    dropPriv();

    neticmp_h  * req     = NULL;
    icmp_ts    * its     = NULL;
    char       * wptr    = NULL;
    char       * wbuff   = NULL;
    bool         sendReq = true;

    sockaddr_in  csock;
    ipv4addr_t   addr;
    size_t       sz, buflen, idsz;
    ssize_t      wt, rd;
    int          cnt, sent, rcvd;
    float        mstot, avg;

    cnt    = 1;
    sent   = 0;
    rcvd   = 0;
    mstot  = 0.0;
    avg    = 0.0;
    buflen = 2048;
    idsz   = sizeof(neticmp_h) + sizeof(icmp_ts);

    CircularBuffer * rbuff = new CircularBuffer(buflen);

    wbuff       = (char*) malloc(idsz);
    req         = (neticmp_h*) wbuff;
    its         = (icmp_ts*) wbuff + sizeof(icmp_ts);

    req->type   = ICMP_ECHO;
    req->code   = 0;
    req->chksum = 0;
    req->id     = Pid;
    req->seq    = 0;

    if ( count > 0 )
        cnt = count;

    std::cout << "Sending ";
    if ( count > 0 )
        std::cout << "(" << count << ") ";
    std::cout << "ICMP echo requests from " << CidrUtils::ntop(srcaddr)
        << " to " << CidrUtils::ntop(dstaddr) << " (" << host << ")" 
        << std::endl;


    while ( ! Alarm )
    {
        rbuff->reset();

        if ( ! getTimeOfDay(tvin) )
            errorOut("error in gettime");

        float lastsnt = timeDiff(tvin, tvsnt);

        if ( lastsnt >= interval )
            sendReq = true;

        if ( sendReq && cnt > 0 ) {
            tvsnt        = tvin;
            its->secs    = tvin.tv_sec;
            its->usecs   = tvin.tv_usec;
            req->chksum  = 0;
            req->seq++;
            req->chksum  = Socket::IpChkSum((uint16_t*)req, idsz);

            wt = icmps->write(wbuff, idsz);
            if ( wt < 0 )
                errorOut("Error in write " + icmps->getErrorString());
            
            sent++;
            sendReq = false;
            if ( count > 0 )
                cnt--;

            if ( debug )
                std::cout << "Request <" << sent << "> sent" << std::endl;
        }

        sz   = rbuff->writePtrAvailable();
        wptr = rbuff->getWritePtr(&sz);
        if ( wptr == NULL )
            errorOut("Error in writing to rbuff");

        rd   = icmps->readFrom(wptr, sz, csock);
        addr = csock.sin_addr.s_addr;
        if ( rd < 0 )
            errorOut("Error in readFrom " + icmps->getErrorString());

        rbuff->setWritePtr(rd);

        if ( rd > 0 && addr == dstaddr )
        {
            IcmpResponse   response;
            
            if ( ! getTimeOfDay(tvin) )
                errorOut("error in gettime");

            rd = readIcmpHeader(rbuff, response);

            if ( rd > 0 && response.icmph.id == Pid ) {
                sz      = rbuff->readAvailable();
                rcvd++;

                if ( sz == sizeof(icmp_ts) ) {
                    timeval   tv;
                    char    * idf = rbuff->getReadPtr(&sz);
                    icmp_ts * ist = (icmp_ts*) idf;
                    tv.tv_sec     = ist->secs;
                    tv.tv_usec    = ist->usecs;

                    if ( debug ) 
                        std::cout << " Received data field in echo response" << std::endl;

                    rbuff->setReadPtr(sz);
                }

                float ms = timeDiff(tvin, tvsnt);

                std::cout << (rd+sz) << " bytes from " << CidrUtils::ntop(addr)
                    << ": seq=" << response.icmph.seq << " time=" << ms << " ms" 
                    << std::endl;

                mstot += ms;
                avg    = (float) mstot / rcvd;

                if ( debug )
                    std::cout << " mstot=" << mstot << " rcvd = " << rcvd << std::endl;
            }
        } 
        
        if ( cnt == 0 ) {
            if ( rcvd == cnt )
                break;
            if ( tvo.tv_sec == 0 )
                tvo.tv_sec = tvin.tv_sec;
            else if ( (tvin.tv_sec - tvo.tv_sec) > timeout )
                break;
        }

        ::usleep(1000);
    }

    float loss;
    if ( rcvd == sent )
        loss = 0.0;
    else
        loss = (  100.0 - ( ((float) rcvd / (float) sent) * 100.0) );

    std::cout << std::endl << "Results:" << std::endl;
    std::cout << "  Sent " << sent << " requests, received " << rcvd 
        << ": Loss=" << loss << "% : Avg Time= " << avg << " ms " 
        << std::endl;

    return 0;

}



