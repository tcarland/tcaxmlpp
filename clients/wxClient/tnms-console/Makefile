TOPDIR = ../../..

NEED_PTHREADS = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_TNMSAPI = 1
NEED_NCURSES = 1
NEED_HEXES = 1

ifdef USE_LIBRT
NEED_LIBRT = 1
endif

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LIBS=
INCLUDES=       -I../include -I.

BIN=		tnms-console

CL_OBJS=	../src/ClientSubscriber.o ../src/ClientTreeMutex.o \
		../src/ClientIOThread.o ../src/ClientIOHandler.o

OBJS=		TnmsConsoleApp.o  tnmsconsole.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tnms-console

include $(TOPDIR)/tcamake/project_defs


tnms-console: $(CL_OBJS) $(OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)

install:
ifdef TNMS_PREFIX
	$(CP) $(BIN) $(TNMS_PREFIX)/bin
	@echo
endif

