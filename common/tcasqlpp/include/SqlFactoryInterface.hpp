/**  SqlFactoryInterface.hpp
  *      Interface used by SqlDbPool for creating a pool of SqlSession
  *   objects.
  *  
  * Copyright (c) 2008,2009 Timothy Charlton Arland 
  *  @Author  tca@charltontechnology.net
  *
  * This file is part of tcasqlpp.
  *
  * tcasqlpp is free software: you can redistribute it and/or modify
  * it under the terms of the GNU Lesser General Public License as 
  * published by the Free Software Foundation, either version 3 of 
  * the License, or (at your option) any later version.
  *
  * tcasqlpp is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public 
  * License along with tcasqlpp.  
  * If not, see <http://www.gnu.org/licenses/>.
  *
 **/
#ifndef _TCASQLPP_SQLFACTORYINTERFACE_HPP_
#define _TCASQLPP_SQLFACTORYINTERFACE_HPP_


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


#endif  // _TCASQLPP_SQLFACTORYINTERFACE_HPP_
