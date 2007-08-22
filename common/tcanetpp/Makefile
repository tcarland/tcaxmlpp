TOPDIR = ../..
NEED_PTHREADS = 1
NEED_LIBDL = 1

ifdef DEBUG
OPT_FLAGS =	-g
endif

LIBS =
INCLUDES =	-Iinclude


OBJS =		src/SocketOption.o src/Socket.o src/BufferedSocket.o \
                src/Cidr.o src/CircularBuffer.o src/StringUtils.o
                
                #src/ConfigOpts.o src/FileUtils.o \
                #src/EventManager.o src/Thread.o src/ThreadLock.o \
                #src/ThreadMutexPool.o src/CirBuffer.o \
                #src/Socket.o src/BufferedSocket.o \
                #src/StringUtils.o src/Logger.o

PT_OBJS =       src/patricia.o
TEST_OBJS =     src/pttest.o
PFX_OBJS =      src/pfxtest.o

BIN =		ptest pfxtest
ALL_OBJS =	$(OBJS) $(TEST_OBJS) $(PFX_OBJS) $(PT_OBJS)
ALL_BINS = 	$(BIN)


all: lib


include ${TOPDIR}/common/make/tca


lib: arlib

arlib: lib/libtcanet.a

solib: lib/libtcanet.so.0.0.1

libtcapt: lib/libtcapt.a

lib/libtcapt.a: ${PTOBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtcanet.a: ${OBJS}
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtcanet.so.0.0.1: ${OBJS}
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtcanet.so )
	$(make-so-rule)
	( cd lib; ln -s $@ libtcanet.so )
	@echo

ptest: ${OBJS} ${TOBJS}
	$(make-cxxbin-rule)

pfxtest: ${OBJS} ${POBJS}
	$(make-cxxbin-rule)


documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

libclean:
	rm -rf lib
	@echo

doc-clean:
	(cd docs; $(MAKE) clean )
	@echo

clean:
	$(RM) $(OBJS) \
	src/*.d src/*.D src/*.bd src/*.o lib/*.bd *.bd

distclean: clean libclean doc-clean
	$(RM) $(BIN)

install:
ifdef SOEMON3_PREFIX
	@echo "Installing libtcanet to $(SOEMON3_PREFIX)/lib"
	$(RSYNC) --delete include/ $(SOEMON3_PREFIX)/include/tcanet/
	$(RSYNC) lib/ $(SOEMON3_PREFIX)/lib/
endif
