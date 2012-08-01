
TOPDIR = ../../..

NEED_PTHREADS = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
ifdef USE_LIBRT
NEED_LIBRT = 1
endif

ifdef TCANMS_DEBUG
OPT_FLAGS =  -g
endif

INCLUDES = 	-I.
LIBS =

OBJS =		        

TEST_OBJS =     tree_test.o oid_test.o msg_test.o \
		msg_server.o msg_client.o

BIN =           treetest oidtest msgtest msgsvr msgclient vertest
ALL_OBJS =      $(OBJS) $(TEST_OBJS)
ALL_BINS =      $(BIN)


all: treetest oidtest msgtest msgsvr msgclient


include ${TOPDIR}/tcamake/project_defs


treetest: tree_test.o
	$(make-cxxbin-rule)

oidtest: oid_test.o
	$(make-cxxbin-rule)

msgtest: msg_test.o
	$(make-cxxbin-rule)

msgsvr: msg_server.o
	$(make-cxxbin-rule)

msgclient: msg_client.o
	$(make-cxxbin-rule)

ver: vertest.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd *.log

distclean: clean
	$(RM) $(ALL_BINS)

install:
