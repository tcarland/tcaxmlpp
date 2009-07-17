# Makefile
#
#   Template Makefile for projects
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
NEED_TCANETPP = 1
#NEED_ZLIB = 1
#NEED_LIBXML2 = 1

ifdef DEBUG
OPT_FLAGS= 	-g
endif

# custom lib/includes
INCLUDES=       -Iinclude
LIBS=		`wx-config --libs`
CFLAGS=		`wx-config --cxxflags`

BIN=		TnmsWxClient
OBJS=		src/ClientFrame.o src/TnmsWxTree.o src/TnmsWxClientMain.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: TnmsWxClient

include $(TOPDIR)/tcamake/project_defs


TnmsWxClient: $(OBJS)
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
ifdef PREFIX
	@echo
endif
