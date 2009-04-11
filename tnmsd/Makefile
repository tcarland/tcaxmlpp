TOPDIR = ..

NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

OPT_FLAGS= 	-g

LIBS=
INCLUDES=       -Iinclude

BIN=		tnmsd
OBJS=		src/TnmsManager.o  \
		src/AgentIOHandler.o src/ClientIOHandler.o \
		src/tnmsd.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tnmsd

include $(TOPDIR)/tcamake/builddefs


tnmsd: $(OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)

install:
ifdef PREFIX
	@echo
endif

