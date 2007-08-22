#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

extern "C" {
# include <stdio.h>
}

#include <string>
#include <stdexcept>


namespace tcanetpp {


class Exception : public std::runtime_error {
	
public:
	
	explicit Exception ( const std::string & errstr )
		: std::runtime_error(errstr)
	{}
	
	virtual ~Exception() throw() {}
	
	virtual void printErr() const
	{
		fprintf(stderr, "Exception: %s\n", this->what());
	}
	
	virtual std::string toString() const
	{
		return this->what();
	}
	
};

} // namespace


#endif /*EXCEPTION_HPP_*/
