EXTRA_DIST		= taco.startup.in taco.shutdown.in taco-config.in tacoenv.sh.in tacoenv.csh.in taco.in taco-manager.sh.in \
			  taco.debian.in

dist_sbin_SCRIPTS 	= off

nodist_sbin_SCRIPTS 	= taco-config taco.startup taco.shutdown 

nodist_template_SCRIPTS = taco taco.debian taco-manager.sh

nodist_template_DATA 	= tacoenv.sh tacoenv.csh

install-data-hook:	$(nodist_template_DATA)
	$(mkinstalldirs) $(DESTDIR)${sysconfdir}
	if test -f $(DESTDIR)${sysconfdir}/tacoenv.sh ; then \
		@AWK@ '/^### USER SETUP START/ {exit}; {print $$0;}' tacoenv.sh >tacoenv.tmp ;\
		@AWK@ '/^### USER SETUP START/,/^### USER SETUP END/ {print $$0}' $(DESTDIR)${sysconfdir}/tacoenv.sh >> tacoenv.tmp ;\
		@AWK@ '/^#!/,/^### USER SETUP END/ {next}; {print $$0}' tacoenv.sh >> tacoenv.tmp ;\
		@INSTALL_DATA@ tacoenv.tmp $(DESTDIR)${sysconfdir}/tacoenv.sh ; \
	else \
		@INSTALL_DATA@ tacoenv.sh $(DESTDIR)${sysconfdir} ;\
	fi
	if test -f $(DESTDIR)${sysconfdir}/tacoenv.csh ; then \
		@AWK@ '/^### USER SETUP START/ {exit}; {print $$0;}' tacoenv.csh >tacoenv.tmp ;\
		@AWK@ '/^### USER SETUP START/,/^### USER SETUP END/ {print $$0}' $(DESTDIR)${sysconfdir}/tacoenv.csh >> tacoenv.tmp ;\
		@AWK@ '/^#!/,/^### USER SETUP END/ {next}; {print $$0}' tacoenv.csh >> tacoenv.tmp ;\
		@INSTALL_DATA@ tacoenv.tmp $(DESTDIR)${sysconfdir}/tacoenv.csh ; \
	else \
		@INSTALL_DATA@ tacoenv.csh $(DESTDIR)${sysconfdir} ;\
	fi

uninstall-local:
	if { basename ${prefix} | grep inst ; } ; then rm -f $(DESTDIR)/$(sysconfdir)/tacoenv.sh $(DESTDIR)/$(sysconfdir)/tacoenv.csh ; fi

CLEANFILES		= tacoenv.tmp 


