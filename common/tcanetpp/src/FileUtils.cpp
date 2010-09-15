/**
  * @file FileUtils.cpp
  *
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
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
#define _TCANETPP_FILEUTILS_CPP_


extern "C" {
#ifdef WIN32
#  include <windows.h>
#else
#  include <dirent.h>
#  include <unistd.h>
#endif
# include <time.h>
# include <sys/types.h>
# include <sys/stat.h>
}
#include <fstream>

#include "FileUtils.h"



namespace tcanetpp {


bool
FileUtils::IsReadable ( const std::string & filename )
{
    std::ifstream ifn(filename.c_str());
    if ( ! ifn )
	    return false;
    ifn.close();
    return true;
}


time_t
FileUtils::LastTouched ( const std::string & filename )
{
# ifdef WIN32
    struct _stat  sb;
    
    if ( ::_stat(filename.c_str(), &sb) != 0 )
        return 0;
# else
    struct stat  sb;
    ::stat(filename.c_str(), &sb);
# endif

    return sb.st_mtime;
}

bool
FileUtils::IsDirectory ( const std::string & dname )
{
# ifdef WIN32
    struct _stat sb;

    if ( ::_stat(dname.c_str(), &sb) == 0 )
        if ( S_IFDIR & sb.st_mode )
            return true;
# else
    struct stat  sb;

    if ( ::stat(dname.c_str(), &sb) < 0 )
        return false;

    if ( S_ISDIR(sb.st_mode) )
        return true;
# endif

    return false;
}


bool
FileUtils::IsSymlink ( const std::string & dname )
{
# ifndef WIN32
    struct stat  sb;

    if ( ::stat(dname.c_str(), &sb) < 0 )
        return false;

    if ( S_ISLNK(sb.st_mode) )
        return true;
# endif

    return false;
}



bool 
FileUtils::GetFilenames ( const std::string & path, FileNameList & files, bool recursive )
{
#ifdef WIN32
    WIN32_FIND_DATA  findData;
    HANDLE           fileHandle;
    int              flag = 1;
    std::string      search ("*.*");

    search = path + "/" + search;

    fileHandle = FindFirstFile(search.c_str(), &findData);

    if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

    while ( flag ) {
        std::string dname = findData.cFileName;

        if ( dname.compare(".") == 0 || dname.compare("..") == 0 ) {
            flag = FindNextFile(fileHandle, &findData);
            continue;
        }

        dname = path + "/" + dname;

        if ( FileUtils::IsDirectory(dname.c_str()) ) {
            if ( recursive ) 
                FileUtils::GetFilenames(dname, files, true);
        } else {
            files.push_back(dname);
        }

        flag = FindNextFile(fileHandle, &findData);
    }

    FindClose(fileHandle);

    return true;

#else  /* UNIX */

    DIR*            dirp;
    struct dirent*  dirEntry;

    if ( (dirp = ::opendir(path.c_str())) == NULL )
        return false;

    while ( (dirEntry = ::readdir(dirp)) != NULL ) {
        std::string dname = dirEntry->d_name;

        if ( dname.compare(".") == 0 || dname.compare("..") == 0 )
            continue;

        dname = path + "/" + dname;

        if ( FileUtils::IsDirectory(dname) ) {
            if ( recursive ) 
                FileUtils::GetFilenames(dname, files, true);
        } else {
            files.push_back(dname);
        }
    }

    ::closedir(dirp);

#endif 

    return true;
}


}  // namespace

