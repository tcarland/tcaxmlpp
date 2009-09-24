TOPDIR = ..

NEED_TCASQLPP = 1
NEED_TNMSCORE = 1
NEED_TNMSSESSION = 1
NEED_PTHREADS = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_GSOAP = 1

ifdef USE_MYSQL
NEED_MYSQL = 1
endif

OPT_FLAGS= 	-g

LIBS=
INCLUDES=       -Iinclude -I.

BIN=		tnmsauthd

GSOAP_OBJS =	soapC.o $(GSOAP_DIR)/stdsoap2.o soapServer.o soapClient.o
SOAP_OBJS =     src/soap_implementation.o

OBJS=		src/TnmsAuthdManager.o  \
		src/SoapIOHandler.o src/SoapClient.o src/SoapThread.o \
		src/AuthIOHandler.o src/AuthdClient.o \
		src/AuthDbThread.o \
		src/tnmsauthd.o

ALL_OBJS=	$(GSOAP_OBJS) $(SOAP_OBJS) $(OBJS)
ALL_BINS=	$(BIN)


all: soap tnmsauthd
soap: gsoap

include $(TOPDIR)/tcamake/project_defs

gsoap:
	$(GSOAP_DIR)/bin/soapcpp2 -I$(GSOAP_DIR) include/soap_definitions.h
	@echo

gsoap-clean:
	$(RM) soapC.* soapServer.* soapServerLib.* soapClient.* \
	soapClientLib.* soapStub.h soapH.h $(GSOAP_DIR)/*.d
	$(RM) *req.xml *res.xml *.wsdl *.xsd *.nsmap
	$(RM) soapTnmsAuth*

tnmsauthd: $(ALL_OBJS)
	$(make-cxxbin-rule)

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.o src/*.d src/*.D src/*.bd src/*.o

distclean: clean gsoap-clean
	$(RM) $(ALL_BINS)

install:
ifdef TCANMS_PREFIX
	$(CP) $(BIN) $(TCANMS_PREFIX)/bin
	@echo
endif

