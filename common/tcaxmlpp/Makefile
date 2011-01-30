# Makefile for tcaxmlpp
#
ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif


NEED_LIBXML2_INCLUDE = 1

ifdef TNMS_DEBUG
OPT_FLAGS =     -g
endif


OPT_FLAGS += -fPIC -O2
CCSHARED += 	-Wl,-soname,$@


INCLUDES =	-Iinclude
LIBS = 

BIN =		
OBJS = 		src/XmlDocument.o src/XmlNode.o src/XmlAttribute.o \
		src/XmlDocNode.o 

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: lib


include $(TOPDIR)/tcamake/project_defs


lib: arlib

arlib: lib/libtcaxmlpp.a
solib: libtcaxmlpp.so.1.0.1

lib/libtcaxmlpp.a: $(OBJS)
	@echo $(OBJS)
	( mkdir -p lib )
	ld -r -o $@ $(OBJS) $(LFLAGS)
	@echo

libtcaxmlpp.so.1.0.1: $(OBJS)
	$(make-so-rule)
	$(RM) lib/$@ lib/libtcaxmlpp.so
	( mkdir -p lib; mv $@ lib/; \
	  cd lib; ln -s $@ libtcaxmlpp.so )
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
ifdef TNMS_PREFIX
	$(MKDIR) $(TNMS_PREFIX)/include/tcaxmlpp
	$(MKDIR) $(TNMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TNMS_PREFIX)/include/tcaxmlpp/
	$(RSYNC) lib/ $(TNMS_PREFIX)/lib/
	@echo
endif

