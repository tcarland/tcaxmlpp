#define _MSG_TEST_CPP_

#include <string>
#include <iostream>

#include "tnmsCore.h"
using namespace tnmsCore;

#include "CircularBuffer.h"
using namespace tcanetpp;

int main()
{
    CircularBuffer * cbuf;
    TnmsMessage      msg;
    size_t           sz, wt, rd;
    char           * ptr;
    
    sz   = TNMS_PACKET_SIZE * 2000;
    cbuf = new CircularBuffer(sz);
    msg  = TnmsMessage("my.new.oid", REQUEST_MESSAGE);

    sz   = msg.size();
    ptr  = cbuf->getWritePtr(&sz);
    wt   = msg.serialize(ptr, sz);
    cbuf->setWritePtr(wt);

    std::cout << "serialize wrote " << wt << std::endl;



    TnmsMessage  msg2;
    ptr  = cbuf->getReadPtr(&sz);
    rd   = msg2.deserialize(ptr, sz);
    if ( rd != sz )
        std::cout << "deserialize() did not match read avail" << std::endl;

    cbuf->setReadPtr(rd);

    std::cout << "Resulting msg got '" << msg2.getElementName() 
        << "'" << std::endl;

    delete cbuf;

    return 0;
}



