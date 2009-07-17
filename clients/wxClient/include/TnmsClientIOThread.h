#ifndef _TNMSCLIENTIOTHREAD_H_
#define _TNMSCLIENTIOTHREAD_H_

#include <map>


#include "TnmsTree_R.hpp"

#include "Thread.h"
#include "EventManager.h"
using namespace tcanetpp;

#include "TnmsClient.h"
using namespace tnmsCore;



class TnmsClientIOThread : public Thread {

  public:

    TnmsClientIOThread ( TnmsTree_R * tree );
    
    virtual ~TnmsClientIOThread();


    virtual void run();


    void          addClient    ( TnmsClient * client );
    void          removeClient ( TnmsClient * client );

  public:

    typedef std::map<evid_t, TnmsClient*>   ClientEventMap;

  private:

    EventManager *      _evmgr;
    TnmsTree_R *        _tree;

    ClientEventMap      _clients;
};


#endif  // _TNMSCLIENTIOTHREAD_H_

