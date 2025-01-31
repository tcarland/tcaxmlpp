# Makefile for tcaxmlpp
#
# note: osx linker 'ld' doesn't support soname
NEED_LIBXML2_INCLUDE = 1

ifdef TCAMAKE_DEBUG
OPT_FLAGS =  -g
endif


OPT_FLAGS += -fPIC -O2
CCSHARED  += -Wl,-soname,$@
CXXFLAGS   = -std=c++23

INCLUDES   = -Iinclude
LIBS =

BIN =
OBJS =	src/XmlDocument.o src/XmlNode.o src/XmlAttribute.o \
		src/XmlDocNode.o

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


ifeq ($(TCAMAKE_HOME),)
	export TCAMAKE_HOME := $(shell realpath ../tcamake)
endif

include $(TCAMAKE_HOME)/tcamake_include


all: lib


lib: arlib solib
arlib: lib/libtcaxmlpp.a
solib: libtcaxmlpp.so.1.3.0

lib/libtcaxmlpp.a: $(OBJS)
	@echo $(OBJS)
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

libtcaxmlpp.so.1.3.0: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) lib/$@ lib/libtcaxmlpp.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libtcaxmlpp.so )
	@echo

lib-clean:
	rm -rf lib
	@echo

documentation:
	(cd docs; $(MAKE) $(MFLAGS) $(MVARS) all )
	@echo

doc-clean:
	(cd docs; $(MAKE) clean )
	@echo

.PHONY: test
test:
	( cd test; $(MAKE) all )
	@echo

test-clean:
	( cd test; $(MAKE) clean )
	@echo

test-distclean:
	(cd test; $(MAKE) distclean )
	@echo

clean: test-clean
	$(RM) $(OBJS) \
	src/*.d src/*.D *.bd src/*.o lib/*.bd
	@echo

distclean: clean lib-clean test-distclean doc-clean
	$(RM) $(BIN)
	@echo

install:
ifdef TCAMAKE_PREFIX
	$(MKDIR) $(TCAMAKE_PREFIX)/include/tcaxmlpp
	$(MKDIR) $(TCAMAKE_PREFIX)/lib
	$(RSYNC) --delete include/ $(TCAMAKE_PREFIX)/include/tcaxmlpp/
	$(RSYNC) lib/ $(TCAMAKE_PREFIX)/lib/
	@echo
endif
