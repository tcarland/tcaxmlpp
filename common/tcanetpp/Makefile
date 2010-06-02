# Makefile for tcanetpp
#
#
ifdef TCAMAKE_PROJECT
  TOPDIR = ../..
else
  TOPDIR = .
endif

NEED_SOCKET = 1
NEED_LIBDL = 1
ifdef USE_PTHREADS
NEED_PTHREADS = 1
endif

#-------------------#

ifdef GK_DEBUG
  OPT_FLAGS =       -g 
# -DEV_DEBUG
endif

OPT_FLAGS +=	    -fPIC -O2
CCSHARED += 	    -Wl,-soname,$@

INCLUDES =	    -Iinclude
LIBS =

TH_OBJS = 	    src/Thread.o src/ThreadLock.o  src/ThreadMutexPool.o
OBJS =		    src/SocketOption.o src/Socket.o src/BufferedSocket.o \
                    src/CircularBuffer.o src/Serializer.o \
		    src/Whois.o src/EventManager.o \
		    src/CidrUtils.o src/StringUtils.o src/FileUtils.o \
		    src/LogFacility.o src/random.o src/RandomPrefix.o \
		    src/patricia.o src/DeviceMap.o \
		    src/NetworkDevice.o src/NetworkInterface.o
ifdef USE_PTHREADS
OBJS +=		    $(TH_OBJS)
endif

CMDBUF_OBJS =       src/CmdBuf.o

PT_OBJS =           src/patricia.o
TEST_OBJS = 	    src/pttest.o
PFX_OBJS =  	    src/pfxtest.o

BIN =		    ptest pfxtest
ALL_OBJS =	    $(OBJS) $(TH_OBJS) $(TEST_OBJS) $(PFX_OBJS) $(PT_OBJS) \
		    $(CMDBUF_OBJS)
ALL_BINS = 	    $(BIN)


all: lib


include ${TOPDIR}/tcamake/project_defs


lib: arlib solib

arlib: lib/libtcanetpp.a

solib: libtcanetpp.so.0.1.1

libtcapt: lib/libtcapt.a

libcmdbuf: lib/libcmdbuf.a

lib/libtcapt.a: ${PT_OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libcmdbuf.a: ${CMDBUF_OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtcanetpp.a: ${OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

libtcanetpp.so.0.1.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtcanetpp.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libtcanetpp.so )
	@echo

documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

libclean:
	rm -rf lib
	@echo

doc-clean:
	( cd docs; $(MAKE) clean )
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	src/*.d src/*.D src/*.bd src/*.o lib/*.bd *.bd
	@echo
	
test-clean:
	( cd test; $(MAKE) distclean )
	@echo

distclean: clean libclean doc-clean test-clean
	$(RM) $(ALL_BINS)
	@echo

dist:
ifdef GK_DISTDIR
	@echo "sync'ing tcanetpp to $(GK_DISTDIR)/tcanetpp"
	( $(RDIST) ./ $(GK_DISTDIR)/tcanetpp/ )
	@echo
endif

install: clean
ifdef GK_PREFIX
	@echo "Installing libtcanetpp to $(GK_PREFIX)/lib"
	$(MKDIR) $(GK_PREFIX)/include/tcanetpp
	$(MKDIR) $(GK_PREFIX)/lib
	$(RSYNC) --delete include/ $(GK_PREFIX)/include/tcanetpp/
	$(RSYNC) lib/ $(GK_PREFIX)/lib/
	@echo
endif

