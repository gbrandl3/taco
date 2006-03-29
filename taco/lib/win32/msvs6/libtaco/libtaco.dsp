# Microsoft Developer Studio Project File - Name="libtaco" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtaco - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtaco.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtaco.mak" CFG="libtaco - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtaco - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtaco - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtaco - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtaco - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "D:\oncrpc\win32\include" /I "D:\workspace\taco\lib" /I "D:\workspace\taco\lib\private" /I "D:\workspace\taco\lib\xdr" /I "D:\workspace\taco\lib\dbase" /I "D:\workspace\taco\lib\dc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_NT" /D "CLIENT_UDP" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libtaco - Win32 Release"
# Name "libtaco - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\xdr\api_xdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\api_xdr_vers3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\asyn_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\browse_cli.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dev_api.c

!IF  "$(CFG)" == "libtaco - Win32 Release"

!ELSEIF  "$(CFG)" == "libtaco - Win32 Debug"

# ADD CPP /I "D:\workspace\taco\lib" /I "D:\workspace\taco\lib\private" /I "D:\workspace\taco\lib\xdr" /I "D:\workspace\taco\lib\dbase"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\dev_cmds.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dev_errors.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dev_states.c
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\dev_xdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\DevServer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\DevServerSig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\DevXdrKernel.c
# End Source File
# Begin Source File

SOURCE=..\..\..\ds_prog_vers3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\event_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\event_relisten.c
# End Source File
# Begin Source File

SOURCE=..\..\..\gen_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\gettrans.c
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\maxe_xdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NT_debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\oic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\reconnect_cli.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sec_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\setacc_cli.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\setacc_clnt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\setacc_xdr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\startup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\svc_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\tools_cli.c
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\update_cli.c
# End Source File
# Begin Source File

SOURCE=..\..\..\util_api.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\Admin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\API.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\API_xdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\API_xdr_vers3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\BlcDsNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CntrlDsNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DasDsNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\db_setup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\dbase\db_xdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\dev_xdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevCmds.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevErrors.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Device.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeviceBase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DeviceFRM.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevSec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevServer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevServerP.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\DevServerXdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevSignal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DevStates.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\DevXdrKernel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\DserverTeams.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Frm2DsNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\hdb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\hdb_out.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MachDsNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\macros.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\maxe_xdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NT_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OICDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OICDeviceFRM.h
# End Source File
# Begin Source File

SOURCE=..\..\..\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Starter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SysNumbers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\xdr\xdr_typelist.h
# End Source File
# End Group
# End Target
# End Project
