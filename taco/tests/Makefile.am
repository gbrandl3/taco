TESTS 		= taco-install \
		check-dbm \
		check-dbupdate \
		check-dbservinfo \
		tst_load_db \
		tst_load_db_fail \
		tst_info_db \
		tst_read_db \
		tst_resget_db \
		tst_resput_db \
		tst_resdel_db \
		tst_resdomain_db \
		tst_resfamily_db \
		tst_resmember_db \
		check-inst-verify \
		tst_info_dev1 \
		tst_info_devx \
		taco-uninstall

XFAIL_TESTS 	= tst_info_devx tst_load_db_fail 

##  check-agps 

EXTRA_DIST 	= taco.in $(TESTS)
EXTRA_DIST 	+= tst_info_db tst_info_dev1 tst_info_devx tst_load_db tst_load_db_fail tst_read_db \
		tst_resdel_db tst_resdomain_db tst_resfamily_db tst_resget_db tst_resmember_db tst_resput_db

EXTRA_DIST 	+= fail.res test.res

nodist_noinst_DATA = taco-stamp

taco-stamp:	taco
	chmod +x taco
	touch taco-stamp

CLEANFILES = taco-stamp log taco

DISTCLEANFILES 	= System.log

check_PROGRAMS	= db_connect \
		db_resget \
		db_resput \
		db_resdomain \
		db_resfamily \
		db_resmember test_asc_dyn test_asc_stat test_tc

TESTS_ENVIRONMENT = 

INCLUDES	= @TACO_INCLUDES@ @TACO_GDBM_INC@ 

db_connect_SOURCES	= db_connect.cpp
db_connect_LDADD	= @TACO_CLIENT_LIBS@ @TACO_GDBM@

db_resget_SOURCES	= db_resget.cpp 
db_resget_LDADD		= @TACO_CLIENT_LIBS@

db_resput_SOURCES	= db_resput.cpp 
db_resput_LDADD		= @TACO_CLIENT_LIBS@

db_resdomain_SOURCES 	= db_resdomain.cpp 
db_resdomain_LDADD	= @TACO_CLIENT_LIBS@

db_resfamily_SOURCES 	= db_resfamily.cpp 
db_resfamily_LDADD	= @TACO_CLIENT_LIBS@

db_resmember_SOURCES 	= db_resmember.cpp 
db_resmember_LDADD	= @TACO_CLIENT_LIBS@

test_asc_stat_SOURCES 	= test_asc_stat.c
test_asc_stat_LDADD 	= @TACO_CLIENT_LIBS@ @TACO_ASCII_LIBS@ -ldl

test_asc_dyn_LDADD 	= @TACO_CLIENT_LIBS@ -ldl 
test_asc_dyn_SOURCES 	=	test_asc_dyn.c

test_tc_SOURCES 	=  test_tc.c
test_tc_LDADD		= @TACO_CLIENT_LIBS@ -ldl

