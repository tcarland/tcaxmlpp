#ifndef _FWGEN_FWLOGREADER_H_
#define _FWGEN_FWLOGREADER_H_

#include <string>

#include "Thread.h"
#include "SynchronizedQueue.hpp"
using namespace tcanetpp;

#include "FwLogEntry.h"


namespace fwgen {


typedef SynchronizedQueue<FwLogEntry>  FwLogQueue;


class FwLogReader : public tcanetpp::Thread {

  public:

    FwLogReader ( const std::string & logfile, bool tail = false );

    virtual ~FwLogReader() {}


    virtual void run();


    FwLogQueue*  getQueue();

    void         setMatch ( const std::string & match );


  private:

   FwLogQueue             _squeue;

   std::string            _logfile;
   std::string            _match;
   bool                   _tail;

};


} // namespace


#endif // _FWGEN_FWLOGREADER_H_

