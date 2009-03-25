
#include <iostream>

#include "tnmsCore.h"

using namespace tnmsCore;


#define TESTV  0x12340011


int main()
{
    uint16_t major = TNMS_VERSION_MAJOR;
    uint16_t minor = TNMS_VERSION_MINOR;

    uint32_t version = TNMS_VERSION;



    std::cout << "Version major: '" << major << "' minor: '" 
    << minor << "' = " << version << std::endl;


    major = TNMS_GET_MAJOR(version);
    minor = TNMS_GET_MINOR(version);
    
    std::cout << "Version major: '" << major << "' minor: '" 
    << minor << "' = " << version << std::endl;
    
    version = TESTV;
    std::cout << "test ver = " << version << std::endl;
    return 0;
}

