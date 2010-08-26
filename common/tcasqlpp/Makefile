ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif

ifdef USE_POSTGRESQL
NEED_LIBPQXX = 1
DB_OBJS =       src/PgsqlSession.o
endif

ifdef USE_MYSQL
NEED_MYSQL = 1
DB_OBJS =       src/MysqlSession.o
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


lib: arlib

arlib: lib/libtcasqlpp.a
solib: libtcasqlpp.so.0.1.1


lib/libtcasqlpp.a: $(OBJS)
	@echo $(OBJS)
	( mkdir -p lib )
	ld -r -o $@ $(OBJS) $(LFLAGS)
	@echo

libtcasqlpp.so.0.1.1: $(OBJS)
	$(make-so-rule)
	$(RM) lib/$@ lib/libtcasqlsqlpp.so
	( mkdir -p lib; mv $@ lib/; \
	  cd lib; ln -s $@ libtcasqlpp.so )
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
endif
