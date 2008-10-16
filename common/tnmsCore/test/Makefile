TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES = 	-I../include

OBJS =		        

TEST_OBJS =     tree_test.o pack_test.o oid_test.o typetests.o \
		msg_test.o

BIN =           treetest packtest oidtest typetest msgtest
ALL_OBJS =      $(OBJS) $(TEST_OBJS)
ALL_BINS =      $(BIN)


all: treetest packtest oidtest typetest msgtest


include ${TOPDIR}/common/make/tca


treetest: tree_test.o
	$(make-cxxbin-rule)

packtest: pack_test.o
	$(make-cxxbin-rule)

oidtest: oid_test.o
	$(make-cxxbin-rule)

typetest: typetests.o
	$(make-cxxbin-rule)

msgtest: msg_test.o

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

distclean: clean
	$(RM) $(BIN)

install:
