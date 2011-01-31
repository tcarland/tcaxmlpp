#ifndef _TNMSCLIENT_CLIENTIOTHREAD_H_
#define _TNMSCLIENT_CLIENTIOTHREAD_H_

#include <map>

#include "Thread.h"
#include "ThreadLock.h"
#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;

namespace tnmsclient {

class ClientIOHandler;
class ClientTreeMutex;


class ClientIOThread : public Thread {

  public:

    ClientIOThread ( ClientTreeMutex * tree );
    
    virtual ~ClientIOThread();


    virtual void  run();
    
    void          timeout      ( const EventTimer & timer );

    bool          addClient    ( TnmsClient * client );
    bool          removeClient ( TnmsClient * client );


  public:

    typedef std::map<TnmsClient*, evid_t>   ClientEventMap;


  protected:

    class ClientIOTimer : public EventTimerHandler  {
      public:
        explicit ClientIOTimer ( ClientIOThread * iothread_ )
            : iothread(iothread_)
        {}

        virtual ~ClientIOTimer() {}

        void  timeout    ( const EventTimer & timer );
        void  finished   ( const EventTimer & timer ) {}

      public:

        ClientIOThread *  iothread;
    };


  private:

    EventManager *          _evmgr;
    ClientTreeMutex *       _mtree;
    ClientIOHandler *       _clientHandler;
    ClientEventMap          _clients;

};

} // namespace

#endif  // _TNMSCLIENT_CLIENTIOTHREAD_H_

