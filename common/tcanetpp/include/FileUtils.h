/**
  * @file FileUtils.h
  *
  * Copyright (c) 2002,2008 Timothy Charlton Arland 
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

