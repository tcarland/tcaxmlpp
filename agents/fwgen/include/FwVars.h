#ifndef _FWGEN_FWVARS_H_
#define _FWGEN_FWVARS_H_

#include <string>
#include <map>
#include <iterator>

#include "Prefix.hpp"
#include "Exception.hpp"
using namespace tcanetpp;


namespace fwgen {


/**
  *    The FwVars class acts as our container of firewall variable mappings.
  *  A variable mapping is simply a key value expression where the key
  *  defines the variable name to be later expanded within a firewall rule.
  *  eg.  MyNetwork = "10.1.2.0/24", ANY = "0.0.0.0"
  *  Variables can be added directly or more commonly parsed from a
  *  file via the parse() method. Note that each call to parse will
  *  add more variables to the map. clear() should be used between calls
  *  to parse() if this behaviour is not desired. Duplicate variables
  *  are not permitted in either case.
 **/
class FwVars {

  public:

    typedef std::map<std::string, Prefix>  FwVarMap;
    typedef FwVarMap::iterator             iterator;
    typedef FwVarMap::const_iterator       const_iterator;

  public:

    FwVars ( const std::string & varfile = "" ) 
        throw ( Exception );

    virtual ~FwVars();

    bool            parse   ( const std::string & varfile );

    Prefix          find    ( const std::string & name );
    bool            exists  ( const std::string & name );
    
    bool            insert  ( const std::string & name, 
                              Prefix            & pfx );

    bool            insert  ( const std::string & name, 
                              const Prefix      & pfx );

    Prefix          remove  ( const std::string & name );

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


#endif  /* _FWGEN_FWVARS_H_  */

