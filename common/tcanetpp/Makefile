# Makefile for tcanetpp
#
ifdef TCAMAKE_PROJECT
  TOPDIR = ../..
else
  TOPDIR = .
endif

NEED_SOCKET = 1
NEED_LIBDL = 1

ifdef USE_LIBRT
NEED_LIBRT = 1
endif
ifdef USE_PTHREADS
NEED_PTHREADS = 1
endif

#-------------------#

ifdef TNMS_DEBUG
  OPT_FLAGS =       -g 
# -DEV_DEBUG
endif

OPT_FLAGS +=	    -fPIC -O2 
# -std=c++0x
CCSHARED += 	    -Wl,-soname,$@

INCLUDES =	    -Iinclude
LIBS =

PT_OBJS =           src/patricia.o
TH_OBJS = 	    src/Thread.o src/ThreadLock.o src/ThreadMutexPool.o
CMDBUF_OBJS =       src/CmdBuffer.o

OBJS =		    src/SocketOption.o src/Socket.o src/BufferedSocket.o \
                    src/CircularBuffer.o src/Serializer.o \
		    src/Whois.o src/EventManager.o \
		    src/IpAddr.o src/AddrInfo.o \
		    src/StringUtils.o src/FileStat.o src/FileUtils.o \
		    src/LogFacility.o src/tcanetpp_random.o \
		    src/patricia.o src/DeviceMap.o \
		    src/NetworkDevice.o src/NetworkInterface.o

ifdef USE_PTHREADS
OBJS +=		    $(TH_OBJS)
endif


BIN =		    ptest pfxtest
ALL_OBJS =	    $(OBJS) $(TH_OBJS) $(PT_OBJS) $(CMDBUF_OBJS)
ALL_BINS = 	    $(BIN)


all: lib


include ${TOPDIR}/tcamake/project_defs


lib: arlib 

arlib: lib/libtcanetpp.a

solib: libtcanetpp.so.1.0.7

libtcapt: lib/libtcapt.a

cmdbuffer: cmdbuf
cmdbuf:    libcmdbuf
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

libtcanetpp.so.1.0.7: ${OBJS}
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

dist: distclean
ifdef TNMS_DISTDIR
	@echo "build distribution to $(TNMS_DISTDIR)/tcanetpp"
	( $(TOPDIR)/tcamake/scripts/tcamake_build.sh dist $(TNMS_DISTDIR) )
	@echo
endif

install: clean
ifdef TNMS_PREFIX
	@echo "Installing libtcanetpp to $(TNMS_PREFIX)/lib"
	$(MKDIR) $(TNMS_PREFIX)/include/tcanetpp
	$(MKDIR) $(TNMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TNMS_PREFIX)/include/tcanetpp/
	$(RSYNC) lib/ $(TNMS_PREFIX)/lib/
	@echo
endif

