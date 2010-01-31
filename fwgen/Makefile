TOPDIR = ..
NEED_PTHREADS = 1
NEED_TNMSAPI = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1


OPT_FLAGS =	-g

LIBS=

INCLUDES=	-Iinclude

BIN=		fwtest fwlogp

OBJS=		src/FwVars.o src/FwZones.o src/FwRules.o

LOG_OBJS=	src/FwLogEntry.o src/FwLogReport.o

ALL_OBJS=	$(OBJS) $(LOG_OBJS)
ALL_BINS=	$(BIN)

all: fwlogp

include $(TOPDIR)/tcamake/project_defs


fwtest: $(OBJS) fwtest.o
	$(make-cxxbin-rule)

fwlogp: $(LOG_OBJS) fwlogparse.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(BIN)


