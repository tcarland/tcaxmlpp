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

    TnmsClientIOThread ( TnmsTree * tree, ThreadLock * rlock );
    
    virtual ~TnmsClientIOThread();


    virtual void  run();
    
    void          timeout      ( const EventTimer & timer );

    bool          addClient    ( TnmsClient * client );
    void          removeClient ( TnmsClient * client );

  public:

    typedef std::map<TnmsClient*, evid_t>   ClientEventMap;


  protected:

    class ClientIOTimer : public EventTimerHandler 
    {
        public:
        explicit ClientIOTimer ( TnmsClientIOThread * iothread_ )
            : iothread(iothread_)
        {}
        virtual ~ClientIOTimer() {}

        void timeout  ( const EventTimer & timer );
        void finished ( const EventTimer & timer ) {}

        TnmsClientIOThread *  iothread;
    };


  private:

    EventManager *          _evmgr;
    TnmsTree *              _tree;
    ThreadLock *            _mutex;

    TnmsClientIOHandler *   _clientHandler;

    ClientEventMap          _clients;

};


#endif  // _TNMSCLIENTIOTHREAD_H_

