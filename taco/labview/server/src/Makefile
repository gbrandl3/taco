DS_HOME  = $(DSHOME)
LIB_HOME = $(DS_HOME)/lib/linux/x86
LV_HOME  = /usr/local/lv70/

LV_DEBUG = -DLV_DEBUG
LV_DC = -DLV_DC
CFLAGS = $(LV_DEBUG) $(LV_DC) -DLABVIEW -DNOMAIN -I$(DS_HOME)/include -g \
	-I$(DS_HOME)/include/private -I../include \
	-I$(LV_HOME)/cintools

all:  libtaco.so lv_dsapi.so lv_dsclass.so 


lv_dsapi.so : lv_dsapi.o 
	cc -g -shared -Wl,-soname,lv_dsapi.so -o lv_dsapi.so \
	lv_dsapi.o -L. -ltaco

lv_dsclass.so : lv_dsclass.o
	cc -g -shared -Wl,-soname,lv_dsclass.so -o lv_dsclass.so \
	lv_dsclass.o -L. -ltaco

libtaco.so : $(LIB_HOME)/libdsapi.so $(LIB_HOME)/libdbapi.so \
	$(LIB_HOME)/libdsxdr.so $(LIB_HOME)/libdcapi.so 
	cc -g -shared -Wl,-soname,libtaco.so -o libtaco.so \
	$(LIB_HOME)/libdsapi.so $(LIB_HOME)/libdbapi.so \
	$(LIB_HOME)/libdsxdr.so $(LIB_HOME)/libdcapi.so

install : all
	cp lv_dsapi.so $(LIB_HOME)
	cp lv_dsclass.so $(LIB_HOME)
	cp libtaco.so $(LIB_HOME)
	cp lv_dsapi.so ../lib
	cp lv_dsclass.so ../lib
	cp libtaco.so ../lib

clean :
	rm -f *.o
	
clobber : clean
	rm -f lv_dsapi.so
	rm -f lv_dsclass.so
	rm -f libtaco.so
