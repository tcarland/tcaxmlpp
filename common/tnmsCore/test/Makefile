TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES = 	-I.

OBJS =		        

TEST_OBJS =     tree_test.o pack_test.o oid_test.o msg_test.o \
		msg_server.o msg_client.o

BIN =           treetest packtest oidtest msgtest msgsvr msgclient
ALL_OBJS =      $(OBJS) $(TEST_OBJS)
ALL_BINS =      $(BIN)


all: treetest packtest oidtest msgtest msgsvr msgclient


include ${TOPDIR}/common/make/tca


treetest: tree_test.o
	$(make-cxxbin-rule)

packtest: pack_test.o
	$(make-cxxbin-rule)

oidtest: oid_test.o
	$(make-cxxbin-rule)

msgtest: msg_test.o
	$(make-cxxbin-rule)

msgsvr: msg_server.o
	$(make-cxxbin-rule)

msgclient: msg_client.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd *.log

distclean: clean
	$(RM) $(BIN)

install:
