TOPDIR = ../..

NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_TNMSAPI = 1

OPT_FLAGS= 	-g

LIBS=
INCLUDES=       -Iinclude

BIN=		tnms-console

OBJS=		src/tnms_console.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tnms-console

include $(TOPDIR)/tcamake/builddefs


tnms-console: $(OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean
	$(RM) $(ALL_BINS)

install:
ifdef TCANMS_PREFIX
	@echo
endif

