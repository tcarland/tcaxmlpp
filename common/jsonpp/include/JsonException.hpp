#pragma once
#ifndef _JSONPP_JSONEXCEPTION_HPP_
#define _JSONPP_JSONEXCEPTION_HPP_

#include <string>
#include <stdexcept>

namespace jsonpp {


/**  Exception class used by various jsonpp methods */
class JsonException : public std::runtime_error {
  public:
    explicit JsonException ( const std::string & errstr )
        : std::runtime_error(errstr)
    {}
    virtual ~JsonException() throw() {}
    virtual inline std::string toString() const
    {
        return this->what();
    }
};

} // namespace

#endif // _JSONPP_JSONEXCEPTION_HPP_

