# ------------------------
#  Top-level Makefile
#
include ./tcamake/projectdefs

# ------------------------

all: print-env external common TnmsApi tnmsd tnms-console


print-env:
	@uname -a
	@echo
	@gcc -v
	@echo

.PHONY: common
common:
	( cd common; make all )
	@echo

.PHONY: TnmsApi
TnmsApi:
	( cd TnmsApi; make all )
	@echo

.PHONY: tnmsd
tnmsd:
	( cd tnmsd; make all )
	@echo

tnms-console:
	( cd clients/tnms-console; make all )
	@echo

external:
	( cd common/ext; ./build.sh )
	@echo

external-clean:
	( cd common/ext; make distclean )
	@echo

clean:
	( cd common; make clean )
	( cd TnmsApi; make clean )
	( cd tnmsd; make clean )
	( cd clients/tnms-console; make clean )

distclean: external-clean
	( cd common; make distclean )
	( cd TnmsApi; make distclean )
	( cd tnmsd; make distclean )
	( cd clients/tnms-console; make distclean )

install:
ifdef PREFIX
	( cd common; make install )
	( cd TnmsApi; make install )
	( cd tnmsd; make install )
endif

