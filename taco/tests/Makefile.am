SUBDIRS			= ## testserver

EXTRA_DIST 		= taco.in 

CLEANFILES 		= log taco

DISTCLEANFILES 		= System.log atconfig

INCLUDES		= @TACO_INCLUDES@ @TACO_GDBM_INC@ -g -O0

test_asc_stat_SOURCES 	= test_asc_stat.c
test_asc_stat_LDADD 	= @TACO_CLIENT_LIBS@ @TACO_ASCII_LIBS@ 

test_asc_dyn_LDADD 	= @TACO_CLIENT_LIBS@ 
test_asc_dyn_SOURCES 	= test_asc_dyn.c

test_tc_SOURCES 	= test_tc.c
test_tc_LDADD		= @TACO_CLIENT_LIBS@ 

EXTRA_DIST 		+= testsuite.at $(TESTSUITE_AT) testsuite package.m4

TESTSUITE 		= $(srcdir)/testsuite

TESTSUITE_AT		= dbase.at devices.at starter.at

AUTOTEST 		= $(AUTOM4TE) --language=autotest

check_PROGRAMS 		= db_connect db_resget db_resput \
			db_resdomain db_resfamily db_resmember dev_ping \
			test_asc_dyn test_asc_stat test_tc test_starter \
			test_cmd_query

check_SCRIPT		= taco

TESTS_ENVIRONMENT 	= 

db_connect_SOURCES 	= db_connect.cpp
db_connect_LDADD 	= @TACO_CLIENT_LIBS@ @TACO_GDBM_LDFLAGS@ @TACO_GDBM_LIBS@

db_resget_SOURCES 	= db_resget.cpp 
db_resget_LDADD 	= @TACO_CLIENT_LIBS@

db_resput_SOURCES 	= db_resput.cpp 
db_resput_LDADD 	= @TACO_CLIENT_LIBS@

db_resdomain_SOURCES 	= db_resdomain.cpp 
db_resdomain_LDADD 	= @TACO_CLIENT_LIBS@

db_resfamily_SOURCES 	= db_resfamily.cpp 
db_resfamily_LDADD 	= @TACO_CLIENT_LIBS@

db_resmember_SOURCES 	= db_resmember.cpp 
db_resmember_LDADD 	= @TACO_CLIENT_LIBS@

dev_ping_SOURCES 	= dev_ping.c
dev_ping_LDADD 		= @TACO_CLIENT_LIBS@ 

test_starter_SOURCES	= test_start.cpp
test_starter_LDADD	= @TACO_CLIENT_LIBS@

test_cmd_query_SOURCES	= test_cmd_query.cpp
test_cmd_query_LDFLAGS	= -g 
test_cmd_query_LDADD	= @TACO_CLIENT_LIBS@

$(srcdir)/package.m4:	$(top_srcdir)/configure.ac
	{\
		echo '# Signature of the current package.'; \
		echo 'm4_define([AT_PACKAGE_NAME], 	[@PACKAGE_NAME@])'; \
		echo 'm4_define([AT_PACKAGE_TARNAME],	[@PACKAGE_TARNAME@])'; \
		echo 'm4_define([AT_PACKAGE_VERSION],	[@PACKAGE_VERSION@])'; \
		echo 'm4_define([AT_PACKAGE_STRING],	[@PACKAGE_STRING@])'; \
		echo 'm4_define([AT_PACKAGE_BUGREPORT],	[@PACKAGE_BUGREPORT@])'; \
	} > $(srcdir)/package.m4

check-local:	atconfig atlocal $(TESTSUITE) taco
	$(SHELL) $(TESTSUITE)

$(TESTSUITE):	$(srcdir)/testsuite.at $(TESTSUITE_AT) $(srcdir)/package.m4
	$(AUTOTEST) -I $(srcdir) $@.at -o $@.tmp
	mv $@.tmp $@

atconfig:	$(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

clean-local:
	$(SHELL) $(TESTSUITE) --clean

