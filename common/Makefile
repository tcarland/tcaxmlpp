# common Makefile
#
include ../tcamake/build_defs

all: tcanetpp tcaxmlpp tcasnmp tcasqlpp tnmsSession tnmsCore


.PHONY: tcanetpp
tcanetpp:
	( cd tcanetpp; make all )
	@echo

.PHONY: tcaxmlpp
tcaxmlpp:
	( cd tcaxmlpp; make all )
	@echo

.PHONY: tcasnmp
tcasnmp:
	( cd tcasnmp; make all )
	@echo

.PHONY: tcasqlpp
tcasqlpp:
	( cd tcasqlpp; make all )
	@echo

.PHONY: tnmsCore
tnmsCore:
	( cd tnmsCore; make all )
	@echo

.PHONY: tnmsSession
tnmsSession:
	( cd tnmsSession; make all )
	@echo

install:
	( cd tcaxmlpp; make install )
ifdef USE_MYSQL
	( cd tcasqlpp; make install )
endif
	( cd tcanetpp; make install )
	( cd tcasnmp; make install )
	( cd tnmsSession; make install )
	( cd tnmsCore; make install )
	@echo

clean:
	( cd tcaxmlpp; make clean )
ifdef USE_MYSQL
	( cd tcasqlpp; make clean )
endif
	( cd tcanetpp; make clean )
	( cd tcasnmp; make clean )
	( cd tnmsSession; make clean )
	( cd tnmsCore; make clean )
	@echo

distclean:
	( cd tcaxmlpp; make distclean )
ifdef USE_MYSQL
	( cd tcasqlpp; make distclean )
endif
	( cd tcanetpp; make distclean )
	( cd tcasnmp; make distclean )
	( cd tnmsSession; make distclean )
	( cd tnmsCore; make distclean )
	@echo

