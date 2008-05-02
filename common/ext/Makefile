# common/ext/Makefile 
#  For building external libraries
#
TOPDIR = ../..

include ${TOPDIR}/common/make/tca

#BOOST_ROOT = "boost_1_33_1"
# for now, this is not so cross platform
#BOOST_JAM  = tools/build/jam_src/bin.linuxx86/bjam


all: zlib net-snmp


help:
	@echo "Make targets are required to build external libs as needed. "
	@echo "Available make targets: net-snmp mysqlpp udplibrary zlib libxml2"


net-snmp:
	( cd net-snmp-5.2.3; \
	  ./configure --prefix=/usr --with-gnuld --disable-applications \
	  --disable-manuals --disable-scripts --with-defaults --disable-shared; \
	  chmod u+w stamp-h;\
	  make )
	@echo

.PHONY: zlib
zlib:
	( cd zlib; sh ./configure; make )
	@echo

.PHONY: libxml2
libxml2:
	( cd libxml2; chmod u+w aclocal.m4; chmod u+w configure; \
	libtoolize --copy --force; sh ./configure; \
	chmod u+w python/libxml2-py.c; make )
	@echo

.PHONY: openssl
openssl:
	( cd openssl; ./config no-shared; \
	chmod u+w crypto/objects/obj_dat.h; make )
	@echo


clean:
	( cd net-snmp-5.2.3; make clean )
	( cd mysqlpp/mysql++-1.7.40; make clean )
	@echo

distclean:
	if [ -f .external ]; then \
		( rm .external ) \
	fi
	if [ -f net-snmp-5.2.3/Makefile ]; then \
		( cd net-snmp-5.2.3; make distclean ) \
	fi
	( cd zlib; make distclean )
	if [ -f libxml2/Makefile ]; then\
		( cd libxml2; make distclean ) \
	fi
	( cd openssl; make clean )
	if [ -f mysql++-1.7.40/Makefile ]; then \
		( cd mysql++-1.7.40; make distclean ) \
	fi
	@echo

install:
ifdef SHAMON_PREFIX
	@echo "Installing external auth binaries to $(SOEMON3_PREFIX)/bin"
	if [ ! -d $(SOEMON3_PREFIX)/bin ]; then \
		( mkdir $(SOEMON3_PREFIX)/bin ) \
	fi
endif

