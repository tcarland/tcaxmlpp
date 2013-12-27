#include <iostream>

#include "StringUtils.h"
#include "HashMap.hpp"
using namespace tcanetpp;

#include "foo.h"

// not really a test but a simple example. should make this a proper test.
int main()
{
    HashMap<foo*>   hasht;

    foo_t  * f1   = new foo();
    foo_t  * f2   = new foo();

    f1->client = 0;
    f1->timestamp = 0;
    f1->count = 1;
    f2->client = 1;
    f2->timestamp = 1;
    f2->count = 2;


    hasht.add("foo1", f1);

    hasht.add("foo2", f2);

    std::cout << "Hash size is " << hasht.size() << std::endl;

    HashMap<foo*>::iterator  hIter, fIter;
    hIter = hasht.find("foo2");
    fIter = hasht.find("foo1");

    if ( fIter != hasht.end() ) 
    {
        std::cout << "Found foo1: id=" << fIter->second->client << std::endl;
    }

    if ( hIter != hasht.end() ) 
    {
        std::cout << "Found foo2: id=" << hIter->second->client << std::endl;
        hasht.erase(hIter);
    }

    std::cout << "Hash size is " << hasht.size() << std::endl;

    return 0;
}

