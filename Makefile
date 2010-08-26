# ------------------------
#  Top-level Makefile
#

export USE_PTHREADS=1


include ./tcamake/build_defs

# ------------------------

all: print-env external common TnmsApi tnmsd tnmsauthd \
	archiver tnms-console wxClient


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

tnmsauthd:
	( cd TnmsAuthd; make soap )
	( cd TnmsAuthd; make tnmsauthd )
	@echo

archiver:
	( cd clients/TnmsArchiver; make all )
	@echo 

tnms-console:
	( cd clients/tnms-console; make all )
	@echo

wxClient:
	( cd clients/wxClient; make all )
	@echo

external:
	( cd common/ext; ./build_extlibs.sh )
	@echo

external-clean:
	( cd common/ext; make distclean )
	@echo

clean:
	( cd common; make clean )
	( cd TnmsApi; make clean )
	( cd tnmsd; make clean )
	( cd TnmsAuthd; make clean )
	( cd clients/TnmsArchiver; make clean )
	( cd clients/tnms-console; make clean )
	( cd clients/wxClient; make clean )

distclean: external-clean
	( cd common; make distclean )
	( cd TnmsApi; make distclean )
	( cd tnmsd; make distclean )
	( cd TnmsAuthd; make distclean )
	( cd clients/TnmsArchiver; make distclean )
	( cd clients/tnms-console; make distclean )
	( cd clients/wxClient; make distclean )

install:
ifdef TNMS_PREFIX
	( cd config; make install )
	( cd common; make install )
	( cd TnmsApi; make install )
	( cd tnmsd; make install )
	( cd TnmsAuthd; make install )
	( cd clients/TnmsArchiver; make install )
	( cd clients/tnms-console; make install )
	( cd clients/wxClient; make install )
endif

