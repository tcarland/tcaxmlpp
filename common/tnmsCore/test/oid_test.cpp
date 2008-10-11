#define toidtest_cpp

#include <iostream>

#include "TnmsOid.h"
using namespace tnmsCore;


int main()
{
    std::string  name1 = "test/foo/bar/joy";
    std::string  name2 = "test/foo";
    std::string  name3 = "/test/foo/";
    std::string  name4 = "/test/foo/bar/joy/beer1/";

    std::string  oid1  = "1.2.3.4.5.7";
    std::string  oid2  = "1.3.5.7.11";
    std::string  oid3  = "555.123.345";

    TnmsOid  toid1 = TnmsOid(oid1);
    TnmsOid  toid2 = TnmsOid(oid2);
    TnmsOid  toid3("10.10.10");

    std::cout << "oid IN is  " << oid1 << std::endl;
    std::cout << "oid OUT is " << toid1.toString() << std::endl;
    std::cout << std::endl;
    std::cout << "oid IN is  " << oid2 << std::endl;
    std::cout << "oid OUT is " << toid2.toString() << std::endl;
    std::cout << std::endl;
    std::cout << "oid OUT is " << toid3.toString() << std::endl;

    std::cout << std::endl << "tnmsoid1 '" << toid1.toString() << "' has a size of "
        << toid1.size() << " and last value of " << toid1.lastValue() << std::endl;

    std::cout << std::endl << "iteration: " << std::endl;
    TnmsOid::iterator tIter;
    for ( tIter = toid1.begin(); tIter != toid1.end(); ++tIter )
        std::cout << *tIter << ".";
    std::cout << std::endl;

    std::cout << "to oid array: " << std::endl;
    uint16_t* oida = toid1.toArray();
    uint16_t* t    = oida;
    for ( u_int i = 0; i != toid1.size(); ++i, ++t ) {
        std::cout << "array[" << i << "] = " << *t << std::endl;
    }



    return 0;
}
