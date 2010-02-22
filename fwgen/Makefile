TOPDIR = ..
NEED_PTHREADS = 1
NEED_TNMSAPI = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1


OPT_FLAGS =	-g

LIBS=

INCLUDES=	-Iinclude

BIN=		fwtest fwlr

OBJS=		src/FwVars.o src/FwZones.o src/FwRules.o
FWTEST_OBJS=	test/fwtest.o
FWLR_OBJS=	src/FwLogEntry.o src/FwLogReport.o src/FwService.o \
		src/FwLogReader.o src/fwlr.o

ALL_OBJS=	$(OBJS) $(FWLR_OBJS) $(FWTEST_OBJS)
ALL_BINS=	$(BIN)

all: fwlr

include $(TOPDIR)/tcamake/project_defs


fwtest: $(OBJS) $(FWTEST_OBJS)
	$(make-cxxbin-rule)

fwlr: $(FWLR_OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)


