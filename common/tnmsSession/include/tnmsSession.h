#ifndef _TNMSSESSION_H
#define _TNMSSESSION_H

#include "AuthenticationInterface.hpp"
#include "RandomStringInterface.hpp"
#include "TicketDatabase.h"


namespace tnmsSession {


#define TNMS_TICKET_LENGTH       128
#define TICKET_MAX_RETRY         5
#define TICKET_REFRESH_INTERVAL  3600 * 4
#define TICKET_GRACE_PERIOD      3600


struct TnmsSession;


struct TnmsSession {
    AuthenticationInterface   * authenticator;
    RandomStringInterface     * ticket_generator;
    TicketDatabase            * ticket_database;
};


}  // namespace


#endif // _TNMSSESSION_H
