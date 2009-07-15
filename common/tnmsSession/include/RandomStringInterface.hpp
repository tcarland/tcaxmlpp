#ifndef _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_
#define _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_

#include <string>


namespace tnmsSession {



class RandomStringInterface {

public:

    virtual ~RandomStringInterface() {}

    virtual void randomString ( std::string & randomString,
                                size_t        length, 
                                bool          printable = true ) = 0;
    
    virtual void randomString ( const std::string & prefix, 
                                std::string       & randomString,
                                size_t              length, 
                                bool                printable = true ) = 0;

};


} // namespace


#endif  // _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_

