noinst_HEADERS 		= PowerSupply.h AGPowerSupply.h

INCLUDES		= @TACO_INCLUDES@

LIBADD 			= @TACO_SERVER_LIBS@

noinst_LTLIBRARIES 	= libPowersupply.la

libPowersupply_la_SOURCES = PowerSupply.cpp

LDADD			= @TACO_SERVER_LIBS@ libPowersupply.la -ldsapig++

bin_PROGRAMS 		= AGPSpp ps_menu

AGPSpp_SOURCES 		= AGPowerSupply.cpp startup.cpp

ps_menu_SOURCES 	= ps_menu.cpp

