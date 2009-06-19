#define _TNMSD_TNMSCLIENT_CPP_


#include "TnmsClient.h"

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
      _isAgent(false),
      _isMirror(true)
{
}


TnmsClient::TnmsClient ( TnmsTree * tree, AuthClient * auth,
                         BufferedSocket * sock, bool isAgent )
    : TnmsSocket(sock, new TnmsMessageHandler(tree, this)),
      _tree(tree),
      _auth(auth),
      _isAgent(isAgent),
      _isMirror(false)
{
}


TnmsClient::~TnmsClient()
{
}


int
TnmsClient::send()
{
    int   wt, wtt = 0;
    bool  queued  = true;
    int   qsz     = 0;

    TnmsAdd     add;
    TnmsRemove  remove;

    UpdateSet::iterator  uIter;
    RemoveSet::iterator  rIter;

    wt  = TnmsSocket::send();

    if ( wt < 0 )
        return wt;

    // ADDs
    qsz = _adds.size();
    for ( uIter = _adds.begin(); uIter != _adds.end(); ) 
    {
        TnmsTree::Node * node = *uIter;

        if ( node->getValue().erase ) {
            _adds.erase(uIter++);
            continue;
        }

        add = TnmsAdd(node->getValue().metric.getElementName());
        if ( (queued = this->sendMessage(&add)) == true )
            _adds.erase(uIter++);
        else
            break;
    }
    if ( qsz && _adds.size() == 0 )
        this->setLastRecord();

    // REMOVEs
    qsz  = _removes.size();
    if ( queued && _adds.size() == 0 )
    {
        for ( rIter = _removes.begin(); rIter != _removes.end(); ) {
            remove = TnmsRemove(*rIter);
            queued = this->sendMessage(&remove);

            if ( queued )
                _removes.erase(rIter++);
            else
                break;
        }
        if ( qsz && _removes.size() == 0 )
            this->setLastRecord();
    }

    // UPDATES
    qsz = _updates.size(); 
    if ( queued && _removes.size() == 0 ) 
    {
        for ( uIter = _updates.begin(); uIter != _updates.end(); ) {
            TnmsTree::Node * node = *uIter;

            if ( node->getValue().erase ) {
                _updates.erase(uIter++);
                continue;
            }

            queued = this->sendMessage(&node->getValue().metric);

            if ( queued )
                _updates.erase(uIter++);
            else
                break;
        }
        if ( qsz && _updates.size() == 0 )
            this->setLastRecord();
    }

    wtt = TnmsSocket::send();

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

void
TnmsClient::queueAdd ( TnmsTree::Node * node )
{
    if ( _removes.erase(node->getValue().metric.getElementName()) )
        return;
    _adds.insert(node);
    return;
}

void
TnmsClient::queueRemove ( TnmsTree::Node * node )
{
    _adds.erase(node);
    _updates.erase(node);
    _removes.insert(node->getValue().metric.getElementName());
}


void
TnmsClient::queueUpdate ( TnmsTree::Node * node )
{
    _updates.insert(node);
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
        //time_t  now = ::time(NULL);
        //init client stats
    }
}

// agent login credentials:  'tnmsagent@ipaddr:/my/agent/id'
// client login credentials: 'username@ipaddr:'
void
TnmsClient::AuthRequestHandler ( const TnmsAuthRequest & request )
{
    std::string              login, authname;
    std::string::size_type   delim;

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
        this->_login    = request.getElementName();
        this->_authname = _login;

        _authname.append("@").append(this->getHostStr());
        _authname.append(":");

        TnmsAuthRequest req(_authname, request.agent_key());

        // init client stats

        if ( _auth ) {
            _auth->authClient(this, req);
        } else {
            LogFacility::LogMessage("TnmsClient::AuthRequestHandler() Invalid Auth handle");
            // no auth svc, for now let it thru
            TnmsAuthReply reply(_authname);
            reply.authResult(AUTH_NO_RESULT);
            this->sendMessage(&reply);
        }
    }
}




} // namespace


// _TNMSD_TNMSCLIENT_CPP_

