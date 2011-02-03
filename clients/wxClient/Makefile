# Makefile
#
#

ifdef TCAMAKE_PROJECT
    TOPDIR = ../..
else
    TOPDIR = .
endif

# set requirements
NEED_SOCKET = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

# custom lib/includes
INCLUDES=       -Iinclude
LIBS=		`wx-config --libs`
CFLAGS=		`wx-config --cxxflags`

BIN=		TnmsWxClient
OBJS=		src/ClientFrame.o \
		src/TnmsWxTreeCtl.o src/MetricListView.o \
		src/ClientSubscriber.o src/ClientTreeMutex.o \
		src/ClientIOHandler.o src/ClientIOThread.o \
		src/ConnectDialog.o src/TnmsWxClientMain.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: TnmsWxClient tnmsconsole


include $(TOPDIR)/tcamake/project_defs


TnmsWxClient: $(OBJS)
	$(make-cxxbin-rule)
	@echo

tnmsconsole:
	( cd tnms-console; make all )
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
	$(CP) $(BIN) $(TNMS_PREFIX)/bin
	@echo
endif

