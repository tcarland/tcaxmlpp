#ifndef _TCANETPP_FILEUTILS_H_
#define _TCANETPP_FILEUTILS_H_


#include <cstdlib>
#include <string>
#include <list>


namespace tcanetpp {


typedef std::list< std::string >   FileNameList;


class FileUtils {

  public:

    static time_t        LastTouched    ( const std::string & filename );
    static bool          IsReadable     ( const std::string & filename );
    static bool          IsDirectory    ( const std::string & dirname );
    static bool          IsSymlink      ( const std::string & dirname );

    static bool          GetFilenames   ( const std::string & path,
                                          FileNameList      & files,
                                          bool    recursive = true );

};

}  // namespace

#endif // _TCANETPP_FILEUTILS_H_
