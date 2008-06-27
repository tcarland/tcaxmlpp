#define STRTEST_CPP

#include <iostream>
#include <vector>

#include "StringUtils.h"
#include "CidrUtils.h"
using namespace tcanetpp;






int main() {

    // string conversions
    int inta      = 256;
    uint32_t intb = 512;
    uint16_t intc = 1;
    uint8_t  mb   = 28;
    uint16_t mbb  = 29;


    std::string mbs  = "16";
    std::string ints = "1024";
    std::string cidrStr = "10.10.10.0/24";
    std::string addrstr;
    uint32_t    addr;

    std::cout << "mb = " << mb << std::endl;
    std::cout << "mbb = " << mbb << std::endl;

    uint8_t uintmb = StringUtils::fromString<uint8_t>(mbs);


    std::cout << "string of '" << mbs << "' converted to '" << uintmb << "'"
        << std::endl;

    intc = StringUtils::fromString<uint16_t>(ints);
    std::cout << "string of '" << ints << "' converted to uint16 of '" << intc << "'"
        << std::endl;

    mbs = StringUtils::toString(mb);
    std::cout << "uint8 of " << mb << " converted to '" << mbs << "'" 
        << std::endl;
    mbs = StringUtils::toString(mbb);
    std::cout << "uint16 of " << mbb << " converted to '" << mbs << "'" 
        << std::endl;
    
    
    std::vector<std::string>  stok;
    
    StringUtils::split(cidrStr, '/', std::back_inserter(stok));
    
    if ( stok.size() >= 1 )
        addrstr = stok[0];

    if ( CidrUtils::StringToAddr(addrstr, addr) <= 0 )
	return -1;

    if ( stok.size() == 2 )
	uintmb = StringUtils::fromString<uint8_t>(stok[1]);

    std::cout << "string of '" << stok[1] << "' converted to '" << uintmb << "'"
        << std::endl;

    if ( uintmb <= 0 || uintmb > 32 )
	uintmb = 32;

    std::cout << "string of '" << cidrStr << "' split to '" << addrstr 
        << "' and '" << uintmb << "'" << std::endl;

    return 0;
}


