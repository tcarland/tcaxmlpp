#ifndef _TNMSCLIENTIOTHREAD_H_
#define _TNMSCLIENTIOTHREAD_H_

#include <map>



#include "Thread.h"
#include "ThreadLock.h"
#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;


class TnmsClientIOHandler;


class TnmsClientIOThread : public Thread {

  public:

    TnmsClientIOThread ( ThreadLock * tree );
    
    virtual ~TnmsClientIOThread();


    virtual void  run();
    
    void          timeout      ( const EventTimer * timer );

    bool          addClient    ( TnmsClient * client );
    void          removeClient ( TnmsClient * client );

  public:

    typedef std::map<TnmsClient*, evid_t>   ClientEventMap;


  protected:

    class ClientIOTimer : public EventTimerHandler {
      public:

        explicit ClientIOTimer ( TnmsClientIOThread * iothread_ )
            : iothread(iothread_)
        {}

        void timeout ( const EventTimer * timer );

        TnmsClientIOThread * iothread;
    };


  private:

    EventManager *       _evmgr;
    ThreadLock *         _mutex;

    TnmsClientIOHandler* _clientHandler;

    ClientEventMap       _clients;
};


#endif  // _TNMSCLIENTIOTHREAD_H_

