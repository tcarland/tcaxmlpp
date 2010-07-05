# Makefile
#
# TOPDIR is the relative path to our workspace root
# where /tcamake should exist.
#
# The following ifdef is used for extracting the Makefile 
# into a standalone project. See 'build.sh' for extracting
# solo distributions.  In most cases this is not necessary, 
# simply set TOPDIR to the relative root and compile.
ifdef TCAMAKE_PROJECT
    TOPDIR = ../..
else
    TOPDIR = .
endif

# set requirements
NEED_SOCKET = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_TNMSAPI = 1
NEED_TCASQLPP = 1
NEED_PTHREADS = 1

ifdef USE_MYSQL
NEED_MYSQL = 1
endif

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

INCLUDES=       -Iinclude
LIBS=

BIN=		tnmsarchived
OBJS=		src/ArchiveConfig.o src/ArchiverThread.o \
		src/ArchiveMessageHandler.o src/ArchiveSubscriber.o \
		src/ArchiveClient.o src/ClientIOHandler.o \
		src/DbMaintainer.o src/DbArchiver.o \
		src/TnmsArchiveManager.o src/archiver_main.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: tnmsarchived

include $(TOPDIR)/tcamake/project_defs


tnmsarchived: $(OBJS)
	$(make-cxxbin-rule)
	@echo

.PHONY: test
test:

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o
	@echo

distclean: clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TNMS_PREFIX
	$(CP) $(BIN) $(TNMS_PREFIX)/tnms/sbin
	$(CP) etc/ta_dbinit.sh $(TNMS_PREFIX)/tnms/bin
	$(CP) etc/ta_dbcreatesql.sh $(TNMS_PREFIX)/tnms/bin
	$(CP) etc/ta_credentials.conf.example $(TNMS_PREFIX)/tnms/etc
	@echo
endif

