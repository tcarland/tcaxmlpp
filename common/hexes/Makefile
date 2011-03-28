# Makefile for libhexes
#
ifdef TCAMAKE_PROJECT
  TOPDIR = ../..
else
  TOPDIR = .
endif 

NEED_NCURSES = 1


ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif
OPT_FLAGS +=	-fPIC -O2
CCSHARED +=	-Wl,-soname,$@

INCLUDES=       -Iinclude
LIBS=		
LFLAGS=		

OBJS=		src/HexApp.o src/HexPanel.o src/HexWindow.o \
		src/HexString.o src/HexStringField.o \
		src/HexDialog.o src/HexStack.o \
		src/LineInputHandler.o src/LineOutputHandler.o

ALL_OBJS=	$(OBJS)


all: lib test


include $(TOPDIR)/tcamake/project_defs


lib: arlib

arlib: lib/libhexes.a

solib: libhexes.so.0.0.1

lib/libhexes.a: $(OBJS)
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

libhexes.so.0.0.1: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) $@ lib/libhexes.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libhexes.so )
	@echo

documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

libclean:
	rm -rf lib
	@echo

doc-clean:
	( cd docs; $(MAKE) clean )
	@echo

.PHONY: test
test:
	( cd test; $(MAKE) all )
	@echo

clean: test-clean
	$(RM) $(ALL_OBJS) \
	*.d *.bd *.D *.o src/*.d src/*.D src/*.bd src/*.o lib/*.bd
	@echo
	
test-clean:
	( cd test; $(MAKE) distclean )
	@echo


distclean: clean libclean test-clean
	$(RM) $(ALL_BINS)
	@echo

dist:
ifdef TNMS_DISTDIR
	@echo "running sync to $(TNMS_DISTDIR)/libhexes"
	( $(RDIST) ./ $(TNMS_DISTDIR)/libhexes/ )
	@echo
endif
	
install:
ifdef TNMS_PREFIX
	@echo "Installing libhexes to $(TNMS_PREFIX)/lib"
	$(MKDIR) $(TNMS_PREFIX)/include/hexes
	$(MKDIR) $(TNMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TNMS_PREFIX)/include/hexes
	$(RSYNC) lib/ $(TNMS_PREFIX)/lib/
	@echo	
endif

