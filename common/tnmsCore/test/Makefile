TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES = -I..

OBJS =		        

TEST_OBJS =             tree_test.o pack_test.o

BIN =                   treetest packtest
ALL_OBJS =              $(OBJS) $(TEST_OBJS)
ALL_BINS =              $(BIN)


all: treetest packtest


include ${TOPDIR}/common/make/tca


treetest: tree_test.o
	$(make-cxxbin-rule)

packtest: pack_test.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

distclean: clean
	$(RM) $(BIN)

install:
