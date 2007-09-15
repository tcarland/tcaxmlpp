/**
  *  HashMap - A wrapper to the SGI hash_map<> class that 
  *  provides a default hasher implementation for using string keys.
  *
  *  @Author   tcarland@gmail.com
  *  @Version  0.2
  *
  *  $Id: HashMap.hpp,v 0.1 2007/09/16 14:13:00 Exp $
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
        { return ( strcmp(a, b) == 0 ); }
    };

    typedef hash_map<const char*, ValueType,
        hash<const char*>, keyeq>              Hash;
    typedef typename Hash::iterator            iterator;
    typedef typename Hash::const_iterator      const_iterator;
    
  public:

    HashMap() {}

    virtual ~HashMap() { this->clear(); }


    void     add   ( const std::string & key, ValueType & val )
        { _hash[key.c_str()] = val; }

    iterator find  ( const std::string & key )
        { return _hash.find(key.c_str()); }

    void     erase ( iterator iter )           { _hash.erase(iter); }
    void     erase ( const std::string & key ) { _hash.erase(key.c_str()); }
        
    size_t   size() const   { return _hash.size(); }
    void     clear()        { return _hash.clear(); }

    iterator begin()        { return _hash.begin(); }
    iterator end()          { return _hash.end(); }

    const_iterator  begin() const { return _hash.begin(); }
    const_iterator  end()   const { return _hash.end(); }

    ValueType& operator[] ( const std::string & key ) 
        { return _hash[key.c_str()]; }

    
  private:

    Hash       _hash;

};

} // namespace
    
#endif  // _TCANETPP_HASHMAP_HPP_
