NEED_LIBXML2 = 1

ifdef TCAMAKE_DEBUG
OPT_FLAGS =     -g
endif

INCLUDES =	-I../include
LIBS =  -L../lib -ltcaxmlpp 

BIN =		xmltest xmldump nodeftest
OBJS = 		xmltest.o 

ALL_OBJS =	$(OBJS) xmldump.o nodefactorytest.o
ALL_BINS = 	$(BIN)

# -----------

ifeq ($(TCAMAKE_HOME),)
	export TCAMAKE_HOME := $(shell realpath ../../tcamake)
endif

include $(TCAMAKE_HOME)/tcamake_include

# -----------

all: xmltest nodeftest xmldump


xmltest: $(OBJS)
	$(make-cxxbin-rule)
	@echo

nodeftest: nodefactorytest.o
	$(make-cxxbin-rule)
	@echo

xmldump: xmldump.o
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(OBJS) \
	*.d *.D *.bd *.o lib/*.bd

distclean: clean
	$(RM) $(BIN) xmltestout.xml

install:
