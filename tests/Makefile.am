# Toolkit for building distributed control systems or any other distributed system.
#
# Copyright (c) 2003-2014 Jens Kr�ger <jkrueger1@users.sf.net>
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

SUBDIRS			= ## testserver

EXTRA_DIST 		= taco.in t1.py t2.py t3.py t4.py t5.py t6.py

CLEANFILES 		= log taco

DISTCLEANFILES 		= System.log atconfig *.lineno package.m4 testsuite

AM_CPPFLAGS		= @TACO_INCLUDES@ @LOG4CPP_CFLAGS@ @LOG4TACO_CFLAGS@

## @GDBM_CFLAGS@ 

AM_CFLAGS		= -g -O0

test_asc_stat_SOURCES 	= test_asc_stat.c
test_asc_stat_LDADD 	= @TACO_CLIENT_LIBS@ @TACO_ASCII_LIBS@ 

test_asc_dyn_LDADD 	= @TACO_CLIENT_LIBS@ 
test_asc_dyn_SOURCES 	= test_asc_dyn.c

test_tc_SOURCES 	= test_tc.c
test_tc_LDADD		= @TACO_CLIENT_LIBS@ 

EXTRA_DIST 		+= testsuite.at $(TESTSUITE_AT) testsuite 

# local.at ext.at

TESTSUITE 		= testsuite

TESTSUITE_AT		= dbase.at devices.at starter.at message.at specialtest1.at local.at ext.at gdbm.at
TESTSUITE_AT		+= python.at

AUTOTEST 		= $(AUTOM4TE) --language=autotest

check_PROGRAMS 		= db_connect db_resget db_resput db_getdevexp \
			db_resdomain db_resfamily db_resmember \
			db_getresoval db_devdomain db_devfamily db_devmember \
			db_devlist db_devperslist dev_export dev_import \
			db_serverlist db_hostlist db_dsonhost \
			test_asc_dyn test_asc_stat test_tc test_starter \
			dev_cmdquery dev_conparams \
			test_server test_client test_client2 test_status test_connect \
			longest_testserver \
			taco_hostname \
                        db_analyze \
			db_getres \
			db_reconnect

check_SCRIPT		= taco

TESTS_ENVIRONMENT 	= 

db_connect_SOURCES 	= db_connect.cpp
db_connect_LDADD 	= @TACO_CLIENT_LIBS@ @GDBM_LIBS@ -lstdc++

db_resget_SOURCES 	= db_resget.cpp 
db_resget_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_resput_SOURCES 	= db_resput.cpp 
db_resput_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_resdomain_SOURCES 	= db_resdomain.cpp 
db_resdomain_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_devdomain_SOURCES 	= db_devdomain.cpp 
db_devdomain_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

dev_export_SOURCES 	= db_devexp.cpp 
dev_export_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_resfamily_SOURCES 	= db_resfamily.cpp 
db_resfamily_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_devlist_SOURCES 	= db_devlist.cpp 
db_devlist_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_serverlist_SOURCES 	= db_serverlist.cpp 
db_serverlist_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_hostlist_SOURCES 	= db_hostlist.cpp 
db_hostlist_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_dsonhost_SOURCES 	= db_dsonhost.cpp 
db_dsonhost_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_devperslist_SOURCES 	= db_devperslist.cpp 
db_devperslist_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_devfamily_SOURCES 	= db_devfamily.cpp 
db_devfamily_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

dev_import_SOURCES 	= test_dev_import.cpp 
dev_import_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_resmember_SOURCES 	= db_resmember.cpp 
db_resmember_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_devmember_SOURCES 	= db_devmember.cpp 
db_devmember_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

test_starter_SOURCES	= test_start.cpp
test_starter_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

dev_cmdquery_SOURCES	= test_cmd_query.cpp
dev_cmdquery_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

dev_conparams_SOURCES	= test_con_params.cpp
dev_conparams_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_getresoval_SOURCES	= db_getresoval.cpp
db_getresoval_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_getdevexp_SOURCES	= db_getdevexp.cpp
db_getdevexp_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_analyze_SOURCES      = db_analyze.cpp
db_analyze_LDADD        = @TACO_CLIENT_CXX_LIBS@ -lstdc++

db_getres_SOURCES	= db_getres.cpp
db_getres_CPPFLAGS	= @TACO_INCLUDES@ -I$(top_srcdir)/ext/include
db_getres_LDADD		= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la

db_reconnect_SOURCES	= db_reconnect.cpp
db_reconnect_CPPFLAGS	= @TACO_INCLUDES@ -I$(top_srcdir)/ext/include
db_reconnect_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la

package.m4:	$(top_srcdir)/configure.ac $(srcdir)/Makefile.am
	{\
		echo '# Signature of the current package.'; \
		echo 'm4_define([AT_PACKAGE_NAME], 	[@PACKAGE_NAME@])'; \
		echo 'm4_define([AT_PACKAGE_TARNAME],	[@PACKAGE_TARNAME@])'; \
		echo 'm4_define([AT_PACKAGE_VERSION],	[@PACKAGE_VERSION@])'; \
		echo 'm4_define([AT_PACKAGE_STRING],	[@PACKAGE_STRING@])'; \
		echo 'm4_define([AT_PACKAGE_BUGREPORT],	[@PACKAGE_BUGREPORT@])'; \
		echo 'm4_define([AT_PACKAGE_URL],	[@PACKAGE_URL@])'; \
	} > package.m4

check-local:	atconfig atlocal $(TESTSUITE) taco
	$(SHELL) $(TESTSUITE)

$(TESTSUITE):	$(srcdir)/testsuite.at $(TESTSUITE_AT) package.m4
	$(AUTOTEST) -I $(srcdir) $(srcdir)/testsuite.at -o $@.tmp
	sed  -e's/5>&1//g' $@.tmp > $@
	chmod +x $@
	rm -f $@.tmp	

atconfig:	$(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@

clean-local:
	if test -f $(TESTSUITE) ; then $(SHELL) $(TESTSUITE) --clean ; fi	

noinst_HEADERS		= test_server.h test_serverP.h TestExtServer.h

test_server_SOURCES 	= test_server.cpp
test_server_LDADD 	= @TACO_SERVER_CXX_LIBS@ -lstdc++
test_server_LDFLAGS 	= -no-install

test_client_SOURCES 	= test_client.cpp
test_client_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

test_client2_SOURCES 	= test_client2.cpp
test_client2_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

test_status_SOURCES 	= test_status.cpp
test_status_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

test_connect_SOURCES 	= test_connect.cpp
test_connect_LDADD 	= @TACO_CLIENT_CXX_LIBS@ -lstdc++

longest_testserver_SOURCES = test_server.cpp
longest_testserver_LDADD = @TACO_SERVER_CXX_LIBS@ -lstdc++

if BUILD_EXT
check_PROGRAMS		+= TestExtServer ExtServerClient TestExtClient TestExtClient2 TestExtClient3

TestExtServer_CPPFLAGS  = -I$(top_srcdir)/ext/include ${AM_CPPFLAGS} -DTACO_EXT
TestExtServer_SOURCES	= TestExtServer.cpp TestExtServerStartup.cpp
TestExtServer_LDADD	= @TACO_SERVER_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la
TestExtServer_LDFLAGS	= -no-install

ExtServerClient_CPPFLAGS= -I$(top_srcdir)/ext/include ${AM_CPPFLAGS} -DTACO_EXT
ExtServerClient_SOURCES = ExtServerClient.cpp
ExtServerClient_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la
ExtServerClient_LDFLAGS = -no-install

TestExtClient_SOURCES 	= TestExtClient.cpp
TestExtClient_CPPFLAGS  = -I$(top_srcdir)/ext/include ${AM_CPPFLAGS} -DTACO_EXT
TestExtClient_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la
TestExtClient_LDFLAGS	= -no-install

TestExtClient2_SOURCES 	= TestExtClient2.cpp
TestExtClient2_CPPFLAGS = -I$(top_srcdir)/ext/include ${AM_CPPFLAGS} -DTACO_EXT
TestExtClient2_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la
TestExtClient2_LDFLAGS	= -no-install

TestExtClient3_SOURCES 	= TestExtClient3.cpp
TestExtClient3_CPPFLAGS = -I$(top_srcdir)/ext/include ${AM_CPPFLAGS} -DTACO_EXT
TestExtClient3_LDADD	= @TACO_CLIENT_CXX_LIBS@ -lstdc++ $(top_builddir)/ext/src/libTACOExtensions.la
TestExtClient3_LDFLAGS	= -no-install

endif

taco_hostname_SOURCES	= taco_hostname.c
taco_hostname_LDADD	= @TACO_CLIENT_LIBS@
