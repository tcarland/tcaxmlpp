#ifndef _TNMSAUTH_SOAPTHREAD_H_
#define _TNMSAUTH_SOAPTHREAD_H_


#include "Thread.h"
using namespace tcanetpp;

namespace tnmsauth {

class SoapClient;


class SoapThread : public Thread {

  public:

    SoapThread ( SoapClient * client, const time_t & now );

    virtual ~SoapThread();


    virtual void run();

    time_t       started();


  private:

    SoapClient *  _client;
    time_t        _started;

};

}  // namespace


#endif  //  _TNMSAUTH_SOAPTHREAD_H_
 
