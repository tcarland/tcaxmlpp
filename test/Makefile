#NEED_TCAXMLPP = 1
NEED_LIBXML2 = 1

ifdef TCAMAKE_DEBUG
OPT_FLAGS =     -g
endif

ifeq ($(TCAMAKE_HOME),)
	export TCAMAKE_HOME := $(shell realpath ../../tcamake)
endif


INCLUDES =	-I../include
LIBS =  -L../lib -ltcaxmlpp 

BIN =		XmlTest xmldump nodeftest
OBJS = 		xmltest.o 

ALL_OBJS =	$(OBJS) xmldump.o nodefactorytest.o
ALL_BINS = 	$(BIN)


include $(TCAMAKE_HOME)/tcamake_include


all: XmlTest nodeftest xmldump


XmlTest: $(OBJS)
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
