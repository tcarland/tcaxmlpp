ifdef TCAMAKE_PROJECT
TOPDIR = ../../..
else
TOPDIR = ..
endif
NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_TCANETPP = 1
NEED_TCASNMP = 1   
NEED_NET_SNMP = 1

OPT_FLAGS =	-g

INCLUDES =	-I.
LIBS =		

BIN =		snmptest
OBJS =		snmp_test.o
ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)

all: snmptest

include ${TOPDIR}/tcamake/builddefs

snmptest: ${OBJS}
	$(make-cxxbin-rule)

clean:
	$(RM) $(OBJS) \
	*.d *.D *.bd *.o

distclean: clean
	$(RM) $(BIN)


