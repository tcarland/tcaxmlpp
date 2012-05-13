/**
  * @file FileStat.h
  *
  * Copyright (c) 2002,2008,2011 Timothy Charlton Arland 
  * @author  tca@charltontechnology.net
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
#ifndef _TCANETPP_FILESTAT_H_
#define _TCANETPP_FILESTAT_H_

extern "C" {
#ifdef WIN32
#  include <windows.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
}

#include <string>

#include "Exception.hpp"


namespace tcanetpp {


#ifdef WIN32
typedef struct _stat  filestat_t;
#else
typedef struct stat   filestat_t;
#endif


class FileStat {

  public:

    FileStat ( const std::string & filename ) 
        throw ( Exception );

    virtual ~FileStat();

    bool          operator< ( const FileStat & fs ) const;

    bool          isReadable() const;
    bool          isDirectory() const;
    bool          isBlockDevice() const;
    bool          isCharDevice() const;
    bool          isPipe() const;
    bool          isSocket() const;
    bool          isSymlink() const;

    time_t        lastTouched() const;
    time_t        lastAccessed() const;
    uid_t         uidOwner() const;
    gid_t         gidOwner() const;

    std::string   getFileName() const;
    std::string   getPathName() const;
    std::string   getAbsoluteName() const;

    size_t        size() const;
    size_t        bytes() const { return this->size(); }
    size_t        blocks() const;
    size_t        blockCount() const;
    size_t        blockSize() const;

  protected:

    filestat_t    _statb;
    std::string   _file;

};


}  // namespace


#endif  // _TCANETPP_FILESTAT_H_
