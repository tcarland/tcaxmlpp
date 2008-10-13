#ifndef _TNMSCORE_SERIALIZABLE_HPP_
#define _TNMSCORE_SERIALIZABLE_HPP_


#include "tcanetpp_types.h"


namespace tnmscore {


class Serializable {

public:

    Serializable() {}
    virtual ~Serializable() {}


    virtual ssize_t  serialize ( char * buffer, size_t buffer_len ) = 0;

};


} // namespace

#endif  //  _TNMSCORE_SERIALIZABLE_HPP_

