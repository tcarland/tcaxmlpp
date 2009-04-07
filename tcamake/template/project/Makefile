TOPDIR = ..

# set requirements
#NEED_ZLIB = 1
#NEED_LIBXML2 = 1

OPT_FLAGS= 	-g

# custom lib/includes
INCLUDES=       -Iinclude
LIBS=

BIN=		mybin
OBJS=		src/foo.o src/main.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: mybin

include $(TOPDIR)/tcamake/builddefs


mybin: $(OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o
	@echo

distclean: clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef PREFIX
	@echo
endif
