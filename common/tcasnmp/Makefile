ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif

NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_TCANETPP = 1
NEED_NET_SNMP = 1


ifdef DEBUG
OPT_FLAGS = -g
endif

INCLUDES =	-Iinclude
LIBS =

BIN =		snmptest
OBJS =		src/SnmpSession.o src/SnmpOid.o src/SnmpPdu.o \
		src/SnmpUtils.o 
TOBJS = 	test/snmp_test.o

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: lib

include ${TOPDIR}/tcamake/builddefs


lib: arlib

arlib: lib/libtcasnmp.a

solib: libtcasnmp.so.0.0.1

lib/libtcasnmp.a: ${OBJS}
	( mkdir -p lib )
	ld -r -o $@ ${OBJS}
	@echo

libtcasnmp.so.0.0.1: ${OBJS}
	$(make-so-rule)
	$(RM) lib/$@ lib/libtcasnmp.so
	( mkdir -p lib; mv $@ lib/; \
	  cd lib; ln -s $@ libtcasnmp.so )
	@echo

snmptest: ${OBJS} ${TOBJS}
	$(CXX) -o $@ $(OBJS) $(TOBJS) 


documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

libclean:
	rm -rf lib
	@echo

doc-clean:
	(cd docs; $(MAKE) clean )
	@echo

clean:
	$(RM) $(OBJS) \
	src/*.d src/*.D src/*.bd src/*.o lib/*.bd *.bd

distclean: clean libclean doc-clean
	$(RM) $(BIN)

install:
ifdef TCANMS_PREFIX
	@echo
endif
