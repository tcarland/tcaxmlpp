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
#NEED_SOCKET = 1

OPT_FLAGS= 	-g

# custom lib/includes
INCLUDES=       -I.
LIBS=

BIN=		jsontest
OBJS=		jsontest.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: jsontest

include $(TOPDIR)/tcamake/project_defs


jsontest: $(OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o
	@echo

distclean: clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TNMS_PREFIX
	@echo
endif
