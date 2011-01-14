TOPDIR = ../../..

NEED_NCURSES = 1
NEED_HEXES = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LFLAGS=		
INCLUDES=       -I.

BIN=		hextest
OBJS=		hextest.o TestHexApp.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: hextest

include $(TOPDIR)/tcamake/project_defs


hextest: $(OBJS)
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

