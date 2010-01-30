TOPDIR=..


all:

include $(TOPDIR)/tcamake/project_defs


install:
ifdef TCANMS_PREFIX
	@echo "Initializing install to '$(TCANMS_PREFIX)'"
	sh scripts/tcanms_install.sh --force
	$(CP) scripts/tcanms.sh $(TCANMS_PREFIX)/bin
	$(CP) scripts/tcanms_functions.sh $(TCANMS_PREFIX)/bin
	$(CP) scripts/tcanms_dbinit.sh $(TCANMS_PREFIX)/bin
	$(CP) scripts/mysql* $(TCANMS_PREFIX)/bin
	$(CP) scripts/tcanmsrc $(TCANMS_PREFIX)/etc
	@echo
endif


targetclean:
	$(RM) $(TCANMS_PREFIX)

clean:

distclean:

