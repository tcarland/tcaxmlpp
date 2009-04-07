# common Makefile
#


all: libproject


.PHONY: libproject
libproject:
	( cd libproject; make all )
	@echo
clean:
	( cd libproject; make clean )
	@echo

distclean:
	( cd libproject; make distclean )
	@echo

install:
ifdef PREFIX
	( cd libproject; make install )
	@echo
endif

