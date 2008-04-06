
sbin_PROGRAMS = testcs

noinst_HEADERS = CSHost.h DeviceServer.h EsrfControlSystem.h KernelServer.h testcs.h

testcs_SOURCES = testcs.cpp cshost.cpp esrfcontrolsystem.cpp kernelserver.cpp deviceserver.cpp 
testcs_LDADD   = @TACO_SERVER_CXX_LIBS@


AM_CPPFLAGS	=  @TACO_INCLUDES@
