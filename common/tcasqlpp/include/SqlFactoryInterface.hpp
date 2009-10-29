#ifndef _TCASQL_SQLFACTORYINTERFACE_HPP_
#define _TCASQL_SQLFACTORYINTERFACE_HPP_


namespace tcasqlpp {


class SqlSessionInterface;

/**  SqlSessionInterface defines the pure virtual interface for 
  *  instantiating new database session objects primarily used 
  *  by the SqlDbPool class.
 **/
class SqlFactoryInterface {
  public:
    virtual ~SqlFactoryInterface() {}

    virtual SqlSessionInterface* operator() ( SqlSessionInterface * master ) = 0;
};


} // namespace


#endif  // _TCASQL_SQLFACTORYINTERFACE_HPP_
