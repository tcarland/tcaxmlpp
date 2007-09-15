/**
  *  Static utility methods for c++ string manipulation
  *  of both ascii and wide char strings.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.1
 **/
#ifndef _TCANETPP_STRINGUTILS_H_
#define _TCANETPP_STRINGUTILS_H_

#include <string>
#include <sstream>


#include "tcanetpp_config.h"


namespace tcanetpp {


#define SMLSTRLINE 256
#define MEDSTRLINE 512
#define BIGSTRLINE 1024
#define MAXSTRLINE 2048


/**   A utility class of static methods that provide some convenient
  *   string manipulation functions. 
 **/
class StringUtils {

  public:

    static bool          equals           ( const std::string & strA, 
                                            const std::string & strB );

    static bool          equalsIgnoreCase ( const std::string & strA, 
                                            const std::string & strB );

    static bool          startsWith       ( const std::string & str,  
                                            const std::string & prefix );

    static bool          endsWith         ( const std::string & str,  
                                            const std::string & suffix );

    static int           indexOf          ( const std::string & str,  
                                            const std::string & match );

    static int           indexOf          ( const std::string & str,  
                                            const std::string & match, 
                                            size_t from );

    static int           lastIndexOf      ( const std::string & str,  
                                            const std::string & match );

    static int           lastIndexOf      ( const std::string & str,  
                                            const std::string & match, 
                                            size_t from );

    static std::string   charAt           ( const std::string & str, 
                                            size_t index );

    static std::string   toLowerCase      ( const std::string & str );
    static void          toLowerCase      ( std::string & str );

    static std::string   toUpperCase      ( const std::string & str );
    static void          toUpperCase      ( std::string & str );

    static std::string   trim             ( const std::string & str );
    static void          trim             ( std::string & str );
    static void          trim             ( std::string & str, 
                                            const std::string & remove );

    template<typename T>
    static inline std::string toString    ( const T & a )
    {
        std::stringstream  stream;
        stream << a;
        return stream.str();
    }

    template<typename T>
    static inline  T     fromString       ( const std::string & str )
    {
        T target = T();
        std::stringstream stream(str);
        stream >> target;
        return target;
    }

    template< typename OutputIterator_ >
    static inline void   split            ( const std::string  & str, 
                                            const char           delimiter,
                                            OutputIterator_      outI )
    {
        std::string::size_type  begin = 0, end = 0;

        while ( (begin = str.find_first_not_of(delimiter, begin)) != std::string::npos )
        {
            end     = str.find_first_of(delimiter, begin);
            *outI++ = str.substr(begin, end - begin);
            begin   = end;
        }
    }


    static const size_t  MAXLINE;


// ----------------------------------------------------------------------
//  The same interface is provided for wide chars and is enabled by the
//  following compile-time definition.

#ifdef TCANET_WIDECHAR

    static bool          equals           ( const std::wstring & strA, 
                                            const std::wstring & strB );

    static bool          equalsIgnoreCase ( const std::wstring & strA, 
                                            const std::wstring & strB );

    static bool          startsWith       ( const std::wstring & str,  
                                            const std::wstring & prefix );

    static bool          endsWith         ( const std::wstring & str,  
                                            const std::wstring & suffix );

    static int           indexOf          ( const std::wstring & str,  
                                            const std::wstring & match );

    static int           indexOf          ( const std::wstring & str,  
                                            const std::wstring & match, 
                                            size_t from );

    static int           lastIndexOf      ( const std::wstring & str,  
                                            const std::wstring & match );

    static int           lastIndexOf      ( const std::wstring & str,  
                                            const std::wstring & match, 
                                            size_t from );

    static std::wstring  charAt           ( const std::wstring & str, size_t index );
    static std::wstring  toLowerCase      ( const std::wstring & str );
    static void          toLowerCase      ( std::wstring & str );
    static std::wstring  toUpperCase      ( const std::wstring & str );
    static void          toUpperCase      ( std::wstring & str );
    static std::wstring  trim             ( const std::wstring & str );
    static void          trim             ( std::wstring & str );
    static void          trim             ( std::wstring & str, const std::wstring & rem );
    
    static std::wstring  ctowstr          ( const std::string  & str );
    static std::string   wtocstr          ( const std::wstring & str );

    template< typename OutputIterator_ >
    static inline void   split            ( const std::wstring  & str, 
                                            char                  delimiter,
                                            OutputIterator_       outI )
    {
        std::wstring::size_type  begin = 0, end = 0;

        while ( (begin = str.find_first_not_of(delimiter, begin)) != std::string::npos )
        {
            end = str.find_first_of(delimiter, begin);
            *outI++ = str.substr(begin, end - begin);
            begin   = end;
        }
    }

#endif  // TCANET_WIDECHAR


}; 

 
}  // namespace

#endif  // _TCANETPP_STRINGUTILS_H_

