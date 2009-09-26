TOPDIR=..


all:

include $(TOPDIR)/tcamake/project_defs


install:
ifdef TCANMS_PREFIX
	@echo "Initializing install to '$(TCANMS_PREFIX)'"
	sh scripts/tcanms_install.sh
	$(CP) scripts/init* $(TCANMS_PREFIX)/bin
	$(CP) scripts/tcanmsrc $(TCANMS_PREFIX)/etc
	@echo
endif


targetclean:
	$(RM) $(TCANMS_PREFIX)

clean:

distclean:

