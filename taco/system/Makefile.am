if XDEVMENUBUILD
 xdev_dir=xdevmenu
else
 xdev_dir=
endif

if ALARMBUILD
 alarm_dir=alarm
else
 alarm_dir=
endif

if DC_BUILD
  dc_dir=dc
else
  dc_dir=
endif

if BUILD_SERVER
starter=starter
else
starter=
endif

if BUILD_MANAGER
manager=manager
else
manager=
endif

if BUILD_MESSAGESERVER
messageserver=msg
else
messageserver=
endif

if BUILD_DATABASESERVER
databaseserver=dbase
else
databaseserver=
endif

SUBDIRS = . $(manager) $(messageserver) $(dc_dir) $(databaseserver) $(starter) $(xdev_dir) $(alarm_dir) $(dbdev_dir)

