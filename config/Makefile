TOPDIR=..


all:

include $(TOPDIR)/tcamake/builddefs


install:
ifdef TCANMS_PREFIX
	@echo "Initializing install path of '$(TCANMS_PREFIX)'"
	scripts/tcanms_install.sh
	@echo
endif

targetclean:
	$(RM) $(TCANMS_PREFIX)

clean:

distclean:

