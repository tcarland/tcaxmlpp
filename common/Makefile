# common Makefile
#
include ../tcamake/projectdefs

all: tcanetpp tcaxml++ tcasnmp tcasql++ ticketmgr tnmsCore


.PHONY: tcanetpp
tcanetpp:
	( cd tcanetpp; make all )
	@echo

.PHONY: tcaxml++
tcaxml++:
	( cd tcaxml++; make all )
	@echo

.PHONY: tcasnmp
tcasnmp:
	( cd tcasnmp; make all )
	@echo

.PHONY: tcasql++
tcasql++:
	( cd tcasql++; make all )
	@echo

.PHONY: tnmsCore
tnmsCore:
	( cd tnmsCore; make all )
	@echo

.PHONY: ticketmgr
ticketmgr:
	( cd ticketmgr; make all )
	@echo

install:
	( cd tcaxml++; make install )
ifdef USE_MYSQL
	( cd tcasql++; make install )
endif
	( cd tcanetpp; make install )
	( cd tcasnmp; make install )
	( cd ticketmgr; make install )
	( cd tnmsCore; make install )
	@echo

clean:
	( cd tcaxml++; make clean )
ifdef USE_MYSQL
	( cd tcasql++; make clean )
endif
	( cd tcanetpp; make clean )
	( cd tcasnmp; make clean )
	( cd ticketmgr; make clean )
	( cd tnmsCore; make clean )
	@echo

distclean:
	( cd tcaxml++; make distclean )
ifdef USE_MYSQL
	( cd tcasql++; make distclean )
endif
	( cd tcanetpp; make distclean )
	( cd tcasnmp; make distclean )
	( cd ticketmgr; make distclean )
	( cd tnmsCore; make distclean )
	@echo

