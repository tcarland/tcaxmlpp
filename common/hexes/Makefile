TOPDIR = ../..

NEED_NCURSES = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LFLAGS=		
LIBS=		
INCLUDES=       -Iinclude

BIN=		hextest

OBJS=		src/HexApp.o src/HexPanel.o src/HexWindow.o 

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)


all: lib

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

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o
	@echo
	
test-clean:
	( cd test; $(MAKE) distclean )
	@echo


distclean: clean libclean test-clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TNMS_PREFIX
	$(CP) $(BIN) $(TNMS_PREFIX)/bin
	@echo
endif

