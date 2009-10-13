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

ifdef TCANMS_DEBUG
OPT_FLAGS= 	-g
endif

INCLUDES=       -Iinclude
LIBS=

BIN=		archiver
OBJS=		src/ArchiveConfig.o src/Archiver.o src/ArchiverThread.o \
		src/ArchiveMessageHandler.o src/ArchiveClient.o \
		TnmsArchiveManager.o archiver.cpp

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: archiver

include $(TOPDIR)/tcamake/project_defs


archiver: $(OBJS)
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
ifdef TCANMS_PREFIX
	@echo
endif
