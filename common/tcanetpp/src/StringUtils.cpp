/**
  *  A collection of static methods for c++ string manipulation.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
 **/
#define _TCANETPP_STRINGUTILS_CPP_

#include <cctype>

#include "StringUtils.h"
#include "tcanetpp_types.h"


namespace tcanetpp {


const size_t
StringUtils::MAXLINE = BIGSTRLINE;


// ----------------------------------------------------------------------

/**@{  Indicates whether a given string starts or ends with the provided 
  *    prefix or suffix, respectively.  */
bool
StringUtils::startsWith ( const std::string & str, const std::string & prefix )
{
    std::string::const_iterator  sIter, pIter;

    if ( str.size() < prefix.size() )
        return false;

    for ( sIter = str.begin(), pIter = prefix.begin(); 
          pIter != prefix.end(); 
          ++sIter, ++pIter )
    {
        if ( *sIter != *pIter )
            return false;
    }

    return true;
}

bool
StringUtils::endsWith ( const std::string & str, const std::string & suffix )
{
    std::string::const_reverse_iterator  sIter, pIter;

    if ( str.size() < suffix.size() )
        return false;

    for ( sIter = str.rbegin(), pIter = suffix.rbegin();
          pIter != suffix.rend();
          ++sIter, ++pIter )
    {
        if ( *sIter != *pIter )
            return false;
    }

    return true;
}
/*@}*/


// ----------------------------------------------------------------------
/**  Returns a string representing the character at the provided index. */

std::string
StringUtils::charAt ( const std::string & str, size_t index )
{
    return str.substr(index, index + 1);
}

// ----------------------------------------------------------------------
/**@{   Indicates string equality with or without considering case */

bool
StringUtils::equals ( const std::string & strA, const std::string & strB )
{
    return( strA.compare(strB) == 0 );
}

bool
StringUtils::equalsIgnoreCase ( const std::string & strA, const std::string & strB )
{
    std::string  a = strA;
    std::string  b = strB;

    StringUtils::toLowerCase(a);
    StringUtils::toLowerCase(b);

    return( a.compare(b) == 0 );
}
/*@}*/

// ----------------------------------------------------------------------
/**@{  Returns the first (or last) index of @param match in the given string. */

int
StringUtils::indexOf ( const std::string & str, const std::string & match )
{
    return StringUtils::indexOf(str, match, 0);
}

int
StringUtils::indexOf ( const std::string &str, const std::string & match, size_t from )
{
    std::string             tmp;
    std::string::size_type  indx;

    if ( from > str.length() || from < 0 )
	return -1;

    tmp = str.substr( (0 + from), str.length() );

    if ( (indx = tmp.find(match)) == std::string::npos )
	return -1;

    return( (int) indx );
}

int
StringUtils::lastIndexOf ( const std::string & str, const std::string & match )
{
    return StringUtils::lastIndexOf(str, match, str.length());
}

int
StringUtils::lastIndexOf ( const std::string & str, const std::string & match, size_t from )
{
    std::string             tmp;
    std::string::size_type  indx;

    if ( from > str.length() )
	from = str.length();
    else if ( from < 0 )
	return -1;

    tmp = str.substr(0, 0 + from);

    if ( (indx = tmp.find_last_of(match)) == std::string::npos )
	return -1;

    return ( (int) indx );
}
/*@}*/

// ----------------------------------------------------------------------
/**@{  Transforms the provided string.  */

std::string
StringUtils::toLowerCase ( const std::string & str )
{
    std::string  tmp = str;
    StringUtils::toLowerCase(tmp);
    return tmp;
}


void
StringUtils::toLowerCase ( std::string & str )
{
    std::string::size_type  i;
    for ( i = 0; i < str.length(); i++ )
	str[i] = tolower( (int) str[i] );
    return;
}


std::string
StringUtils::toUpperCase ( const std::string & str )
{
    std::string  tmp = str;
    StringUtils::toUpperCase(tmp);
    return tmp;
}


void
StringUtils::toUpperCase ( std::string & str )
{
    std::string::size_type  i;
    for ( i = 0; i < str.length(); i++ )
	str[i] = toupper( (int) str[i] );
    return;
}
/*@}*/


// ----------------------------------------------------------------------
/**@{  Trims the whitespace at the front and back of the string. */

std::string
StringUtils::trim ( const std::string & str )
{
    std::string  tmp = str;
    StringUtils::trim(tmp);
    return tmp;
}

void
StringUtils::trim ( std::string & str )
{
    while ( isspace(str[0]) )
	str.erase(0,1);
    while ( isspace(str[str.length() - 1]) )
	str.erase(str.length() - 1, str.length());
    return;
}
/*@}*/


/**  Strips all occurances of @param rem from the provided string. */
void
StringUtils::trim ( std::string & str, const std::string & rem )
{
    int indx = 0;

    while ( (indx = StringUtils::indexOf(str, rem)) >= 0 )
        str.erase(indx, rem.length());

    return;
}


// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
//   Widechar support for the same methods including conversion from 
//   wide to narrow/ascii and back.


#ifdef TCANETPP_WIDECHAR

extern "C" {
# include <wchar.h>
}


// ----------------------------------------------------------------------

bool
StringUtils::startsWith ( const std::wstring & str, const std::wstring & prefix )
{
    std::wstring::size_type  indx;

    indx = str.find(prefix);
    if ( indx == 0 )
        return true;

    return false;
}


bool
StringUtils::endsWith ( const std::wstring & str, const std::wstring & suffix )
{
    std::wstring::size_type  indx;

    indx = str.rfind(suffix);

    if ( indx == std::wstring::npos ) 
        return false;
    if ( (str.length() - indx) == suffix.length() )
        return true;

    return false;
}

// ----------------------------------------------------------------------

std::wstring
StringUtils::charAt ( const std::wstring & str, size_t index )
{
    return str.substr(index, index + 1);
}

// ----------------------------------------------------------------------

bool
StringUtils::equals ( const std::wstring & strA, const std::wstring & strB )
{
    return(strA.compare(strB) == 0);
}


bool
StringUtils::equalsIgnoreCase ( const std::wstring & strA, const std::wstring & strB )
{
    std::wstring a = strA;
    std::wstring b = strB;
    StringUtils::toLowerCase(a);
    StringUtils::toLowerCase(b);
    return( a.compare(b) == 0 );
}

// ----------------------------------------------------------------------

int
StringUtils::indexOf ( const std::wstring & str, const std::wstring & match )
{
    return StringUtils::indexOf(str, match, 0);
}


int
StringUtils::indexOf ( const std::wstring & str, const std::wstring & match, size_t from )
{
    std::wstring             tmp;
    std::wstring::size_type  indx;

    if ( from > str.length() || from < 0 )
	return -1;

    tmp = str.substr( (0 + from), str.length() );

    if ( (indx = tmp.find(match)) == std::wstring::npos )
	return -1;

    return( (int) indx );
}


int
StringUtils::lastIndexOf ( const std::wstring & str, const std::wstring & match )
{
    return StringUtils::lastIndexOf(str, match, str.length());
}


int
StringUtils::lastIndexOf ( const std::wstring & str, const std::wstring & match, size_t from )
{
    std::wstring             tmp;
    std::wstring::size_type  indx;

    if ( from > str.length() )
	from = str.length();
    else if ( from < 0 )
	return -1;

    tmp = str.substr(0, 0 + from);

    if ( (indx = tmp.find_last_of(match)) == std::wstring::npos )
	return -1;

    return ( (int) indx );
}

// ----------------------------------------------------------------------

std::wstring
StringUtils::toLowerCase ( const std::wstring & str )
{
    std::wstring  tmp = str;
    StringUtils::toLowerCase(tmp);
    return tmp;
}


void
StringUtils::toLowerCase ( std::wstring & str )
{
    for ( uint32_t i = 0; i < str.length(); i++ )
	str[i] = tolower( (int) str[i] );
    return;
}


std::wstring
StringUtils::toUpperCase ( const std::wstring & str )
{
    std::wstring  tmp = str;
    StringUtils::toUpperCase(tmp);
    return tmp;
}


void
StringUtils::toUpperCase ( std::wstring & str )
{
    for ( uint32_t i = 0; i < str.length(); i++ )
	str[i] = toupper( (int) str[i] );
    return;
}

// ----------------------------------------------------------------------

std::wstring
StringUtils::trim ( const std::wstring & str )
{
    std::wstring  tmp = str;
    StringUtils::trim(tmp);
    return tmp;
}


void
StringUtils::trim ( std::wstring & str )
{
    while ( isspace(str[0]) )
	str.erase(0,1);
    
    while ( isspace(str[str.length() - 1]) )
	str.erase(str.length() - 1, str.length());

    return;
}


void
StringUtils::trim ( std::wstring & str, const std::wstring & rem )
{
    int indx = 0;

    while ( (indx = StringUtils::indexOf(str, rem)) >= 0 )
        str.erase(indx, str.length());

    return;
}

// ----------------------------------------------------------------------
/**@{  Provides conversion between ascii and wide strings  */

std::wstring
StringUtils::ctowstr ( const std::string & str )
{
    std::wstring  wstr = L"";
    wchar_t      *tmp  = NULL;

    size_t slen = ::strlen(str.c_str());
    size_t wlen = ::mbstowcs(NULL, str.c_str(), slen) + 1;

    tmp  = (wchar_t*) ::malloc(wlen * sizeof(wchar_t*));
    wlen = ::mbstowcs(tmp, str.c_str(), slen + 1);

    if ( tmp == NULL )
        return wstr;
    
    wstr = std::wstring(tmp);
    ::free(tmp);

    return wstr;
}


std::string
StringUtils::wtocstr ( const std::wstring & wstr )
{
    std::string  str = "";
    char        *tmp = NULL;

    size_t wlen = ::wcslen(wstr.c_str());
    size_t slen = ::wcstombs(NULL, wstr.c_str(), wlen) + 1;

    tmp  = (char*) ::malloc(slen * sizeof(char*));
    slen = ::wcstombs(tmp, wstr.c_str(), wlen+1);

    if ( tmp == NULL )
        return str;

    str = std::string(tmp);
    free(tmp);

    return str;
}
/*@}*/

// ----------------------------------------------------------------------

#endif  // TCANETPP_WIDECHAR


} // namespace


// _TCANETPP_STRINGUTILS_CPP_
