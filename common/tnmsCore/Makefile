TOPDIR = ../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES =

OBJS =          

BIN =                        
ALL_OBJS =              $(OBJS)
ALL_BINS =              $(BIN)


all: test


include ${TOPDIR}/common/make/tca

.PHONY: test
test:
	( cd test; $(MAKE) all )
	@echo

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

doc-clean:
	( cd doc; $(MAKE) distclean )
	@echo

lib-clean:
	$(RM) lib
	@echo

test-clean:
	( cd test; $(MAKE) distclean )
	@echo

distclean: clean libclean doc-clean test-clean
	$(RM) $(BIN)

install:
