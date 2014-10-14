# Makefile
#
ifdef TCAMAKE_PROJECT
    TOPDIR = ../..
else
    TOPDIR = .
endif

# set requirements
NEED_SOCKET = 1

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

OPT_FLAGS += 	-fPIC -O2
CCSHARED +=	-Wl,-soname,$@


INCLUDES=       -Iinclude
LIBS=

BIN=		
OBJS=		src/JsonObject.o src/JsonArray.o src/JSON.o

ALL_OBJS=	$(OBJS)
ALL_BINS=	$(BIN)

all: lib

include $(TOPDIR)/tcamake/project_defs

lib: arlib

arlib: lib/libjsonpp.a

solib: libjsonpp.so.0.2.2

lib/libjsonpp.a: $(OBJS)
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

libjsonpp.so.0.2.2: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) lib/$@ lib/libjsonpp.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libjsonpp.so )
	@echo

.PHONY: test
test: 
	( cd test; make all )
	@echo

documentation:
	(cd docs; ${MAKE} ${MFLAGS} ${MVARS} all )
	@echo

doc-clean:
	( cd docs; $(MAKE) clean )
	@echo

libclean:
	( $(RM) lib )
	@echo

testclean:
	( cd test; $(MAKE) distclean )
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o lib/*.bd
	@echo

distclean: clean libclean testclean doc-clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TNMS_PREFIX
	@echo
endif
