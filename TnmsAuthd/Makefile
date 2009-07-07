TOPDIR = ..

NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

OPT_FLAGS= 	-g

LIBS=
INCLUDES=       -Iinclude

BIN=		tnmsauthd
OBJS=		src/TnmsAuthdManager.o  \
		src/SoapIOHandler.o src/SoapClient.o src/SoapThread.o \
		src/AuthIOHandler.o src/AuthdClient.o \
		src/AuthDbThread.o \
		src/tnmsauthd.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tnmsd

include $(TOPDIR)/tcamake/project_defs


tnmsd: $(OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)

install:
ifdef TCANMS_PREFIX
	$(CP) $(BIN) $(TCANMS_PREFIX)/bin
	@echo
endif

