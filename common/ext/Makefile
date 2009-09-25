# common/ext/Makefile 
#  For building external libraries
#
TOPDIR = ../..

include ${TOPDIR}/tcamake/project_defs


all: zlib libxml2 net-snmp openssl


help:
	@echo "Make targets are required to build external libs as needed. "
	@echo "Available make targets: net-snmp mysqlpp udplibrary zlib libxml2"

.PHONY: zlib
zlib:
	( cd zlib; sh ./configure; make )
	@echo

update-libxml2:
	( cd libxml2; autoconf; aclocal; libtoolize --copy --force )
	@echo

# note: if configuration fails with libtool errors use 'make update-libxml2'
#
.PHONY: libxml2
libxml2:
	( cd libxml2; chmod u+w aclocal.m4; chmod u+wx configure; \
	 sh ./configure; chmod u+w python/libxml2-py.c; make )
	@echo

net-snmp:
	( cd net-snmp-5.2.3; \
	  ./configure --prefix=/usr --with-gnuld --disable-applications \
	  --disable-manuals --disable-scripts --with-defaults --disable-shared; \
	  chmod u+w stamp-h; \
	  make )
	@echo

openssl:
	( cd openssl-0.9.8k; ./config no-shared; make )
	@echo

clean:
	( cd net-snmp-5.2.3; make clean )
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
	( cd openssl-0.9.8k; make clean )
	@echo

install:
ifdef PREFIX
	@echo "Installing external auth binaries to $(PREFIX)/bin"
	if [ ! -d $(PREFIX)/bin ]; then \
		( mkdir $(PREFIX)/bin ) \
	fi
endif

