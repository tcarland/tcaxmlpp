/**  CmdBuf.h
  *
  *  @Author   tca@soe.sony.com
  *  @Version  0.4
 **/
#ifndef _TCANET_CMDBUF_H_
#define _TCANET_CMDBUF_H_

#include <iostream>
#include <string>
#include <vector>

#if defined __GNUC__ && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# include <ext/stdio_filebuf.h>
using __gnu_cxx::stdio_filebuf;
#endif


namespace tca {
namespace net {


#define DEFAULT_CMDBUF_SIZE  64
#define MINIMUM_CMDBUF_SIZE  2
#define MAXIMUM_CMDBUF_SIZE  1024



/**  The CmdBuf class buffers a system command's stdout
  *  for parsing. This is a simple implementation using the 
  *  SGI STL extentsion "stdio_filebuf". The current implementation
  *  requires libstdc++ 5 or Gnu version 3.1 or higher.
 **/
class CmdBuf {

  public:

    CmdBuf();
    CmdBuf ( u_int bufsize );

    virtual ~CmdBuf();

    bool   Open ( const char * cmdstr );
    void   Close();

    bool   isOpen();
    bool   haveData();
    bool   hasData() { return this->haveData(); }

    void   getAllLines ( std::vector<std::string> & lines );
    
    std::string getLine();

    bool   putLine ( const std::string & line );

  private:

    stdio_filebuf<char>*       _cmdbuf;
    std::streamsize            _bufsize;
    FILE*                      _file;
    bool                       _init;

    static const char*         _endline;
};


} // namespace
} // namespace


#endif // _TCANET_CMDBUF_H_
