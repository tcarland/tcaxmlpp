TOPDIR = ../..

NEED_PTHREADS = 1
NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_GSOAP = 1

OPT_FLAGS = 	-g

CFLAGS =
LIBS = 
INCLUDES = 

BIN = 		tnmsauth_test

GSOAP_OBJS =	soapC.o $(GSOAP_DIR)/stdsoap2.o soapServer.o soapClient.o
SOAP_OBJS =	soap_cl_impl.o

OBJS =		tnmsauth_test.o

ALL_OBJS = 	$(GSOAP_OBJS) $(SOAP_OBJS) $(OBJS)
ALL_BINS = 	$(BIN)


all: soap tnmsauth_test
soap: gsoap

include $(TOPDIR)/tcamake/project_defs

gsoap:
	$(GSOAP_DIR)/bin/soapcpp2 -I$(GSOAP_DIR) ../include/soap_definitions.h
	@echo

gsoap-clean:
	$(RM) soapC.* soapServer.* soapServerLib.* soapClient.* \
	soapClientLib.* soapStub.h soapH.h $(GSOAP_DIR)/*.d
	$(RM) *req.xml *res.xml *.wsdl *.xsd *.nsmap
	$(RM) soapTnmsAuth*

tnmsauth_test: $(ALL_OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o

distclean: clean gsoap-clean
	$(RM) $(ALL_BINS)

install:
