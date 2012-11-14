ifdef TCAMAKE_PROJECT
    TOPDIR = ../../..
else
    TOPDIR = ..
endif

NEED_LIBDL = 1
NEED_SOCKET = 1
NEED_TCANETPP = 1

ifdef USE_LIBRT
NEED_LIBRT = 1
endif
ifdef USE_PTHREADS
NEED_PTHREADS = 1
endif

ifdef TNMS_DEBUG
OPT_FLAGS =	-g -DTWHOIS_MAIN
endif

INCLUDES=	-I. -I../include
LIBS=


ASOBJ=          ../src/IpAddr.o ../src/AddrInfo.o \
		../src/tcanetpp_random.o ../src/Socket.o \
		../src/SocketOption.o ../src/Whois.o

OBJS=		client.o server.o pack_test.o ping.o

CBUFF_OBJS=     cirbuffer_test.o
CIDR_OBJS=	cidrtest.o
PT_OBJS=        
PTT_OBJS=	pt_test.o

BIN=		asnwho ipconvert client server cachetest pttest pfxtest \
		strtest bufftest cidrtest packtest pingt gktrace filetest

ALL_OBJS=	$(OBJS) $(ASOBJ) $(STROBJ) $(CBUFF_OBJS) $(CIDR_OBJS)
ALL_BINS= 	$(BIN)



all: asnwho ipconvert client server cidrtest strtest bufftest pttest \
	pingt gktrace packtest filetest


include ${TOPDIR}/tcamake/project_defs


asnwho: $(ASOBJ)
ifdef TNMS_DEBUG
	$(CXX) -o $@ $(CFLAGS) $^ -ldl
	@echo
endif 

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

filetest: fileutil_test.o
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

thtest: threadtest.o
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) $(BIN) \
	*.d *.D *.bd *.o

distclean: clean


