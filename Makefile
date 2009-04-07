# ------------------------
#  Top-level Makefile
#

include ./tcamake/projectdefs

# ------------------------

all: print-env external common tnmsd


print-env:
	@gcc -v
	@uname -a

.PHONY: common
common:
	( cd common; make all )
	@echo

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

