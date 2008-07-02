/**  CmdBuf.h
  *
  *  @Author   tarland@gmail.com
  *  @Version  0.5
 **/
#ifndef _TCANETPP_CMDBUF_H_
#define _TCANETPP_CMDBUF_H_

#include <iostream>
#include <string>
#include <vector>

#if defined __GNUC__ && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# include <ext/stdio_filebuf.h>
using __gnu_cxx::stdio_filebuf;
#endif


namespace tcanetpp {


#define DEFAULT_CMDBUF_SIZE  64
#define MINIMUM_CMDBUF_SIZE  2
#define MAXIMUM_CMDBUF_SIZE  1024



/**  Provides a simpler wrapper to the gnu_cxx:stdio_filebuf interface
  *  offering the buffering system file io such as stdout, for parsing. 
 **/
class CmdBuf {

  public:

    CmdBuf();
    CmdBuf ( u_int bufsize );

    virtual ~CmdBuf();

    bool        Open        ( const char * cmdstr );
    void        Close();

    bool        isOpen();
    bool        haveData();
    bool        hasData()   { return this->haveData(); }

    bool        putLine     ( const std::string & line );
    std::string getLine();
    void        getAllLines ( std::vector<std::string> & lines );


  private:

    stdio_filebuf<char>*       _cmdbuf;
    std::streamsize            _bufsize;
    FILE*                      _file;
    bool                       _init;

    static const char*         _endline;
};


} // namespace


#endif // _TCANETPP_CMDBUF_H_
