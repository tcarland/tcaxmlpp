# Makefile for tcanetpp
#
#
ifdef TCAMAKE_PROJECT
  TOPDIR = ../..
else
  TOPDIR = .
endif

NEED_PTHREADS = 1
NEED_LIBDL = 1

#-------------------#

ifdef DEBUG
  OPT_FLAGS = -g 
# -DEV_DEBUG
endif

INCLUDES =	    -Iinclude
LIBS =

OBJS =		    src/SocketOption.o src/Socket.o src/BufferedSocket.o \
                    src/CircularBuffer.o src/Serializer.o src/EventManager.o \
		    src/CidrUtils.o src/StringUtils.o src/FileUtils.o \
                    src/Thread.o src/ThreadLock.o  src/ThreadMutexPool.o \
		    src/LogFacility.o src/random.o src/RandomPrefix.o \
		    src/patricia.o src/DeviceMap.o \
		    src/NetworkDevice.o src/NetworkInterface.o
                    
                    
CMDBUF_OBJS =       src/CmdBuf.o

PT_OBJS =           src/patricia.o
TEST_OBJS = 	    src/pttest.o
PFX_OBJS =  	    src/pfxtest.o

BIN =		    ptest pfxtest
ALL_OBJS =	    $(OBJS) $(TEST_OBJS) $(PFX_OBJS) $(PT_OBJS) $(CMDBUF_OBJS)
ALL_BINS = 	    $(BIN)


all: lib


include ${TOPDIR}/tcamake/builddefs


lib: arlib

arlib: lib/libtcanetpp.a

solib: lib/libtcanetpp.so.1.0.1

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

lib/libtcanetpp.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtcanetpp.so )
	$(make-so-rule)
	( cd lib; ln -s $@ libtcanetpp.so )
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
ifdef DISTDIR
	@echo "sync'ing tcanetpp to $(DISTDIR)/tcanetpp"
	( $(RDIST) ./ $(DISTDIR)/tcanetpp/ )
	@echo
endif

install: clean
ifdef TCANMS_PREFIX
	@echo "Installing libtcanetpp to $(TCANMS_PREFIX)/lib"
	$(MKDIR) $(TCANMS_PREFIX)/include/tcanetpp
	$(MKDIR) $(TCANMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TCANMS_PREFIX)/include/tcanetpp/
	$(RSYNC) lib/ $(TCANMS_PREFIX)/lib/
	@echo
endif

