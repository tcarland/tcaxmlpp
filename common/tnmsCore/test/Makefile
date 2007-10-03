TOPDIR = ../../..
NEED_PTHREADS = 1
NEED_TCANETPP = 1

ifdef DEBUG
OPT_FLAGS =  -g
endif

LIBS =
INCLUDES = -I..

OBJS =          

TEST_OBJS =             test.o  

BIN =                   treetest
ALL_OBJS =              $(OBJS)
ALL_BINS =              $(BIN)


all: treetest


include ${TOPDIR}/common/make/tca


treetest: test.o
	$(make-cxxbin-rule)

clean:
	$(RM) $(OBJS) \
	*.D *.d *.bd *.o lib/*.bd *.bd

distclean: clean
	$(RM) $(BIN)

install:
