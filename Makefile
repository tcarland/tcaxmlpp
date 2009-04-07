# ------------------------
#  Top-level Makefile
#
include ./tcamake/projectdefs

# ------------------------

all: print-env external common tnmsd


print-env:
	@uname -a
	@echo
	@gcc -v
	@echo

.PHONY: common
common:
	( cd common; make all )
	@echo

.PHONY: tnmsd
tnmsd:
	( cd tnmsd; make all )
	@echo

external:
	( cd common/ext; ./build.sh )
	@echo

external-clean:
	( cd common/ext; make distclean )
	@echo

clean:
	( cd common; make clean )
	( cd tnmsd; make clean )

distclean: external-clean
	( cd common; make distclean )
	( cd tnmsd; make distclean )

install:
ifdef PREFIX
	( cd common; make install )
	( cd tnmsd; make install )
endif

