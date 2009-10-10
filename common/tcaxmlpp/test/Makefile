ifdef TCAMAKE_PROJECT
TOPDIR = ../../..
else
TOPDIR = ..
endif

NEED_TCAXMLPP = 1
NEED_LIBXML2 = 1

ifdef TCANMS_DEBUG
OPT_FLAGS =     -g
endif

INCLUDES =	
LIBS = 

BIN =		XmlTest xmldump
OBJS = 		xmltest.o 

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: XmlTest xmldump

include $(TOPDIR)/tcamake/project_defs


XmlTest: $(OBJS)
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
