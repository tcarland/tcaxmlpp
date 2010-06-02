
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <byte_order.h>
#include <tcanetpp_types.h>


void usage()
{
    std::cout << "Usage: ntohl <number>\n\n"
        << "ntohs or \"Network to Host Long\" converts a provided argument number\n"
        << "from Network(Big Endian) to Host order (assumed Little-endian)\n"
        << "The number is assumed to be an unsigned long (4 bytes).\n"
        << "This tool is intended for convenience when using tools like network sniffers\n"
        << "that display bytes in typical network order\n";
    exit(0);
}

int main ( int argc, char **argv )
{
    if ( argc < 2 ) 
        usage();

    uint32_t inp = atoi(argv[1]);
    uint32_t oup = ntohl(inp);

    printf("%u > %u\n", inp, oup);

    return 0;
}


