# Microsoft Developer Studio Project File - Name="libqq" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libqq - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libqq.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libqq.mak" CFG="libqq - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libqq - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libqq - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libqq - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QQLIB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "libQQLIB" /D "__WIN32__" /D _WIN32_WINNT=0x0500 /FD /TP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ws2_32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/libqq.dll"

!ELSEIF  "$(CFG)" == "libqq - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QQLIB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "libQQLIB" /D "__WIN32__" /D _WIN32_WINNT=0x0500 /FD /GZ /TP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../bin/libqq.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "libqq - Win32 Release"
# Name "libqq - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\buddy.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\commplatform.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\config.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\crc32.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\debug.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\group.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\libqq.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\list.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\loop.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\md5.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\memory.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\myqq_private.rc
# End Source File
# Begin Source File

SOURCE=..\packetmgr.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_buddy.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_group.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_im.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_login.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_misc.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_qun.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\prot_user.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\protocol.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qqclient.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qqconn.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qqcrypt.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qqpacket.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qqsocket.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\qun.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\utf8.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\util.c
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Source File

SOURCE=.\libqq.def
# End Source File
# End Target
# End Project
