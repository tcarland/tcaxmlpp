#ifndef _INCLUDE_FWZONES_H_
#define _INCLUDE_FWZONES_H_

#include <list>
#include <fstream>
#include <iterator>

#include "fwgen.h"

#include "Exception.hpp"
using namespace tcanetpp;


namespace fwgen {


struct FwZone {
    std::string           zoneName;
    FwDevList             deviceList; 

    FwZone ( const std::string & zonename ) 
        : zoneName(zonename) {}
};



class FwZones {

  public:

    typedef std::map< std::string, FwZone* >  FwZoneMap;
    typedef FwZoneMap::iterator               iterator;
    typedef FwZoneMap::const_iterator         const_iterator;

  public:

    FwZones();
    FwZones ( const std::string & zonefile ) throw ( Exception );
    ~FwZones();


    bool            parse   ( const std::string & zonefile );

    FwZone*         find    ( const std::string & zonename );
    bool            insert  ( const std::string & zonename, 
                              FwZone * zone );
    FwZone*         remove  ( const std::string & zonename );

    bool            exists  ( const std::string & zonename );
    void            clear();
    size_t          size() const;

    iterator        begin();
    iterator        end();
    const_iterator  begin() const;
    const_iterator  end() const;

    void            erase ( iterator & iter );
    void            erase ( const std::string & zonename );
    void            erase ( iterator & begin, iterator & end );

    std::string     getErrorStr() const;


  private:

    FwDevice*       resolveDevice   ( const std::string & devicename );

    void            parseZoneData   ( FwZone        * fwzone, 
    								  std::string   & ln );           

    void            parseDeviceData ( std::ifstream & ifn, 
                                      std::string   & line );

    void            parseInterfaces ( std::ifstream & ifn, 
                                      FwDevice      * fwdev );


  private:

    FwZoneMap       _fwZoneMap;
    std::string     _errStr;
    bool            _debug;
    
};

}  // namespace


#endif  //  _INCLUDE_FWZONES_H_
