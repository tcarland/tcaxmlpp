TOPDIR = ../..

NEED_PTHREADS = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_TNMSAPI = 1
NEED_NCURSES = 1
NEED_HEXES = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LIBS=
INCLUDES=       -Iinclude

BIN=		tconsole

CL_OBJS=	src/ClientSubscriber.o src/ClientTreeMutex.o \
		src/ClientIOThread.o src/ClientIOHandler.o

OBJS=		src/TnmsConsoleApp.o  \
		src/tconsole.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tconsole

include $(TOPDIR)/tcamake/project_defs


tconsole: $(CL_OBJS) $(OBJS)
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

