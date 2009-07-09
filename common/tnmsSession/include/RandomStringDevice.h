#ifndef _TNMSSESSION_RANDOMSTRINGDEVICE_H_
#define _TNMSSESSION_RANDOMSTRINGDEVICE_H_

#include <string>

#include "tnmsSession.h"


namespace tnmsSession {


class RandomStringDevice : public RandomStringInterface {

public:

    RandomStringDevice ( const std::string & filename = "/dev/urandom" );

    virtual ~RandomStringDevice();


    void randomString  ( std::string & randomString,
                         size_t        length    = TNMS_TICKET_LENGTH,
                         bool          printable = true );


    void randomString  ( const std::string & prefix,
                         std::string       & randomString,
                         size_t              length    = TNMS_TICKET_LENGTH,
                         bool                printable = true );

private:

    void   makePrintable  ( char * buff, size_t buff_len );

private:

    FILE *      _source;

};

} // namespace

#endif // _TNMSSESSION_RANDOMSTRINGDEVICE_H_

