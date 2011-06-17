TOPDIR = ../..

NEED_ZLIB_INCLUDE = 1
NEED_LIBXML2_INCLUDE = 1
NEED_TCANETPP = 1
NEED_TCAXMLPP = 1
NEED_ZIPSTREAM = 1

ifdef TNMS_DEBUG
OPT_FLAGS =  -g
endif

OPT_FLAGS +=	-fPIC -O2
CCSHARED += 	-Wl,-soname,$@

INCLUDES =      -Iinclude
LIBS =

OBJS =          src/TnmsOid.o src/TnmsSocket.o \
		src/TnmsMessage.o src/TnmsMetric.o \
		src/TnmsAuthRequest.o src/TnmsAuthReply.o \
		src/AuthClient.o \
		src/TnmsConfig.o src/TnmsMessageHandler.o \
		src/TnmsTree.o src/TnmsClient.o \
		src/TnmsSubscriber.o

BIN =                        
ALL_OBJS =      $(OBJS)
ALL_BINS =      $(BIN)


all: lib test


include ${TOPDIR}/tcamake/project_defs

.PHONY: test
test:
	( cd test; $(MAKE) all )
	@echo

lib:	arlib

arlib:	lib/libtnmsCore.a

solib:	lib/libtnmsCore.so.0.1.1

lib/libtnmsCore.a:  ${OBJS}
	( $(MKDIR) lib )
	#$(make-lib-rule)
	# custom link to include all our libs
	ld -r -o $@ ${OBJS} ${LFLAGS} ${LIBS}
	@echo

lib/libtnmsCore.so.0.1.1: ${OBJS}
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
ifdef TNMS_PREFIX
	@echo "Installing libtnmsCore to $(TNMS_PREFIX)/lib"
	$(MKDIR) $(TNMS_PREFIX)/include/tnmsCore
	$(MKDIR) $(TNMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TNMS_PREFIX)/include/tnmsCore/
	$(RSYNC) lib/ $(TNMS_PREFIX)/lib/

endif
