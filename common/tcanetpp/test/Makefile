TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_SOCKET = 1
NEED_TCANETPP = 1


OPT_FLAGS =	-g -DASWHO_MAIN

LIBS=

INCLUDES=	-I.

BIN=		client server asnwho strtest bufftest cidrtest

OBJS=		client.o server.o
ASOBJ=          ../src/CmdBuf.o ../src/ASWho.o
CBUFF_OBJS=     cirbuffer_test.o
CIDR_OBJS=	cidrtest.o

ALL_OBJS=	$(OBJS) $(ASOBJ) $(STROBJ) $(CBUFF_OBJS) $(CIDR_OBJS)
ALL_BINS= 	$(BIN)



all: client server cidrtest bufftest


include ${TOPDIR}/common/make/tca



client: client.o
	$(make-cxxbin-rule)
	@echo

server: server.o
	$(make-cxxbin-rule)
	@echo

asnwho: $(ASWHO)
	$(make-cxxbin-rule)
	@echo

cidrtest: $(CIDR_OBJS)
	$(make-cxxbin-rule)
	@echo

bufftest: $(CBUFF_OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) $(BIN) \
	*.d *.D *.bd *.o

distclean: clean


