TOPDIR = ..

NEED_LIBXML2_INCLUDE = 1
NEED_ZLIB_INCLUDE = 1
NEED_TNMSCORE = 1
NEED_ZIPSTREAM = 1

ifdef TNMS_DEBUG
OPT_FLAGS =  -g
endif

OPT_FLAGS += -fPIC -O2

LIBS =
INCLUDES =              -I.

OBJS =                  TnmsAPI.o TnmsBase.o TnmsAgent.o

BIN =                        
ALL_OBJS =              $(OBJS)
ALL_BINS =              $(BIN)


all: lib


include ${TOPDIR}/tcamake/project_defs

lib:	arlib

arlib:	lib/libTnmsApi.a

solib:	lib/libTnmsApi.so.0.0.1

lib/libTnmsApi.a:  ${OBJS}
	( $(MKDIR) lib )
	ld -r -o $@ ${OBJS} ${LFLAGS} ${LIBS}
	@echo

lib/libTnmsApi.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libTnmsApi.so
	$(make-so-rule)
	( cd lib; ln -s $@ libTnmsApi.so )
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
ifdef TNMS_PREFIX
	@echo "Installing TnmsAPI..."
	$(MKDIR) $(TNMS_PREFIX)/tnms/include/tnmsapi
	$(MKDIR) $(TNMS_PREFIX)/tnms/lib
	$(RSYNC) --delete ./*.h $(TNMS_PREFIX)/tnms/include/tnmsapi/
	$(RSYNC) lib/ $(TNMS_PREFIX)/tnms/lib/
	@echo
endif

