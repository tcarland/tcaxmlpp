#ifndef _TNMSD_TNMSCLIENT_H_
#define _TNMSD_TNMSCLIENT_H_

#include "TnmsSocket.h"
using namespace tnmsCore;

namespace tnmsd {

class TnmsClient : public TnmsSocket {

  public:

    TnmsClient ( TnmsTree * tree );

    virtual ~TnmsClient();

    bool            isMirrorClient();
    
    void            queueAdd();
    void            queueUpdate();
    void            queueRemove();

    virtual void    close();

  private:

    TnmsTree*            _tree;
    TnmsMessageHandler*  _msgHandler;
};


} // namespace

#endif  //  _TNMSD_TNMSCLIENT_H_
