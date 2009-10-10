ifdef TCAMAKE_PROJECT
TOPDIR = ../..
else
TOPDIR = .
endif


NEED_PTHREADS = 1
NEED_LIBDL = 1
NEED_TCANETPP = 1

ifdef TCANMS_DEBUG
OPT_FLAGS =	-g
endif

CFLAGS =        
INCLUDES =	-Iinclude 
LIBS =          

BIN =		lib/libtnmsSession.a

OBJS = 		src/CommandLineAuthenticator.o \
		src/RandomStringDevice.o \
		src/TicketDatabase.o


ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)

all:  libtnmsSession


include $(TOPDIR)/tcamake/project_defs

lib: arlib

arlib: lib/libtnmsSession.a
solib: lib/libtnmsSession.so.0.1.1

libtnmsSession: lib/libtnmsSession.a

lib/libtnmsSession.a: $(OBJS)
	( $(MKDIR) lib )
	$(make-lib-rule)
	@echo

lib/libtnmsSession.so.0.1.1: $(OBJS)
	( $(MKDIR) lib )
	( $(RM) $@ lib/libtnmsSession.so
	$(make-so-rule)
	( cd lib; ln -s $@ libtnmsSession.so

lib-clean:
	( $(RM) lib )

clean: 
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd src/*.o src/*.d lib/*.bd

distclean: clean lib-clean
	$(RM) $(BIN)

install:
ifdef TCANMS_PREFIX
	#( $(MKDIR) $(SOME_PREFIX)/include/tnmsSession )
	#( cp include/* $(SOME_PREFIX)/include/tnmsSession )
	#( cp lib/libtnmsSession* $(SOME_PREFIX)/lib )
	@echo
endif

