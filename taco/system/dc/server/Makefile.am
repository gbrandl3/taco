sbin_PROGRAMS	=dcwr_alo dcrd_alo

AM_CPPFLAGS	= -DALONE
INCLUDES	=-I$(top_srcdir)/dbase/include @TACO_INCLUDES@
LIBDC		=../library/libdcmem.la

dcwr_alo_SOURCES=dc_devall.c \
		dc_devinf.c \
		dc_inf.c \
		dc_put.c \
		dc_serv.c \
		dcwr_svc.c \
		dcwr_xdr.c 

dcwr_alo_LDADD	= @TACO_SERVER_LIBS@ $(LIBDC)

dcrd_alo_SOURCES=dc_devall.c \
		dc_devinf.c \
		dc_inf.c \
		dc_read.c \
		dcrd_svc.c \
		dcrd_xdr.c 

dcrd_alo_LDADD	= @TACO_SERVER_LIBS@ $(LIBDC)
