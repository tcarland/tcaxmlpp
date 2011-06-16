TOPDIR = ../..

NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_SOCKET = 1
NEED_NET_SNMP = 1
NEED_TCANETPP = 1
NEED_TCASNMP = 1
NEED_TCAXMLPP = 1
NEED_LIBXML2 = 1
NEED_ZLIB = 1


ifdef DEBUG
OPT_FLAGS +=	-g
endif

INCLUDES =	-Iinclude
LIBS =

BIN = 		pollap
TOOLS=          

OBJS =		PollerConfig.o PollerManager.o polheir.o

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN) $(TOOLS)


all: pollap

include ${TOPDIR}/tcamake/project_defs


pollap: $(OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o 

distclean: clean
	$(RM) $(ALL_BINS)

install:
ifdef TNMS_PREFIX
	@echo "Installing '$(BIN)' to '$(TNMS_PREFIX)/bin'"
	$(CP) $(BIN) $(TNMS_PREFIX)/bin
	#$(RSYNC) etc/ $(TNMS_PREFIX)/etc/
endif

