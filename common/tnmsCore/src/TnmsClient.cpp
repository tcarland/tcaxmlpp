#define _TNMSD_TNMSCLIENT_CPP_


#include "TnmsClient.h"
#include "TnmsMessageHandler.h"


namespace tnmsCore {


TnmsClient::TnmsClient ( TnmsTree * tree )
    : TnmsSocket(),
      _messageHandler(new TnmsMessageHandler(tree, this)),
      _tree(tree),
      _isAgent(false),
      _isMirror(true)
{
    this->setMessageHandler(this->_messageHandler);
}


TnmsClient::TnmsClient ( TnmsTree * tree, BufferedSocket * sock, bool isAgent )
    : TnmsSocket(sock),
      _messageHandler(new TnmsMessageHandler(tree, this)),
      _tree(tree),
      _isAgent(isAgent),
      _isMirror(false)
{
    this->setMessageHandler(this->_messageHandler);
}


TnmsClient::~TnmsClient()
{
    delete _messageHandler;
}


int
TnmsClient::send()
{
    int   wt, wtt = 0;
    bool  queued  = true;

    UpdateSet::iterator  uIter;
    RemoveSet::iterator  rIter;

    wt  = TnmsSocket::send();
    if ( wt < 0 )
        return wt;

    // ADDs
    TnmsAdd  add;
    int addsz = _adds.size();
    for ( uIter = _adds.begin(); uIter != _adds.end(); ) {
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

    if ( addsz && _adds.size() == 0 )
        this->setLastRecord();

    TnmsRemove remove;
    if ( queued && _adds.size() == 0 ) {

        for ( rIter = _removes.begin(); rIter != _removes.end(); ) {
            remove = TnmsRemove(*rIter);
            queued = this->sendMessage(&remove);

            if ( queued )
                _removes.erase(rIter++);
            else
                break;
        }
    }

    if ( queued && _removes.size() == 0 ) {
        
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
    }

    wtt = TnmsSocket::send();

    if ( wtt < 0 )
        return wtt;

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



} // namespace


// _TNMSD_TNMSCLIENT_CPP_

