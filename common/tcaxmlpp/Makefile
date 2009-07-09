ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif


NEED_LIBXML2_INCLUDE = 1



ifdef DEBUG
OPT_FLAGS =     -g
endif

INCLUDES =	-Iinclude
LIBS = 

BIN =		
OBJS = 		src/XmlDocument.o src/XmlNode.o src/XmlAttribute.o

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: lib


include $(TOPDIR)/tcamake/project_defs


lib: arlib

arlib: lib/libtcaxmlpp.a

solib: libtcaxmlpp.so.0.1.1

lib/libtcaxmlpp.a: $(OBJS)
	@echo $(OBJS)
	( mkdir -p lib )
	ld -r -o $@ $(OBJS) $(LFLAGS)
	@echo

libtcaxmlpp.so.0.1.1: $(OBJS)
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

distclean: clean lib-clean test-distclean
	$(RM) $(BIN)
	@echo

install:
