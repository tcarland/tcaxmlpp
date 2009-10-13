#ifndef _TNMSDB_ARCHIVESUBSCRIBER_H_
#define _TNMSDB_ARCHIVESUBSCRIBER_H_

#include <string>
#include <queue>

#include "TreeSubscriber.hpp"
#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;

#include "ThreadLock.h"
using namespace tcanetpp;


namespace tnmsdb {


#define ARCHIVE_QUEUESIZE  (65535 * 16)


class ArchiveSubscriber : public TreeSubscriber {

  public:

    ArchiveSubscriber();

    virtual ~ArchiveSubscriber();


    virtual bool  queueAdd    ( TnmsTree::Node * node );
    virtual bool  queueUpdate ( TnmsTree::Node * node );
    virtual bool  queueRemove ( TnmsTree::Node * node );

    int           lock();
    int           trylock();
    int           unlock();


  public:

    std::queue<TnmsMetric>       _metricq;
    std::queue<std::string>      _removeq;

  protected:

    ThreadLock                   _mutex;

};

}  // namespace 

#endif  // _TNMSDB_ARCHIVESUBSCRIBER_H_
