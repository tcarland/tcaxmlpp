#  Makefile for tcasqlpp
#
ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif

NEED_TCANETPP_INCLUDE = 1

ifdef USE_POSTGRESQL
NEED_LIBPQXX = 1
DB_OBJS =       src/PgsqlSession.o
endif

ifdef USE_MYSQL
NEED_MYSQL = 1
DB_OBJS =       src/MysqlSession.o
endif

ifndef DB_OBJS
$(warning 'Required environment variable not found, check the README for details')
$(error 'ERROR database type not found. eg. export USE_MYSQL=1')
endif

ifdef TNMS_DEBUG
OPT_FLAGS =     -g
endif


OPT_FLAGS +=	-fPIC -O2
CCSHARED += 	-Wl,-soname,$@


INCLUDES =	-Iinclude -I$(TOPDIR)/common/tcanetpp/include
LIBS = 

BIN =		
OBJS = 		src/SqlDbPool.o
OBJS +=         $(DB_OBJS)

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: lib

include $(TOPDIR)/tcamake/project_defs


lib: arlib solib

arlib: lib/libtcasqlpp.a
solib: libtcasqlpp.so.1.1.19


lib/libtcasqlpp.a: $(OBJS)
	@echo $(OBJS)
	( mkdir -p lib )
	$(make-lib-rule)
	@echo

libtcasqlpp.so.1.1.19: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtcasqlpp.so )
	$(make-so-rule)
	( mv $@ lib/; cd lib; ln -s $@ libtcasqlpp.so )
	@echo

.PHONY: test
test:
	(cd test; $(MAKE) all)

test-clean:
	(cd test; $(MAKE) distclean)
	
documentation:
	(cd docs; $(MAKE) $(MFLAGS) $(MVARS) all )
	@echo

libclean:
	rm -rf lib
	@echo

doc-clean:
	(cd docs; $(MAKE) clean )
	@echo

clean:
	$(RM) $(OBJS) \
	src/*.d src/*.D *.bd src/*.o lib/*.bd

distclean: clean libclean test-clean
	$(RM) $(BIN)

install:
ifdef TNMS_PREFIX
	@echo
	$(MKDIR) $(TNMS_PREFIX)/include/tcasqlpp
	$(MKDIR) $(TNMS_PREFIX)/lib
	$(RSYNC) --delete include/ $(TNMS_PREFIX)/include/tcasqlpp/
	$(RSYNC) lib/ $(TNMS_PREFIX)/lib/

endif
