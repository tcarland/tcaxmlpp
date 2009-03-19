#ifndef _TESTMESSAGEHANDLER_HPP_
#define _TESTMESSAGEHANDLER_HPP_

#include <iostream>
#include <string>

#include "MessageHandler.hpp"
using namespace tnmsCore;


namespace tnmsTest {


class TestMessageHandler : public MessageHandler {

  public:

    virtual ~TestMessageHandler() {}

    /*  Messaging Callbacks  */
    virtual void  AddHandler         ( const TnmsAdd     & add )
    {
        std::cout << "AddHandler(): " << add.getElementName() << std::endl;
    }
    virtual void  RemoveHandler      ( const TnmsRemove  & remove )
    {
        std::cout << "RemoveHandler(): " << remove.getElementName() << std::endl;
    }
    virtual void  MetricHandler      ( const TnmsMetric  & metric )
    {
        std::cout << "MetricHandler(): " << metric.getElementName() << std::endl;
    }
    virtual void  RequestHandler     ( const TnmsRequest & request ) 
    {
        std::cout << "RequestHandler(): " << request.getElementName() << std::endl;
    }
    virtual void  SubscribeHandler   ( const std::string & name )
    {
        std::cout << "SubscribeHandler(): " << name << std::endl;
    }
    virtual void  UnsubscribeHandler ( const std::string & name )
    {
        std::cout << "UnsubscribeHandler(): " << name << std::endl;
    }
    virtual void  StructureHandler   ( bool  subscribe )
    {
        std::cout << "StructureHandler()" << std::endl;
    }
    virtual void  AuthRequestHandler ( const TnmsAuthRequest & request )
    {
        std::cout << "AuthRequestHandler()" << std::endl;
    }
    virtual void  AuthReplyHandler   ( const TnmsAuthReply   & reply )
    {
        std::cout << "AuthReplyHandler()" << std::endl;
    }
    virtual void  PingHandler()
    {
        std::cout << "PingHandler()" << std::endl;
    }
    virtual void  PingReplyHandler()
    {
        std::cout << "PingReplyHandler()" << std::endl;
    }
    virtual void  LastMessageHandler ( int   record_type )
    {
        std::cout << "LastMessageHandler(): " << record_type << std::endl;
    }

};

}

#endif  //  _TESTMESSAGEHANDLER_HPP_
