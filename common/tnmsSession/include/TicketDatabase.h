#ifndef _TNMSSESSION_TICKETREPOSITORY_H_
#define _TNMSSESSION_TICKETREPOSITORY_H_

#include <string>
#include <list>
#include <set>
#include <map>


#include "ThreadLock.h"



namespace tnmsSession {



class TicketDatabase {

  public:

    TicketDatabase();
    ~TicketDatabase();


    bool          insert       ( const std::string & username,
                                 const std::string & ticket,
                                 const std::string & ip_addr,
                                 const time_t      & now,
                                 time_t  refresh_interval );

    bool          refresh      ( const std::string & username,
                                 const std::string & ticket,
                                 const std::string & ip_addr,
                                 const time_t      & now );

    bool          expire       ( const std::string & username,
                                 const std::string & ticket,
                                 const std::string & ip_addr );
     
    bool          isAuthentic  ( const std::string & username,
                                 const std::string & ticket,
                                 const std::string & ip_addr );

    void          clearStale   ( std::list<std::string> & ticketlist,
                                 const time_t           & now );

    const
    std::string&  getErrorStr() { return _errStr; }


  private:

    bool          haveTicket   ( const std::string & ticket );


  private: 

    struct Ticket {
        std::string     username;
        std::string     ticket;
        std::string     ipaddr;
        time_t          expire;
        time_t          refresh;

        std::multiset< std::pair<time_t, struct Ticket*> >::iterator  timer;
    };


  public:

    typedef std::map<std::string, Ticket*>   TicketMap;
    typedef TicketMap::const_iterator        const_iterator;

    typedef std::pair<time_t, Ticket*>       TicketPair;
    typedef std::multiset< TicketPair >      TimerSet;



  public:

    size_t              size()  const { return _tickets.size(); }
    const_iterator      begin() const { return _tickets.begin(); }
    const_iterator      end()   const { return _tickets.end(); }


  private:

    TicketMap                   _tickets;
    TimerSet                    _timers;

    tcanetpp::ThreadLock*       _lock;

    std::string                 _errStr;

};


} // namespace

#endif //  _TNMSSESSION_TICKETREPOSITORY_H_

