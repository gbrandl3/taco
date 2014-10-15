SUBDIRS = src include tests 

if DOC
SUBDIRS += doc 
endif

## DIST_SUBDIRS = src include doc tests

bin_SCRIPTS = log4cpp-config

m4datadir = $(datadir)/aclocal
m4data_DATA = log4cpp.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = log4cpp.pc

EXTRA_DIST = \
	autogen.sh \
	log4cpp.spec.in log4cpp.spec log4cpp.m4 \
	log4cpp.pc.in log4cpp-config.in \
	m4/ac_config_libconfig_in.m4 \
	m4/ac_config_pkgconfig_in.m4 \
	m4/AC_CREATE_PREFIX_CONFIG_H.m4 \
	m4/BB_ENABLE_DOXYGEN.m4 \
	m4/AC_CXX_HAVE_SSTREAM.m4 \
	m4/AC_CXX_NAMESPACES.m4 \
	m4/PETI_PEDANTIC_GCC.m4 \
	m4/AC_FUNC_SNPRINTF.m4 \
	m4/AC_C_INT64_T.m4 \
	m4/BB_CHECK_OMNITHREADS.m4 \
	m4/BB_CHECK_PTHREADS.m4

.PHONY: check rpm docs doc-dist

DISTCLEANFILES = log4cpp.pc log4cpp-config

dist-hook:
#	perl -pi -e 's/\n/\r\n/g' `find $(distdir) -name '*.ds?'`
#	perl -pi -e 's/\n/\r\n/g' `find $(distdir) -name '*.bpg'`

distclean-local:
	$(RM) CONTENTS TAGS log4cpp-config

CONTENTS::
	find $(top_srcdir) \( ! -type l -a -name '*.cpp' -o -name '*.hh' -o -name Makefile.am \) > $@

TAGS:: CONTENTS
	etags `cat $<`

      convenience.h \
rpm: dist
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz
	mv -f /usr/src/redhat/SRPMS/$(PACKAGE)-$(VERSION)-*.rpm $(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-$(VERSION)-*.rpm $(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-devel-$(VERSION)-*.rpm \
		$(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-doc-$(VERSION)-*.rpm \
		$(top_srcdir)

docs:
	$(MAKE) -C doc

doc-dist: docs
	tar -zcf $(PACKAGE)-docs-$(VERSION).tar.gz -C $(top_srcdir)/doc/html --exclude CVS .
 
