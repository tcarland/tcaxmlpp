/*
 * TnmsRecordTypes.h
 *
 *  Created on: Oct 12, 2008
 *      Author: tca
 */

#ifndef TNMSRECORDTYPES_H_
#define TNMSRECORDTYPES_H_


namespace tnmscore {


#include "TnmsMetric.h"


class TnmsRequest : public Serialize {

    TnmsRequest()
    TnmsRequest ( const std::string & name );
    TnmsRequest ( TnmsOid * oid );

    virtual ~TnmsRequest() {}

    bool Serialize

};
}

#endif /* TNMSRECORDTYPES_H_ */
