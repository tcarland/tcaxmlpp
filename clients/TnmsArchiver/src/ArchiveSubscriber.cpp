#define _TNMSDB_ARCHIVESUBSCRIBER_CPP_

#include "ArchiveSubscriber.h"


namespace tnmsdb {


ArchiveSubscriber::ArchiveSubscriber()
{}

ArchiveSubscriber::~ArchiveSubscriber() 
{}


bool
ArchiveSubscriber::queueAdd ( TnmsTree::Node * node ) 
{
    TnmsMetric & metric = node->getValue().metric;

    metricq.push(metric);

    return true;
}


bool
ArchiveSubscriber::queueUpdate ( TnmsTree::Node * node ) 
{
    TnmsMetric & metric = node->getValue().metric;

    metricq.push(metric);

    return true;
}


bool
ArchiveSubscriber::queueRemove( TnmsTree::Node * node ) 
{
    TnmsMetric & metric = node->getValue().metric;

    removeq.push(metric.getElementName());

    return true;
}


int
ArchiveSubscriber::lock() 
{
    return _mutex.lock();    
}


int
ArchiveSubscriber::trylock() 
{
    return _mutex.trylock();
}


int
ArchiveSubscriber::unlock() 
{
    return _mutex.unlock();
}


} // namespace

// _TNMSDB_ARCHIVESUBSCRIBER_CPP_

