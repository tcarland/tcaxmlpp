TOPDIR = ../..

NEED_TNMSCORE = 1
NEED_ZLIB = 1
NEED_LIBXML2 = 1
NEED_GSOAP = 1
NEED_PTHREADS = 1

OPT_FLAGS = 	-g

CFLAGS =
LIBS = 
INCLUDES = 	-Isoap

BIN = 		tnmsauth_test

GSOAP_OBJS =	soap/soapC.o $(GSOAP_DIR)/stdsoap2.o soap/soapServer.o soap/soapClient.o
SOAP_OBJS =	soap_cl_impl.o

OBJS =		tnmsauth_test.o

ALL_OBJS = 	$(GSOAP_OBJS) $(SOAP_OBJS) $(OBJS)
ALL_BINS = 	$(BIN)


all: soap tnmsauth_test
soap: gsoap

include $(TOPDIR)/tcamake/project_defs

gsoap:
	@if [ ! -d soap ]; then $(MKDIR) soap; \
	$(GSOAP_BIN)/soapcpp2 -dsoap -I$(GSOAP_DIR) ../include/soap_definitions.h; fi
	@echo

gsoap-clean:
	$(RM) soap
	$(RM) $(GSOAP_DIR)/*.d

tnmsauth_test: $(ALL_OBJS)
	$(make-cxxbin-rule)
	@echo

clean:
	$(RM) $(ALL_OBJS) \
	*.d *.D *.bd *.o

distclean: clean gsoap-clean
	$(RM) $(ALL_BINS)

install:
