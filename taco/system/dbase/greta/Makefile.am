noinst_HEADERS		= greta.h  greta_cstes.h

SRC_GRETA		= greta.c greta_creation.c bxutil-c.c greta_cb.c greta_xs.c \
		greta_misc.c greta_sdd.c greta_win.c servutil.c devutil.c \
		greta_update.c

EXTRA_PROGRAMS		= greta greta_ndbm
bin_PROGRAMS		= @GRETA@

appdefault_DATA		= GReta

EXTRA_DIST		= GReta greta.uil
INCLUDES		= @MOTIF_INCLUDES@ @TACO_INCLUDES@ -I$(srcdir)/../include -I$(srcdir)/../../include

greta_SOURCES		= $(SRC_GRETA)
greta_CPPFLAGS		= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@
greta_LDADD		= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ -lX11 -lXt  @X_EXTRA_LIBS@ @TACO_DC_LIBS@

greta_ndbm_SOURCES 	= $(SRC_GRETA)
greta_ndbm_CPPFLAGS  	= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@ -DNDBM_USED 
greta_ndbm_LDADD   	= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ -lX11 -lXt  @X_EXTRA_LIBS@ @TACO_DC_LIBS@
