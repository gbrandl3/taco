# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2005 Jens Krüger <jkrueger1@users.sf.net>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

EXTRA_DIST		= taco.startup.in taco.shutdown.in taco-config.in tacoenv.sh.in tacoenv.csh.in taco.SuSE.in taco-manager.sh.in \
			  taco.debian.in taco.status.in taco-server.pc.in taco-server++.pc.in taco-client.pc.in taco-client++.pc.in

dist_sbin_SCRIPTS 	= off

nodist_sbin_SCRIPTS 	= taco-config taco.startup taco.shutdown taco.status

nodist_template_SCRIPTS = taco.SuSE taco.debian taco-manager.sh

nodist_template_DATA 	= tacoenv.sh tacoenv.csh

sysconf_DATA		= taco_log.cfg

pkgconfigdir		= @libdir@/pkgconfig

nodist_pkgconfig_DATA	= taco-server.pc taco-server++.pc taco-client.pc taco-client++.pc

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
	rm -f $(DESTDIR)/$(sysconfdir)/tacoenv.sh $(DESTDIR)/$(sysconfdir)/tacoenv.csh 

CLEANFILES		= tacoenv.tmp 


