TOPDIR = ../..

NEED_ZLIB_INCLUDE = 1
NEED_LIBXML2_INCLUDE = 1
NEED_TCANETPP = 1
NEED_TCAXML = 1
NEED_ZIPSTREAM = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES =              -Iinclude

OBJS =                  src/Pack.o src/TnmsOid.o src/TnmsSocket.o \
			src/TnmsMessage.o src/TnmsMetric.o \
			src/TnmsAuthRequest.o src/TnmsAuthReply.o \
			src/TnmsConfig.o

BIN =                        
ALL_OBJS =              $(OBJS)
ALL_BINS =              $(BIN)


all: lib test


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
	#$(make-lib-rule)
	ld -r -o $@ ${OBJS} ${LFLAGS} ${LIBS}
	@echo

lib/libtnmsCore.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtnmsCore.so
	$(make-so-rule)
	( cd lib; ln -s $@ libtnmsCore.so )
	@echo

clean: test-clean
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

distclean: clean lib-clean doc-clean test-clean
	$(RM) $(BIN)

install:
