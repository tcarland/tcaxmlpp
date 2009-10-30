#ifndef _TNMSCONSOLE_CLIENTIOHANDLER_H_
#define _TNMSCONSOLE_CLIENTIOHANDLER_H_

#include <set>

#include "EventHandlers.hpp"
#include "Exception.hpp"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;

namespace tnmsconsole {


typedef std::map<std::string, TnmsClient*>     ClientMap;


class ClientIOHandler : public EventIOHandler {

  public:

    typedef std::map<std::string, TnmsClient*>     ClientMap;
    typedef std::set<TnmsClient*>                  ClientSet;
    typedef ClientMap::iterator                    iterator;
    typedef ClientMap::const_iterator              const_iterator;
   

  public:

    ClientIOHandler ( TnmsTree * tree, AuthClient * auth = NULL )
        throw ( Exception );

    virtual ~ClientIOHandler();

    /*  EventIOHandler */

    virtual void   handle_accept  ( const EventIO & io ) {}
    virtual void   handle_read    ( const EventIO & io );
    virtual void   handle_write   ( const EventIO & io );
    virtual void   handle_shut    ( const EventIO & io );
    virtual void   handle_close   ( const EventIO & io );
    virtual void   handle_destroy ( const EventIO & io );
    

    virtual bool   readable       ( const EventIO & io );
    virtual bool   writeable      ( const EventIO & io );


    /*  ClientIOHandler */

    void           timeout        ( const EventTimer  & timer );

    TnmsClient*    find           ( const std::string & name );

    bool           insert         ( const std::string & name,
                                    TnmsClient        * client );
    void           erase          ( const std::string & name );

    iterator       begin();
    iterator       end();
    const_iterator begin() const;
    const_iterator end()   const;
    size_t         size()  const;
    void           clear();

    void           listClients();

  protected:

    TnmsTree *               _tree;
    AuthClient *             _auth;

    ClientMap                _clients;
    ClientSet                _clientSet;

    std::string              _user;
    std::string              _pw;

};

}  // namespace 

#endif //  _TNMSCONSOLE_CLIENTIOHANDLER_H_

