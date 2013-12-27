
#include <string>
#include <iostream>
#include <fstream>

#include "JSON.h"
using namespace jsonpp;


int main ( int argc, char **argv )
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
       \"foo\" : false, \
       \"path_str\" : \"eth.ip.tcp.http.youtube\", \
       \"foo2\" : 33, \
       \"aspath\" : [ 1335, 1740, 701, 1, 19423 ] }";

    if ( ! j.parse(s) ) {
        std::cout << "Json parsing failed at position: " << j.getErrorPos() 
            << " >> '" << j.getErrorStr() << "'" << std::endl;
        return -1;
    }

    JsonObject & root = j.json();

    JsonObject::iterator  jIter;
    for ( jIter = root.begin(); jIter != root.end(); ++jIter )
    {
        const std::string & key  = jIter->first;
        JsonItem          * item = jIter->second;

        std::cout << key << " : " << JSON::TypeToString(item->getType())
            << std::endl;
    }

    std::cout << std::endl;
    std::cout << root << std::endl;

    if ( argc == 1 )
        return 0;

    /* Reading from a file */
    const char  * fn = argv[1];
    std::ifstream ifs(fn);

    if ( ! ifs )
        std::cout << "Error opening file " << fn << std::endl;

    std::istream & buf = (std::istream&) ifs;

    j.clear();
    if ( ! j.parse(buf) ) {
        std::cout << "Json parsing failed at position: " << j.getErrorPos() 
            << " >> '" << j.getErrorStr() << "'" << std::endl;
        return -1;
    }
    root = j.json();

    std::cout << std::endl;
    std::cout << root << std::endl;

    return 0;
}
