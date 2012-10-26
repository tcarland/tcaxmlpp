
#include <string>
#include <iostream>

#include "JSON.h"
using namespace jsonpp;


int main()
{
    JSON j;

    std::string  s = 
    "{ \"_id\" : \"4d7dab887f06676117a7fbd2\", \
       \"end_time\" : 12995200000 , \
       \"event_id\" : 28393, \
       \"src_ip\" : \"74.125.224.65\", \
       \"src_port\" : 80, \
       \"dest_ip\" : \"208.240.243.170\", \
       \"dest_port\" : 57371, \
       \"path\" : [ \"eth\", \"ip\", \"tcp\", \"http\", \"youtube\" ], \
       \"packets_in\" : 1, \
       \"bytes_in\" : 60, \
       \"path_str\" : \"eth.ip.tcp.http.youtube\" }";

    if ( ! j.parse(s) ) {
        std::cout << "Json parsing failed at position: " << j.getErrorPos() << std::endl;
        return -1;
    }

    JsonObject & root = j.json();

    JsonObject::iterator  jIter;

    for ( jIter = root.begin(); jIter != root.end(); ++jIter )
    {
        const std::string & key  = jIter->first;
        JsonItem          * item = jIter->second;

        std::cout << key << " : " << JSON::typeToString(item->getType()) << std::endl;
    }

    std::cout << std::endl;

    std::cout << root.toString() << std::endl;

    return 0;
}
