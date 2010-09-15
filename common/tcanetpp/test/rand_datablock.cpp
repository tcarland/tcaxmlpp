
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>


#include "random.h"


#define  DATABLOCK_SIZE  1444


class DataBlock {

  public:

    DataBlock();

    std::string  getDataBlock();

  protected:

    static void InitDataBlock();

  private:

    static bool         _DataInit;
    static std::string  _DataBlock;

};


bool  
DataBlock::_DataInit  = false;
std::string 
DataBlock::_DataBlock = std::string("");

DataBlock::DataBlock()
{
    if ( ! _DataInit )
        DataBlock::InitDataBlock();
}

std::string
DataBlock::getDataBlock()
{
    return _DataBlock;
}

void
DataBlock::InitDataBlock()
{
    double   range = 255.0;
    uint32_t rval  = 0;
    int32_t  dsize = DATABLOCK_SIZE;
    uint8_t* byte;

    tcanet_seed();

    std::cout << "DataBlock::Init()" << std::endl;

    for ( int i = 0; i < dsize; i++ ) 
    {
        rval = tcanet_randomValue(range);
        _DataBlock.push_back((char) (*((uint8_t*)&rval)) );
    }

    return;
}


int main ( int argc, char **argv ) 
{
    std::string   rblock;
    DataBlock   * dblk;

    dblk    = new DataBlock();
    rblock  = dblk->getDataBlock();

    std::cout << " data block is " << rblock.size() << " bytes." << std::endl;

    std::ofstream ofs("rfile.tnet");
    if ( ! ofs ) { 
        std::cout << "Error writing to file" << std::endl;
        return -1;
    }
    ofs.write(rblock.data(), rblock.size());
    ofs.close();

    int          subsz = 1420;
    std::string  sblk  = rblock.substr(0, subsz);

    std::cout << " sub block is " << sblk.size() << " bytes." << std::endl;

    delete dblk;

    return 0;
}

