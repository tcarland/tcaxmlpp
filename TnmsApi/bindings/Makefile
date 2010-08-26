# Makefile.bindings
#

.PHONY: all java perl python clean java-clean perl-clean python-clean
.DEFAULT: all

SWIG=swig -c++ -I..

CXXFLAGS=-fpic -I.. -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux
LINKFLAGS=-shared -L../lib -lTnmsApi -lxml2 -lpthread
CXXVERSION=$(shell g++ --version | grep GCC | awk '{print $$3}')


all: java perl python


#------------------------------------
# Java
#

JCODE=	libjava_tnmsapi.so \
	java_tnmsapiJNI.java \
	java_tnmsapi.java \
	TnmsApi.java \
	TnmsApiStatus.java \
	TnmsApiErrno.java

java: libjava_tnmsapi.so

java-package: java
	tar czvf java_tnmsapi-gcc-$(CXXVERSION).tgz $(JCODE)

java-zip: java
	zip java_tnmsapi-gcc-$(CXXVERSION).zip $(JCODE)

libjava_tnmsapi.so: java_tnmsapi_wrap.o ../lib/libTnmsApi.a
	$(CXX) $(LINKFLAGS) $< -o $@

java_tnmsapi_wrap.o: java_tnmsapi_wrap.cxx
	$(CXX) $(CXXFLAGS) -c $<

java_tnmsapi_wrap.cxx: java_tnmsapi.i
	$(SWIG) -java $<

java-clean:
	rm -f libjava_tnmsapi.so java_tnmsapi_wrap.o java_tnmsapi_wrap.cxx
	rm -f TnmsApiErrno.java TnmsApiStatus.java TnmsApi.java TnmsAPI.java
	rm -f TnmsApiErrno.class TnmsApiStatus.class TnmsApi.class
	rm -f java_tnmsapi.java java_tnmsapiJNI.java
	rm -f java_tnmsapi.class java_tnmsapiJNI.class SWIGTYPE*.java



#------------------------------------
# Perl
#

PERLFLAGS=$(shell perl -MExtUtils::Embed -e ccopts)
PERLLINK=$(shell perl -MExtUtils::Embed -e ldopts)
PERLCODE=	perl_tnmsapi.so \
		perl_tnmsapi.pm

perl: perl_tnmsapi.so

perl-package: perl
	tar czvf perl_tnmsapi-gcc-$(CXXVERSION).tgz $(PERLCODE)

perl-zip: perl
	zip perl_tnmsapi-gcc-$(CXXVERSION).zip $(PERLCODE)

perl_tnmsapi.so: perl_tnmsapi_wrap.o ../lib/libTnmsApi.a
	$(CXX) $(LINKFLAGS) $(PERLLINK) $< -o $@

perl_tnmsapi_wrap.o: perl_tnmsapi_wrap.cxx
	$(CXX) $(CXXFLAGS) $(PERLFLAGS) -c $<

perl_tnmsapi_wrap.cxx: perl_tnmsapi.i
	$(SWIG) -perl $<

perl-clean:
	rm -f perl_tnmsapi.so perl_tnmsapi_wrap.o perl_tnmsapi_wrap.cxx
	rm -f perl_tnmsapi.pm



#------------------------------------
# Python
#

PYTHONFLAGS=$(shell python2 get-py-info.py --includes)
PYTHONLINK=$(shell python2 get-py-info.py --libs)
PYTHONCODE=	_python_tnmsapi.so \
		python_tnmsapi.py

python: _python_tnmsapi.so

python-package: python
	tar czvf python_tnmsapi-gcc-$(CXXVERSION).tgz $(PYTHONCODE)

python-zip: python
	zip python_tnmsapi-gcc-$(CXXVERSION).zip $(PYTHONCODE)

_python_tnmsapi.so: python_tnmsapi_wrap.o ../lib/libTnmsApi.a
	$(CXX) $(LINKFLAGS) $(PYTHONLINK) $< -o $@

python_tnmsapi_wrap.o: python_tnmsapi_wrap.cxx
	$(CXX) $(CXXFLAGS) $(PYTHONFLAGS) -c $<

python_tnmsapi_wrap.cxx: python_tnmsapi.i
	$(SWIG) -python $<

python-clean:
	rm -f _python_tnmsapi.so python_tnmsapi_wrap.o python_tnmsapi_wrap.cxx
	rm -f python_tnmsapi.py python_tnmsapi.pyc

# 
../lib/libTnmsApi.a:
	cd ..; $(MAKE)

# Clean up
clean: java-clean perl-clean python-clean 

