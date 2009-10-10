ifdef TCAMAKE_PROJECT
TOPDIR = ../../..
else
TOPDIR = ..
endif
NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_SOCKET = 1
NEED_TCANETPP = 1

ifdef TCANMS_DEBUG
OPT_FLAGS =	-g -DASWHO_MAIN
endif

INCLUDES=	-I. -I../include
LIBS=

OBJS=		client.o server.o pack_test.o
ASOBJ=          ../src/CmdBuf.o ../src/ASWho.o
CBUFF_OBJS=     cirbuffer_test.o
CIDR_OBJS=	cidrtest.o
PT_OBJS=        
PTT_OBJS=	pt_test.o

BIN=		client server asnwho pttest strtest bufftest cidrtest \
		ipconvert packtest

ALL_OBJS=	$(OBJS) $(ASOBJ) $(STROBJ) $(CBUFF_OBJS) $(CIDR_OBJS)
ALL_BINS= 	$(BIN)



all: client server cidrtest strtest bufftest pttest ipconvert packtest


include ${TOPDIR}/tcamake/project_defs



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

hashtest: hash_test.o
	$(make-cxxbin-rule)
	@echo

pfxtest: pfxtreetest.o
	$(make-cxxbin-rule)
	@echo

pttest: $(PT_OBJS) $(PTT_OBJS)
	$(make-cxxbin-rule)
	@echo

strtest: str_test.o
	$(make-cxxbin-rule)
	@echo

ipconvert: ip_convert.o
	$(make-cxxbin-rule)
	@echo

packtest: pack_test.o
	$(make-cxxbin-rule)


clean:
	$(RM) $(ALL_OBJS) $(BIN) \
	*.d *.D *.bd *.o

distclean: clean


