#!/bin/csh

setenv DBTABLES_STANDARD NAMES,PS_NAMES,CLASS,SYS,ERROR,CMDS,SEC,EVENTS
# add your DOAMIN tables here
setenv  DBTABLES_DOMAIN TEST
setenv DBTABLES ${DBTABLES_STANDARD},${DBTABLES_DOMAIN}


setenv DSHOME /usr/local/dshome2
setenv RES_BASE_DIR ${DSHOME}/dbase/res
setenv DBM_DIR ${DSHOME}/dbase/dbm

if (${?LD_LIBRARY_PATH}) then
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${DSHOME}/lib/linux/x86
else
setenv LD_LIBRARY_PATH ${DSHOME}/lib/linux/x86
endif

if (${?PYTHONPATH}) then
setenv PYTHONPATH ${PYTHONPATH}:${DSHOME}/lib/linux/x86:$DSHOME/python
else
setenv PYTHONPATH ${DSHOME}/lib/linux/x86:$DSHOME/python
endif
if (${?NETHOST}) then
	;
# echo "Nethost= " $NETHOST
else
setenv NETHOST localhost
endif
setenv PATH ${PATH}:${DSHOME}/system/bin/linux/x86
setenv PATH ${PATH}:${DSHOME}/bin/linux/x86
