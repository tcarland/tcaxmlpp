#define _TNMSD_TNMSCLIENT_CPP_

#include "TnmsClient.h"

#include "TnmsSubscriber.h"
#include "TnmsMessageHandler.h"
#include "AuthClient.h"
#include "TnmsAuthRequest.h"
#include "TnmsAuthReply.h"

#include "LogFacility.h"
#include "StringUtils.h"
#include "CidrUtils.h"
using namespace tcanetpp;


namespace tnmsCore {


TnmsClient::TnmsClient ( TnmsTree * tree )
    : TnmsSocket(new TnmsMessageHandler(tree, this)),
      _tree(tree),
      _treeNotifier(new TnmsSubscriber()),
      _isAgent(false),
      _isMirror(true)
{
}


TnmsClient::TnmsClient ( TnmsTree * tree, AuthClient * auth,
                         BufferedSocket * sock, bool isAgent )
    : TnmsSocket(sock, new TnmsMessageHandler(tree, this)),
      _tree(tree),
      _treeNotifier(new TnmsSubscriber()),
      _auth(auth),
      _isAgent(isAgent),
      _isMirror(false)
{
}


TnmsClient::~TnmsClient()
{
    delete _treeNotifier;
}


int
TnmsClient::send ( const time_t & now )
{
    int   wt, wtt = 0;
    bool  queued  = true;
    int   qsz     = 0;

    TnmsAdd     add;
    TnmsRemove  remove;

    TreeUpdateSet::iterator  uIter;
    TreeRemoveSet::iterator  rIter;

    wt  = TnmsSocket::send(now);

    if ( wt < 0 )
        return wt;

    // ADDs
    TreeUpdateSet & adds = _treeNotifier->adds;
    qsz = adds.size();
    for ( uIter = adds.begin(); uIter != adds.end(); ) 
    {
        TnmsTree::Node * node = *uIter;

        if ( node->getValue().erase ) {
            adds.erase(uIter++);
            continue;
        }

        add    = TnmsAdd(node->getValue().metric.getElementName());
        queued = this->sendMessage(&add);

        if ( queued ) {
            adds.erase(uIter++);
        } else {
            LogFacility::LogMessage("TnmsClient::send() error: " + this->getErrorStr());
            break;
        }
    }
    if ( qsz > 0 && adds.size() == 0 )
        this->setLastRecord();

    // REMOVEs
    TreeRemoveSet & removes = _treeNotifier->removes;
    qsz  = removes.size();
    if ( queued && adds.size() == 0 )
    {
        for ( rIter = removes.begin(); rIter != removes.end(); ) 
        {
            remove = TnmsRemove(*rIter);
            queued = this->sendMessage(&remove);

            if ( queued ) {
                removes.erase(rIter++);
            } else {
                LogFacility::LogMessage("TnmsClient::send() error: " + this->getErrorStr());
                break;
            }
        }
        if ( qsz > 0 && removes.size() == 0 )
            this->setLastRecord();
    }

    // UPDATES
    TreeUpdateSet & updates = _treeNotifier->updates;
    qsz = updates.size(); 
    if ( queued && removes.size() == 0 ) 
    {
        for ( uIter = updates.begin(); uIter != updates.end(); ) 
        {
            TnmsTree::Node * node = *uIter;

            if ( node->getValue().erase ) {
                updates.erase(uIter++);
                continue;
            }

            TnmsMetric & m = node->getValue().metric;
            queued         = this->sendMessage(&m);

            if ( queued ) {
                updates.erase(uIter++);
            } else {
                LogFacility::LogMessage("TnmsClient::send() error: " + this->getErrorStr());
                break;
            }
        }
        if ( qsz > 0 && updates.size() == 0 )
            this->setLastRecord();
    }

    wtt = TnmsSocket::send(now);

    if ( wtt < 0 )
        return wtt;
    else if ( (wt+wtt) > 0 )
        LogFacility::LogMessage("TnmsClient::send(): " + StringUtils::toString(wt+wtt));

    return(wtt + wt);
}


void
TnmsClient::close()
{
    TnmsSocket::close();
}

bool
TnmsClient::isAgent() const
{
    return this->_isAgent;
}

bool
TnmsClient::isMirror() const
{
    return this->_isMirror;
}

bool
TnmsClient::isMirrorClient() const
{
    return this->isMirror();
}

const std::string&
TnmsClient::getConfig() const
{
    return this->_xmlConfig;
}

bool
TnmsClient::inTreeSend() const
{
    return this->_inTreeSend;
}


void
TnmsClient::inTreeSend ( bool insend )
{
    _inTreeSend = insend;
}

bool
TnmsClient::login ( const std::string & name, const std::string & pw ) 
{
    return TnmsSocket::login(name, pw);
}


bool
TnmsClient::login()
{
    if ( this->_login.empty() )
        return false;
    return TnmsSocket::login(this->_login, this->_authkey);
}


void
TnmsClient::AuthReplyHandler ( const TnmsAuthReply & reply )
{
    LogFacility::Message msg;

    TnmsSocket::authReply(reply);

    msg << "TnmsClient::AuthReplyHandler(): " << reply.getElementName();

    if ( this->isAuthorized() ) {
        msg << " is authorized";
        if ( ! reply.authData().empty() )
            this->_xmlConfig = reply.authData();
    } else
        msg << " is not authorized";

    LogFacility::LogMessage(msg.str());

    return;
}

/*
 *  Clients themselves generally would not receive/handle an auth 
 *  request which would specifically be ignored/dropped via the 
 *  MessageHandler::AuthRequestHandler() interface. Alternatively,
 *  server components can proxy client authorization and this 
 *  method provides a default handler for forwarding the request
 *  to the AuthClient. 
 *
 */
void
TnmsClient::AuthRequestHandler ( const TnmsAuthRequest & request )
{
    std::string              login, authname, ipaddr;
    std::string::size_type   delim;

    login  = request.getElementName();
    ipaddr = this->getAddrStr();
    delim  = login.find_first_of(':', 0);

    if ( delim == std::string::npos )
        _login  = login;
    else
        _login  = login.substr(0, delim);

    delim  = _login.find_first_of('@', 0);

    if ( delim != std::string::npos )
        _login  = _login.substr(0, delim);

    _authname = _login;
    _authname.append("@").append(ipaddr);
    _authname.append(":").append(StringUtils::toString(this->getHostPort()));

    TnmsAuthRequest req(_authname, request.agent_key());
    req.ipaddr(this->getAddr());

    if ( _auth )
    {
        _auth->authClient(this, req);
    }
    else  // this should just as well be fatal.// throw Exception("Invalid Auth handle");
    {
        LogFacility::LogMessage("TnmsClient::AuthRequestHandler() Invalid Auth handle");
        TnmsAuthReply reply(_authname);
        reply.authResult(AUTH_NO_RESULT);
        reply.authReason("Auth not available");
        this->sendMessage(&reply);
    }

    return;
}

void
TnmsClient::setSubscriber ( TnmsSubscriber * subscriber )
{
    if ( NULL != subscriber )
    {
        delete _treeNotifier;
        _treeNotifier = subscriber;
    }
}

TnmsSubscriber*
TnmsClient::getSubscriber()
{
    return this->_treeNotifier;
}

TnmsTree*
TnmsClient::getTree()
{
    return this->_tree;
}


} // namespace


// _TNMSD_TNMSCLIENT_CPP_

