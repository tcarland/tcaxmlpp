TOPDIR = ../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES =              -Iinclude

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

lib:	arlib

arlib:	lib/libtnmsCore.a

solib:	lib/libtnmsCore.so.0.0.1

lib/libtnmsCore.a:  ${OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtnmsCore.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtnmsCore.so
	$(make-so-rule)
	( cd lib; ln -s $@ libtnmsCore.so )
	@echo

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

doc-clean:
	( cd docs; $(MAKE) distclean )
	@echo

lib-clean:
	$(RM) lib
	@echo

test-clean:
	( cd test; $(MAKE) distclean )
	@echo

distclean: clean lib-clean doc-clean test-clean
	$(RM) $(BIN)

install:
