/** 
  * @file HashMap.hpp
  *
  *   A wrapper to the SGI hash_map<> class that provides a
  * default hasher implementation for using string keys.
  * As of the time of this writing, the hash_map implementation is 
  * not yet a part of the C++ standard library. This implementation 
  * relies on the original SGI hash_map, which is still provided with 
  * the newer GNU STL releases, but as a result this code may 
  * not cross-compile or function properly on WIN32 platforms (or 
  * non-Gnu stdc++)
  *  
  * Copyright (c) 2002,2008,2009 Timothy Charlton Arland 
  * @author tcarland@gmail.com
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
#ifndef _TCANETPP_HASHMAP_HPP_
#define _TCANETPP_HASHMAP_HPP_


#if defined __GNUC__ && __GNUC__ >= 3
# include <ext/hash_map>
  using __gnu_cxx::hash_map;
  using __gnu_cxx::hash;
#else
#if defined __GNUC__ && __GNUC__ < 3
# include <hash_map>
  using std::hash_map;
  using std::hash;
# endif
#endif


namespace tcanetpp {


template< typename ValueType >
class HashMap {

  public:

    struct keyeq {
        bool operator() ( const char * a, const char * b )
        { 
            return ( strcmp(a, b) == 0 ); 
        }
    };

    typedef hash_map< const char*, ValueType, 
                      hash<const char*>, keyeq >  Hash;
    typedef typename Hash::iterator               iterator;
    typedef typename Hash::const_iterator         const_iterator;

    
  public:

    HashMap() {}

    virtual ~HashMap() { this->clear(); }


    ValueType& operator[]  ( const std::string & key ) 
    {
        return _hash[key.c_str()];
    }

    void  add ( const std::string & key, ValueType & val )
    {
        _hash[key.c_str()] = val;
    }

    iterator find ( const std::string & key )
    {
        return _hash.find(key.c_str());
    }

    void  erase ( iterator iter )           { _hash.erase(iter); }
    void  erase ( const std::string & key ) { _hash.erase(key.c_str()); }
        

    size_t          size() const  { return _hash.size();  }
    void            clear()       { return _hash.clear(); }

    iterator        begin()       { return _hash.begin(); }
    iterator        end()         { return _hash.end();   }

    const_iterator  begin() const { return _hash.begin(); }
    const_iterator  end()   const { return _hash.end();   }

    
  private:

    Hash       _hash;

};

} // namespace
    
#endif  // _TCANETPP_HASHMAP_HPP_

