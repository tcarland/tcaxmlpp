#ifndef _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_
#define _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_

#include <string>


namespace tnmsSession {



class RandomStringInterface {

public:

    virtual ~RandomStringInterface() {}

    virtual void randomString ( size_t        length, 
                                std::string & randomString,
                                bool          printable = true ) = 0;
    
    virtual void randomString ( size_t              length, 
                                const std::string & prefix, 
                                std::string       & randomString,
                                bool                 printable = true ) = 0;

};


} // namespace


#endif  // _TNMSSESSION_RANDOMSTRINGINTERFACE_HPP_

