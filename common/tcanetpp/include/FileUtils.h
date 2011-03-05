/**
  * @file FileUtils.h
  *
  * Copyright (c) 2002,2008,2011 Timothy Charlton Arland 
  * @author tca@charltontechnology.net
  *
  * @section LICENSE
  *
  * This file is part of tcanetpp.
  *
  * tcanetpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcanetpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcanetpp.  
  * If not, see <http://www.gnu.org/licenses/>.
 **/
#ifndef _TCANETPP_FILEUTILS_H_
#define _TCANETPP_FILEUTILS_H_

extern "C" {
#ifdef WIN32
#  include <windows.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
}

#include <string>
#include <list>

#include "Exception.hpp"


namespace tcanetpp {

#ifdef WIN32
typedef struct _stat  FileStat;
#else
typedef struct stat   FileStat;
#endif

typedef std::list< std::string >   FileNameList;


class FileUtils {

  public:

    FileUtils ( const std::string & file ) throw ( Exception );
    ~FileUtils();

    bool           isReadable() const;
    bool           isDirectory() const;
    bool           isBlockDevice() const;
    bool           isCharDevice() const;
    bool           isPipe() const;
    bool           isSocket() const;
    bool           isSymlink() const;

    time_t         lastTouched() const;
    time_t         lastAccessed() const;
    uid_t          uidOwner() const;
    gid_t          gidOwner() const;

    size_t         size() const;
    size_t         bytes() const { return this->size(); }

  public:

    static time_t  LastTouched    ( const std::string & filename );
    static bool    IsReadable     ( const std::string & filename );
    static bool    IsBlockDevice  ( const std::string & filename );
    static bool    IsPipe         ( const std::string & filename );
    static bool    IsDirectory    ( const std::string & dirname  );
    static bool    IsSymlink      ( const std::string & filename );

    static bool    GetFilenames   ( const std::string & path,
                                    FileNameList      & files,
                                    bool  recursive   = true );
  protected:

    static bool    InitFileStat   ( FileStat          * fsb, 
                                    const std::string & file );

  private:

    FileStat      _statb;
    std::string   _file;

};

}  // namespace

#endif // _TCANETPP_FILEUTILS_H_

