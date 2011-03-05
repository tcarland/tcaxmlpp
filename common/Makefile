# common Makefile

include ../tcamake/build_defs


all: hexes tcanetpp tcaxmlpp tcasnmp tcasqlpp tnmsSession tnmsCore


.PHONY: hexes
hexes:
	( cd hexes; make all )
	@echo

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

ifdef USE_MYSQL
.PHONY: tcasqlpp
tcasqlpp:
	( cd tcasqlpp; make all )
	@echo
endif

.PHONY: tnmsCore
tnmsCore:
	( cd tnmsCore; make all )
	@echo

.PHONY: tnmsSession
tnmsSession:
	( cd tnmsSession; make all )
	@echo

install:
ifdef USE_MYSQL
	( cd tcasqlpp; make install )
endif
	( cd hexes; make install )
	( cd tcanetpp; make install )
	( cd tcaxmlpp; make install )
	( cd tcasnmp; make install )
	( cd tnmsSession; make install )
	( cd tnmsCore; make install )
	@echo

clean:
ifdef USE_MYSQL
	( cd tcasqlpp; make clean )
endif
	( cd hexes; make clean )
	( cd tcanetpp; make clean )
	( cd tcaxmlpp; make clean )
	( cd tcasnmp; make clean )
	( cd tnmsSession; make clean )
	( cd tnmsCore; make clean )
	@echo

distclean:
ifdef USE_MYSQL
	( cd tcasqlpp; make distclean )
endif
	( cd hexes; make distclean )
	( cd tcanetpp; make distclean )
	( cd tcaxmlpp; make distclean )
	( cd tcasnmp; make distclean )
	( cd tnmsSession; make distclean )
	( cd tnmsCore; make distclean )
	@echo

