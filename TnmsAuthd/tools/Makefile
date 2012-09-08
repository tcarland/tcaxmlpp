TOPDIR = ../..

NEED_TCASQLPP = 1
NEED_TNMSCORE = 1
NEED_PTHREADS = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

ifdef USE_LIBRT
NEED_LIBRT = 1
endif
ifdef USE_MYSQL
NEED_MYSQL = 1
endif

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LIBS=
INCLUDES=       -I.

BIN=		tnms_useradd


OBJS=		tnms_useradd.o tnms_groupadd.o
ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: tnms_useradd

include $(TOPDIR)/tcamake/project_defs


tnms_useradd: tnms_useradd.o
	$(make-cxxbin-rule)
	@echo

tnms_groupadd: tnms_groupadd.o
	$(make-cxxbin-rule)
	@echo

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

