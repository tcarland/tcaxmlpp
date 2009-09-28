#define _TNMSD_TNMSCLIENT_CPP_

#include "TnmsClient.h"

#include "TnmsSubscriber.h"
#include "TnmsMessageHandler.h"
#include "AuthClient.h"
#include "TnmsAuthRequest.h"
#include "TnmsAuthReply.h"

#include "LogFacility.h"
#include "StringUtils.h"
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
    return _isAgent;
}

bool
TnmsClient::isMirror() const
{
    return _isMirror;
}


bool
TnmsClient::inTreeSend() const
{
    return _inTreeSend;
}


void
TnmsClient::inTreeSend ( bool insend )
{
    _inTreeSend = insend;
}

bool
TnmsClient::login ( const std::string & name, const std::string & pw ) 
{
/*
    std::string  login = TNMS_CLIENT_ID;
    if ( this->isAgent() )
        login = TNMS_AGENT_ID;

    login.append(":").append(name);
*/
    return TnmsSocket::login(name, pw);
}


bool
TnmsClient::login()
{
    return TnmsSocket::login(this->_login, this->_authkey);
}


void
TnmsClient::AuthReplyHandler ( const TnmsAuthReply & reply )
{
    LogFacility::Message msg;

    TnmsSocket::authReply(reply);

    msg << "TnmsClient::AuthReplyHandler(): " << reply.getElementName();

    if ( this->isAuthorized() )
        msg << " is authorized";
    else
        msg << " is not authorized";

    LogFacility::LogMessage(msg.str());

    if ( this->_isMirror ) {
        // track this client
        //time_t  now = ::time(NULL);
        //init client stats
        ;
    }
}

// agent login credentials:  'tnmsagent@ipaddr:/my/agent/id'
// client login credentials: 'username@ipaddr:'
void
TnmsClient::AuthRequestHandler ( const TnmsAuthRequest & request )
{
    std::string              login, authname;
    std::string::size_type   delim;

    login = request.getElementName();
    delim = login.find_first_of(':', 0);

    if ( delim == std::string::npos )
    {
        if ( this->_isAgent )   // invalid login
        {
            TnmsAuthReply reply(login);
            reply.authResult(AUTH_AGENT_INVALID);
            reply.authReason("Invalid Agent login");
            this->sendMessage(&reply);
        } 
        else 
        {
            _login    = "tnmsclient";
            _authname = login;
        }
    } else {
        _login    = login.substr(delim + 1);
        _authname = login.substr(0, delim);
    }

    _authname.append("@").append(this->getAddrStr());
    _authname.append(":").append(_login);

    TnmsAuthRequest req(_authname, request.agent_key());

    if ( _auth )
    {
        _auth->authClient(this, req);
    }
    else
    {
        LogFacility::LogMessage("TnmsClient::AuthRequestHandler() Invalid Auth handle");
        TnmsAuthReply reply(_authname);
        reply.authResult(AUTH_NO_RESULT);
        reply.authReason("Auth not available");
        this->sendMessage(&reply);
    }

/*
    if ( this->_isAgent ) {
        login = request.getElementName();
        delim = login.find_first_of(':', 0);

        if ( delim == std::string::npos ) {
            this->_login    = login;
            this->_authname = "tnmsagent";
        }  else {
            this->_login     = login.substr(delim+1);
            this->_authname  = login.substr(0, delim);
        }

        _authname.append("@").append(this->getAddrStr());
        _authname.append(":").append(this->_login);

        TnmsAuthRequest req(_authname, "");

        if ( this->_auth ) {
            _auth->authClient(this, req);
        } else {
            // no auth svc, for now let it thru
            LogFacility::LogMessage("TnmsClient::AuthRequestHandler() Invalid Auth handle");
            TnmsAuthReply reply(_authname);
            reply.authResult(AUTH_NO_RESULT);
            this->sendMessage(&reply);
        }
    } else {  // client
        _login    = request.getElementName();
        _authname = _login;

        _authname.append("@").append(this->getHostStr());
        _authname.append(":");

        TnmsAuthRequest req(_authname, request.agent_key());

        // init client stats

        if ( _auth ) {
            _auth->authClient(this, req);
        } else {
            LogFacility::LogMessage("TnmsClient::AuthRequestHandler() Invalid Auth handle");
            TnmsAuthReply reply(_authname);
            reply.authResult(AUTH_NO_RESULT);
            this->sendMessage(&reply);
        }
    }
    */
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

