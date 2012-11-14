/**  
  *  A test app demonstrating use of the Thread and SynchronizedQueue objects
  *  x  number of input threads will place items on the queue, with
  *  y  number of output threads reading the queue.
  *
 **/
#define _TCANET_THREADTEST_CPP

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include <set>

#include "EventManager.h"
#include "LogFacility.h"  
#include "Thread.h"
#include "SynchronizedQueue.hpp"
#include "StringUtils.h"
using namespace tcanetpp;


typedef SynchronizedQueue<std::string>  StringQueue;
typedef std::set<Thread*>               ThreadSet;

static bool Alarm = false;


class InputThread : public Thread {

  public:

    InputThread ( StringQueue * q, uint32_t ival ) 
        : _queue(q), _ival(ival) 
    {
    }
    virtual ~InputThread() {}


    virtual void  run()
    {
        uint64_t    stime, val;
        std::string msg;

        stime = msectoevu(_ival);
        stime = evutonsec(stime);
        val   = 0;
        
        LogFacility::LogMessage("Starting " + this->threadName());

        while ( ! this->_Alarm ) 
        {
            EventManager::NanoSleep(stime);

            msg = this->threadName();
            msg.append(":").append(StringUtils::toString(val));

            _queue->push(msg);

            val++;
        }
            
        return;
    }

    virtual void finished()
    {
        std::string name = this->threadName();
        name.append(" Finished");
        LogFacility::LogMessage(name);
    }

  protected:

    StringQueue * _queue;
    uint32_t      _ival;
};


class OutputThread : public Thread {

  public:

    OutputThread ( StringQueue * q, uint32_t ival ) 
        : _queue(q), _ival(ival) 
    {
    }
    virtual ~OutputThread() {}

    virtual void  run()
    {
        uint64_t    stime;
        std::string name, msg;

        stime = msectoevu(_ival);
        stime = evutonsec(stime);
        name  = this->threadName();
        name.append(" ");

        LogFacility::LogMessage("Starting " + name);
        
        while ( ! this->_Alarm ) 
        {
            EventManager::NanoSleep(stime);
            _queue->pop(msg);
            LogFacility::LogMessage(name + msg);
        }
            
        return;
    }
    
    virtual void finished()
    {
        std::string name = this->threadName();
        name.append(" Finished");
        LogFacility::LogMessage(name);
    }


  protected:

    StringQueue * _queue;
    uint32_t      _ival;
};




void sigHandler ( int sig ) {
    if ( sig == SIGTERM || sig == SIGINT )
	Alarm = true;
    return;
}

void usage()
{
    std::cout << "Usage: thtest [-h] [-i <num>] [-o <num>] [-x <num>] [-y <num>]" << std::endl
        << "    -i <num>  :  Input Thread wait interval." << std::endl
        << "    -o <num>  :  Output Thread wait interval." << std::endl
        << "    -x <num>  :  Number of input threads." << std::endl
        << "    -y <num>  :  Number of output threads." << std::endl;
    return;
}



int main ( int argc, char **argv )
{
    char optChar;
    int  xCnt, yCnt, xInt, yInt;

    xCnt = 2;
    yCnt = 5;
    xInt = 1000;
    yInt = 3000;

    while ( (optChar = ::getopt(argc, argv, "hi:o:x:y:")) != EOF )
    {
        switch ( optChar ) {
            case 'h':
                usage();
                return 0;
                break;
            case 'i':
                xInt = atoi(optarg);
                break;
            case 'o':
                yInt = atoi(optarg);
                break;
            case 'x':
                xCnt = atoi(optarg);
                break;
            case 'y':
                yCnt = atoi(optarg);
                break;
            default:
                break;
        }
    }

    ::signal(SIGINT, &sigHandler);
    ::signal(SIGTERM, &sigHandler);
    LogFacility::InitThreaded();
    LogFacility::AddLogStream("stdout", "thtest", &std::cout); 

    StringQueue * queue = new StringQueue();
    ThreadSet     ithreads, othreads;

    InputThread * th    = NULL;
    std::string   iname = "InputThread";

    for ( int i = 0; i < xCnt; i++ )
    {
        std::string thname = iname;
        thname.append("_").append(StringUtils::toString(i));

        th = new InputThread(queue, xInt);
        th->threadName(thname);
        th->start();

        ithreads.insert(th);
    }

    OutputThread * oth = NULL;
    std::string    oname = "OutputThread";

    for ( int i = 0; i < yCnt; i++ )
    {
        std::string thname = oname;
        thname.append("_").append(StringUtils::toString(i));

        oth = new OutputThread(queue, yInt);
        oth->threadName(thname);
        oth->start();

        othreads.insert(th);
    }

    size_t qsz;
    while ( ! Alarm ) 
    {
        sleep(2);
        qsz = queue->size();

        LogFacility::LogMessage("Queue size is " + StringUtils::toString(qsz));
    }
    
    LogFacility::LogMessage("Alarm received, stopping threads");

    ThreadSet::iterator  tIter;

    uint64_t ss = 100000000;
    for ( tIter = ithreads.begin(); tIter != ithreads.end(); ++tIter )
    {
        InputThread * ith = (InputThread*) *tIter;
        ith->setAlarm();
        EventManager::NanoSleep(ss);
        delete ith;
    }

    for ( tIter = othreads.begin(); tIter != othreads.end(); ++tIter )
    {
        OutputThread * oth = (OutputThread*) *tIter;
        oth->setAlarm();
        EventManager::NanoSleep(ss);
        delete oth;
    }

    delete queue;

    return 0;
}



