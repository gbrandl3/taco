SUBDIRS		=

EXTRA_DIST	= doc bitmaps doc/ug 

SRC 		= xdev_bxutil.c xdev_creation.c xdev_gui.c xdevmenu.uil\
  		xdev_cb.c xdev_dev.c xdev_gui.h\
  		XDevmenu xdev_const.h xdev_dev.h xdev_main.c	
		
bin_PROGRAMS	= xdevmenu

appdefault_DATA	= XDevmenu XDevmenu_bw

EXTRA_DIST	+= ${appdefault_DATA} xdevmenu.uil

AM_CFLAGS	= -DXK_MISCELLANY -D_BSD_SOURCE @X_CFLAGS@

INCLUDES	= @MOTIF_INCLUDES@ @TACO_INCLUDES@ 

LDADD		= @TACO_CLIENT_LIBS@ @MOTIF_LIBS@ @X_LIBS@ @X_PRE_LIBS@ @X_EXTRA_LIBS@ @LIB_X11@ -lXt -ldl @TACO_ASCII_LIBS@ 

xdevmenu_SOURCES= $(SRC)

