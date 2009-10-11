#ifndef _TNMSDB_ARCHIVESUBSCRIBER_H_
#define _TNMSDB_ARCHIVESUBSCRIBER_H_

#include "TnmsSubscriber.h"
#include "TnmsTree.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


namespace tnmsdb {


class ArchiveSubscriber : public TnmsSubscriber {

  public:

    ArchiveSubscriber();

    virtual ~ArchiveSubscriber();


    virtual bool  queueAdd    ( TnmsTree::Node * node );
    virtual bool  queueUpdate ( TnmsTree::Node * node );
    virtual bool  queueRemove ( TnmsTree::Node * node );

    int           lock();
    int           trylock();
    int           unlock();

  protected:

    ThreadLock    _mutex;

};

}  // namespace 

#endif  // _TNMSDB_ARCHIVESUBSCRIBER_H_
