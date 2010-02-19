TOPDIR=..


all:

include $(TOPDIR)/tcamake/project_defs


install:
ifdef TNMS_PREFIX
	@echo "Initializing install to '$(TNMS_PREFIX)'"
	sh scripts/tnms_install.sh --force
	$(CP) scripts/tnms.sh $(TNMS_PREFIX)/bin
	$(CP) scripts/tnms_functions.sh $(TNMS_PREFIX)/bin
	$(CP) scripts/tnms_dbinit.sh $(TNMS_PREFIX)/bin
	$(CP) scripts/mysql* $(TNMS_PREFIX)/bin
	$(CP) scripts/tnmsrc $(TNMS_PREFIX)/etc
	@echo
endif


targetclean:
	$(RM) $(TNMS_PREFIX)

clean:

distclean:

