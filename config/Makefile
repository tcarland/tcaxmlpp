TOPDIR=..


all:

include $(TOPDIR)/tcamake/project_defs


install:
ifdef TNMS_PREFIX
	@echo "Initializing install to '$(TNMS_PREFIX)'"
	sh scripts/tnms_install.sh --force
	@echo
endif


targetclean:
	$(RM) $(TNMS_PREFIX)/tnms

clean:

distclean:

