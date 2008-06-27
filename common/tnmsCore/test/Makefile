TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES = 	-I../include

OBJS =		        

TEST_OBJS =     tree_test.o pack_test.o oid_test.o

BIN =           treetest packtest oidtest
ALL_OBJS =      $(OBJS) $(TEST_OBJS)
ALL_BINS =      $(BIN)


all: treetest packtest oidtest


include ${TOPDIR}/common/make/tca


treetest: tree_test.o
	$(make-cxxbin-rule)

packtest: pack_test.o
	$(make-cxxbin-rule)

oidtest: oid_test.o ../src/TnmsOid.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

distclean: clean
	$(RM) $(BIN)

install:
