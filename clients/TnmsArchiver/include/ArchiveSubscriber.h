#ifndef _TNMSDB_ARCHIVESUBSCRIBER_H_
#define _TNMSDB_ARCHIVESUBSCRIBER_H_

#include <string>

#include "TreeSubscriber.hpp"
#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;

#include "SynchronizedQueue.hpp"
using namespace tcanetpp;


namespace tnmsdb {


#define ARCHIVE_QUEUESIZE  (65535 * 16)


class ArchiveSubscriber : public TreeSubscriber {

  public:

    ArchiveSubscriber ( int queuesz = ARCHIVE_QUEUESIZE );

    virtual ~ArchiveSubscriber();


    virtual bool  queueAdd    ( TnmsTree::Node * node );
    virtual bool  queueUpdate ( TnmsTree::Node * node );
    virtual bool  queueRemove ( TnmsTree::Node * node );

    int           lock();
    int           trylock();
    int           unlock();

  protected:

    SynchronizedQueue<TnmsMetric>       _metricq;
    SynchronizedQueue<std::string>      _removeq;

};

}  // namespace 

#endif  // _TNMSDB_ARCHIVESUBSCRIBER_H_
