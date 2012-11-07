TOPDIR = ..

NEED_TCASQLPP = 1
NEED_TNMSCORE = 1
NEED_TNMSSESSION = 1
NEED_PTHREADS = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_OPENSSL = 1
NEED_GSOAP = 1

ifdef USE_LIBRT
NEED_LIBRT = 1
endif
ifdef USE_MYSQL
NEED_MYSQL = 1
endif

ifdef TNMS_DEBUG
OPT_FLAGS= 	-g
endif

LIBS=		
INCLUDES=       -Iinclude -Isoap 

BIN=		tnmsauthd

GSOAP_OBJS =	soap/soapC.o $(GSOAP_DIR)/stdsoap2.o soap/soapServer.o soap/soapClient.o
SOAP_OBJS =     src/soap_implementation.o

OBJS=		src/TnmsAuthManager.o  \
		src/SoapIOHandler.o src/SoapClient.o src/SoapThread.o \
		src/AuthdIOHandler.o src/AuthdClient.o \
		src/AuthDbThread.o src/tnmsauthd.o

ALL_OBJS=	$(GSOAP_OBJS) $(SOAP_OBJS) $(OBJS)
ALL_BINS=	$(BIN)


all: soap tnmsauthd tools
soap: gsoap

include $(TOPDIR)/tcamake/project_defs

gsoap:
	@if [ ! -d soap ]; then $(MKDIR) soap; \
	$(GSOAP_BIN)/soapcpp2 -dsoap -I$(GSOAP_DIR) include/soap_definitions.h; fi
	@echo

gsoap-clean:
	$(RM) $(GSOAP_DIR)/*.d
	$(RM) soap

tnmsauthd: $(ALL_OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	( cd test; make clean )
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

.PHONY: test
test:
	( cd test; make soap; make )
	@echo

test-clean:
	( cd test; make distclean )
	@echo

.PHONY: tools
tools:
	( cd tools; make all )
	@echo

tools-clean:
	( cd tools; make distclean )
	@echo

distclean: clean gsoap-clean test-clean tools-clean
	$(RM) $(ALL_BINS)
	@echo

install:
ifdef TNMS_PREFIX
	( cd tools; make install )
	$(CP) $(BIN) $(TNMS_PREFIX)/sbin
	@echo
endif

