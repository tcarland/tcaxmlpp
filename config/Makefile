TOPDIR=..


all:

include $(TOPDIR)/tcamake/builddefs


install:
ifdef TCANMS_PREFIX
	@echo "Initializing install path of '$(TCANMS_PREFIX)'"
	sh scripts/tcanms_install.sh
	$(CP) scripts/init* $(TCANMS_PREFIX)/sbin
	$(CP) tcanmsrc $(TCANMS_PREFIX)/etc
	@echo
endif

targetclean:
	$(RM) $(TCANMS_PREFIX)

clean:

distclean:

