ifdef TCAMAKE_PROJECT
TOPDIR = ../../..
else
TOPDIR = ..
endif
NEED_PTHREADS = 1
NEED_TCANETPP = 1
NEED_TCAXMLPP = 1
NEED_TCASQLPP = 1
NEED_LIBXML2 = 1

ifdef USE_POSTGRESQL
DB_OBJS =       TestPgsqlSession.o
endif

ifdef USE_MYSQL
NEED_MYSQL = 1
DB_OBJS =       TestMysqlSession.o
endif

ifdef USE_POSTGRESQL
NEED_LIBPQXX = 1
DB_OBS = 	TestPgsqlSession.o
endif

ifdef TCANMS_DEBUG
OPT_FLAGS =     -g
endif

INCLUDES =	
LIBS = 

BIN =		sqlptest
OBJS = 		$(DB_OBJS) TestSqlThread.o sqlplustest.o

ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: test


include $(TOPDIR)/tcamake/project_defs


test: sqlptest

sqlptest: $(OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(OBJS) \
	*.d *.D *.bd *.o lib/*.bd

distclean: clean
	$(RM) $(BIN) *.log

install:
