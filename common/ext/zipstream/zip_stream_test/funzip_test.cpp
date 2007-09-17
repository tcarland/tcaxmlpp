

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "zipstream.hpp"


using namespace zlib_stream;




int main ( int argc, char **argv )
{
    if ( argc != 2 ) {
        std::cout << "Usage: funzip file size" << std::endl;
        return 0;
    }

    char * file = strdup(argv[1]);
    int    size = 1024;
    char   buff[1024];

    std::string outfile = "outfile.txt";

    if ( argc == 3 )
        size = atoi(argv[2]);
        
    std::ifstream ifs(file, std::ios::in | std::ios::binary);

    int  i = 0;
    do {
        ifs.read(&buff[i], 1);
        if ( ifs )
            i++;
    } while ( ifs );

    std::cout << "fstream read " << i << " bytes." << std::endl;

    std::ostringstream  ostream;
    ostream.write(&buff[0], size);

    std::istringstream  istream(ostream.str(), std::ios::in);
    zip_istream unzipper(istream);

    char * rxbuff   = (char*) malloc(size*2);
    size_t rxbuffsz = size * 2;
    char * ptr      = rxbuff;
    size_t rd       = 0;

    std::streamsize   in;

    while ( unzipper.peek() != EOF ) {
        in = unzipper.rdbuf()->in_avail();
        if ( (in+rd) > rxbuffsz ) {
            rxbuffsz += in;
            rxbuff    = (char*) ::realloc(rxbuff, rxbuffsz);
            ptr       = rxbuff + rd;
        }
        std::cout << "in_avail: " << in << std::endl;
        unzipper.read(ptr, in);
        ptr += in;
        rd  += in;
    }

    unzipper.read_footer();
    size_t zin    = unzipper.rdbuf()->get_in_size();
    size_t unzout = unzipper.rdbuf()->get_out_size();
    std::cout << "uncompress() in size: " << zin 
            << " out size: " << unzout << std::endl;


    std::cout << " in = " << in << " rd = " << rd << std::endl;

    std::ofstream ofs(outfile.c_str(), std::ios::out | std::ios::binary);

    ofs.write(rxbuff, rd);
    ofs.close();


    return 0;
}



