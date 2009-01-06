TOPDIR = ../../
NEED_PTHREADS = 1
NEED_TCANETPP = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES =              -I.

OBJS =                  TcaNmsAPI.o TcaNmsBase.o

BIN =                        
ALL_OBJS =              $(OBJS)
ALL_BINS =              $(BIN)


all: lib


include ${TOPDIR}/common/make/tca

lib:	arlib

arlib:	lib/libtnmsAPI.a

solib:	lib/libtnmsAPI.so.0.0.1

lib/libtnmsAPI.a:  ${OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtnmsAPI.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtnmsAPI.so
	$(make-so-rule)
	( cd lib; ln -s $@ libtnmsAPI.so )
	@echo

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd \
	src/*.D src/*.d src/*.bd src/*.o src/*.bd 

doc-clean:
	( cd docs; $(MAKE) clean )
	@echo

lib-clean:
	$(RM) lib
	@echo

test-clean:
	( cd test; $(MAKE) distclean )
	@echo

distclean: clean lib-clean doc-clean
	$(RM) $(BIN)

install:
