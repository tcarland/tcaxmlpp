TOPDIR = ../..

#NEED_NCURSES = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LFLAGS=		
LIBS=		-lpanel -lncurses
INCLUDES=       -Iinclude

BIN=		hextest
OBJS=		src/hextest.o src/TestHexApp.o  \
		src/HexApp.o src/HexPanel.o src/HexWindow.o 

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

