ifdef TCAMAKE_PROJECT
    TOPDIR = ../../..
else
    TOPDIR = ..
endif

NEED_LIBDL = 1
NEED_SOCKET = 1
NEED_TCANETPP = 1

ifdef USE_PTHREADS
NEED_PTHREADS = 1
endif

ifdef GK_DEBUG
OPT_FLAGS =	-g -DASWHO_MAIN
endif

INCLUDES=	-I. -I../include
LIBS=


ASOBJ=          ../src/CmdBuf.o ../src/ASWho.o

OBJS=		client.o server.o pack_test.o ping.o

CBUFF_OBJS=     cirbuffer_test.o
CIDR_OBJS=	cidrtest.o
PT_OBJS=        
PTT_OBJS=	pt_test.o

BIN=		pingt asnwho ipconvert client server cachetest pttest \
		strtest bufftest cidrtest packtest gktrace

ALL_OBJS=	$(OBJS) $(ASOBJ) $(STROBJ) $(CBUFF_OBJS) $(CIDR_OBJS)
ALL_BINS= 	$(BIN)



all: pingt ipconvert client server cidrtest strtest bufftest pttest \
	gktrace packtest


include ${TOPDIR}/tcamake/project_defs


asnwho: $(ASWHO)
	$(make-cxxbin-rule)
	@echo

pingt: ping.o
	$(make-cxxbin-rule)
	@echo


client: client.o
	$(make-cxxbin-rule)
	@echo

server: server.o
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

cachetest: pfxcachetest.o
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
	@echo

gktrace: trace_test.o
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) $(BIN) \
	*.d *.D *.bd *.o

distclean: clean


