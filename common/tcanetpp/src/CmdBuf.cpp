/**  CmdBuf 
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.5
 **/
#define _TCANETPP_CMDBUF_CPP_

#include "CmdBuf.h"


namespace tcanetpp {


const char*
CmdBuf::_endline = "\n";


CmdBuf::CmdBuf()
{
    _bufsize = DEFAULT_CMDBUF_SIZE;
    _init    = false;
}


CmdBuf::CmdBuf ( u_int bufsize )
{
    if ( bufsize >= MINIMUM_CMDBUF_SIZE || bufsize <= MAXIMUM_CMDBUF_SIZE )
	_bufsize = bufsize;
    else
	_bufsize = DEFAULT_CMDBUF_SIZE;
    _init = false;
}


CmdBuf::~CmdBuf()
{
    if ( _init )
	this->Close();
}


bool
CmdBuf::Open ( const char* cmdstr )
{
    FILE* f;

    if ( _init )
	this->Close();

    if ( (f = popen(cmdstr, "r")) == NULL )
	return false;

    _cmdbuf = new stdio_filebuf<char> (f, std::ios_base::in, _bufsize);

    if ( ! _cmdbuf->is_open() ) {
	delete _cmdbuf;
	delete f;
	_init = false;
    } else {
	_init = true;
	_file = f;
    }

    return _init;
}


void
CmdBuf::Close()
{
    if ( _init ) {
	_cmdbuf->close();
	delete _cmdbuf;
	pclose(_file);
    }
    _init = false;
    return;
}


bool
CmdBuf::isOpen()
{
    if ( _init )
	return _cmdbuf->is_open();
    return false;
}


bool
CmdBuf::haveData()
{
    if ( ! _init )
	return false;

    if ( _cmdbuf->sgetc() == EOF )
	return false;

    return true;
}


std::string
CmdBuf::getLine()
{
    char        ch;
    std::string line = "";

    while ( _cmdbuf->sgetc() != EOF && (ch = _cmdbuf->sbumpc()) != *_endline )
	line += ch;

    return line;
}


void
CmdBuf::getAllLines ( std::vector<std::string> & lines )
{
    std::string line = "";
    
    lines = std::vector<std::string> ();

    if ( ! _init )
	return;

    while ( this->haveData() ) {
	line = this->getLine();
	lines.push_back(line);
    }

    return;
}


bool
CmdBuf::putLine ( const std::string & line )
{
    std::streamsize  n;

    n = _cmdbuf->sputn(line.c_str(), line.length());

    if ( n != (std::streamsize) line.length() )
	return true;

    return true;
}


} // namespace

//  _TCANETPP_CMDBUF_CPP_ 

