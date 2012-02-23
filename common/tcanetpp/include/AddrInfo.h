#ifndef _TCANETPP_ADDRINFO_H_
#define _TCANETPP_ADDRINFO_H_

#include <string>
#include "tcanetpp_ip.h"



namespace tcanetpp {


class AddrInfo {

  public:

    class AddrInfoFactory {
      public:
        virtual ~AddrInfoFactory() {}
        virtual AddrInfo* operator() ( struct addrinfo * ai );
    };

    static AddrInfoFactory  factory;

  public:

    AddrInfo();

    AddrInfo ( const std::string & name, 
               const std::string & service = "" );

    virtual ~AddrInfo();


  protected:

    AddrInfo ( struct addrinfo   * ai );


  public:

    bool        isValid() const;

    addrinfo*   begin();
    addrinfo*   next();

    addrinfo*   get();

    int         getFlags() const;
    bool        setFlags ( int flags );

    int         getFamily() const;
    bool        setFamily ( int family );

    int         getSocktype() const;
    bool        setSocktype ( int type );

    int         getProtocol() const;
    bool        setProtocol ( int proto );

    size_t      getAddrlen() const;
    char*       getCanonName() const;

    int         getError() const;

  public:

    /* static factory methods */

    static AddrInfo*  GetAddrInfo ( const std::string & host,
                                    const std::string & svc = "" );

    static AddrInfo*  GetAddrInfo ( const std::string & host,
                                    uint16_t            port );

    /* static methods for getaddrinfo/getnameinfo */

    static int  GetAddrInfo ( const std::string & host,
                              const addrinfo    * hints,
                              addrinfo         ** res );

    static int  GetAddrInfo ( const std::string & host,
                              const std::string & svc,
                              const addrinfo    * hints,
                              addrinfo         ** res );

    static int  GetNameInfo ( const ipv4addr_t  & addr,
                              std::string       & result,
                              int                 flags);

    static int  GetNameInfo ( const ipv6addr_t  & addr,
                              std::string       & result,
                              int                 flags );

    static int  GetNameInfo ( const sockaddr    * sock,
                              socklen_t           salen,
                              std::string       & result,
                              int                 flags );

    /* factory methods for generating addrinfo hints */

    static addrinfo  GetTCPServerHints();
    static addrinfo  GetUDPServerHints();
    static addrinfo  GetTCPClientHints();
    static addrinfo  GetUDPClientHints();


    static int       ai_error;

  protected:

    struct addrinfo*  _ai;
    struct addrinfo*  _nxt;
    int               _err;

};

} // namespace

#endif // _TCANETPP_ADDRINFO_H_

