# common/ext/Makefile 
#  For building external libraries
#
TOPDIR = ../..

include ${TOPDIR}/tcamake/project_defs


all: zlib net-snmp openssl


help:
	@echo "Make targets are required to build external libs as needed. "
	@echo "Available make targets: net-snmp zlib"

.PHONY: zlib
zlib:
	( cd zlib; sh ./configure; make )
	@echo

net-snmp:
	( cd net-snmp-5.2.3; \
	  ./configure --prefix=/usr --with-gnuld --disable-applications \
	  --disable-manuals --disable-scripts --with-defaults --disable-shared; \
	  chmod u+w stamp-h; \
	  make )
	@echo

clean:
	( cd net-snmp-5.2.3; make clean )
	@echo

distclean:
	if [ -f .external ]; then \
		( rm .external .init) \
	fi
	if [ -f net-snmp-5.2.3/Makefile ]; then \
		( cd net-snmp-5.2.3; make distclean ) \
	fi
	( cd zlib; make distclean )
	@echo

install:
ifdef TNMS_PREFIX
	@echo "Installing external auth binaries to $(PREFIX)/bin"
	if [ ! -d $(PREFIX)/bin ]; then \
		( mkdir $(PREFIX)/bin ) \
	fi
endif

