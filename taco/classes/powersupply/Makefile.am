noinst_HEADERS 		= PowerSupply.h AGPowerSupply.h

INCLUDES		= @TACO_INCLUDES@

LIBADD 			= @TACO_SERVER_CXX_LIBS@ -lstdc++

noinst_LTLIBRARIES 	= libPowersupply.la

libPowersupply_la_SOURCES = PowerSupply.cpp

AGPSpp_LDADD		= @TACO_SERVER_CXX_LIBS@ libPowersupply.la 

bin_PROGRAMS 		= AGPSpp ps_menu

AGPSpp_SOURCES 		= AGPowerSupply.cpp startup.cpp

ps_menu_SOURCES 	= ps_menu.cpp

ps_menu_LDADD		= @TACO_CLIENT_CXX_LIBS@ libPowersupply.la 
