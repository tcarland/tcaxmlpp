TOPDIR = ..

NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

OPT_FLAGS= 	-g

LIBS=
INCLUDES=       -Iinclude

BIN=		tnmsd
OBJS=		src/TnmsManager.o src/TnmsTree.o src/TnmsClient.o \
		src/TnmsMessageHandler.o  src/AgentIOHandler.o \
		src/ClientIOHandler.o src/tnmsd.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: tnmsd

include $(TOPDIR)/common/make/tca


tnmsd: $(OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)

install:



