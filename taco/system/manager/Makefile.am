noinst_HEADERS		= ManagerP.h
sbin_PROGRAMS		= Manager unreg

INCLUDES		= @TACO_INCLUDES@

Manager_SOURCES		= Manager.c ManagerMain.c
Manager_LDADD		= @TACO_SERVER_LIBS@ $(top_builddir)/lib/libtaco.la 

unreg_SOURCES		= unreg.c
