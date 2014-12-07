/** 
  * @file HashMap.hpp
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

#include <string>
#include <unordered_map>


namespace tcanetpp {


/**  A wrapper to the unordered_map<> class that provides a
  * default hasher implementation for using string keys.
  * The unordered_map template is a part of the newer C++-11
  * standard and requires the '-std=c++0x' compile flag depending
  * on the compiler and its version (ie. required in gcc <4.8).
 **/
template< typename ValueType >
class HashMap {

  public:

    struct keyeq {
        bool operator() ( const std::string & a,
                          const std::string & b ) const
        { 
            return ( a.compare(b) == 0 );
        }
    };

    typedef std::unordered_map<std::string, ValueType,
                     std::hash<std::string>, keyeq >   Hash;
    typedef typename Hash::iterator                    iterator;
    typedef typename Hash::const_iterator              const_iterator;

    
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

