/**  SqlSession.h
  *     Compile time typedef for the underlying database interface 
  *  to allow all database code to simply refer to an 'SqlSession'
  *  independent of the underlying database driver.
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
#ifndef _TCASQLPP_SQLSESSION_HPP_
#define _TCASQLPP_SQLSESSION_HPP_


#define  TCASQLPP_VERSION  "1.2.2"


#include "SqlFactoryInterface.hpp"


#if defined(USE_MYSQL)
#  include "MysqlSession.h"
#elif defined(USE_POSTGRESQL)
#  include "PgsqlSession.h"
#endif


namespace tcasqlpp {

#if defined(USE_MYSQL)
  typedef MysqlSession SqlSession;
#elif defined(USE_POSTGRESQL)
  typedef PgsqlSession SqlSession;
#endif


} // namespace


#endif  /*  _TCASQLPP_SQLSESSION_HPP_  */
