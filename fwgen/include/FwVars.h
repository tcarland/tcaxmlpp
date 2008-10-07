#ifndef _INCLUDE_FWVARS_H_
#define _INCLUDE_FWVARS_H_

#include <string>
#include <map>
#include <iterator>

#include "Prefix.hpp"
#include "Exception.hpp"
using namespace tcanetpp;


namespace fwgen {


class FwVars {

  public:

    typedef std::map<std::string, Prefix>  FwVarMap;
    typedef FwVarMap::iterator             iterator;
    typedef FwVarMap::const_iterator       const_iterator;

  public:

    FwVars();
    FwVars ( const std::string & varfile ) throw ( Exception );

    ~FwVars();

    bool            parse   ( const std::string & varfile );

    Prefix          find    ( const std::string & name );

    bool            insert  ( const std::string & name, Prefix & p );
    bool            insert  ( const std::string & name, const Prefix & p );

    Prefix          remove  ( const std::string & name );

    bool            exists  ( const std::string & name );

    void            clear();
    size_t          size() const;

    iterator        begin();
    iterator        end();
    const_iterator  begin() const;
    const_iterator  end() const;

    void            erase ( iterator & iter );
    void            erase ( const std::string & name );
    void            erase ( iterator & begin, iterator & end );

    std::string     getErrorStr() const;


  private:

    FwVarMap        _varMap;
    std::string     _errStr;

};

}  // namespace


#endif  // _INCLUDE_FWVARS_H_
