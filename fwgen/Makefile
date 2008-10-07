TOPDIR = ..
NEED_PTHREADS = 1
NEED_TCANETPP = 1


OPT_FLAGS =	-g

LIBS=

INCLUDES=	-Iinclude

BIN=		fwtest

OBJS=		src/FwVars.o src/FwZones.o


ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: fwtest

include $(TOPDIR)/common/make/tca


fwtest: $(OBJS) fwtest.o
	$(make-cxxbin-rule)


clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(BIN)


