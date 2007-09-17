TOPDIR = ../../../..

NEED_ZLIB = 1

INCLUDES=-I..

ifdef DEBUG
OPT_FLAGS =	-g
endif


BIN =		zipstream_test


OBJS =          zip_stream_test.o main.o


ALL_OBJS =	$(OBJS)
ALL_BINS = 	$(BIN)


all: zipstream_test


include ${TOPDIR}/common/make/soe



zipstream_test: $(OBJS)
	$(make-cxxbin-rule)
	@echo

funzip: funzip_test.o
	$(make-cxxbin-rule)
	@echo
	

clean:
	$(RM) *.o *.d *.gz *.zip

distclean: clean
	$(RM) $(BIN)

