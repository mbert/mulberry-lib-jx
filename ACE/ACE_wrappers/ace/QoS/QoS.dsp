# Microsoft Developer Studio Project File - Name="QoS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=QoS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE run the tool that generated this project file and specify the
!MESSAGE nmake output type.  You can then use the following command:
!MESSAGE
!MESSAGE NMAKE.
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE CFG="QoS - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "QoS - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QoS - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QoS - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Debug\QoS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Ob0 /W3 /Gm /GX /Zi /MDd /GR /Gy /Fd"Debug\QoS/" /I "..\.." /D _DEBUG /D WIN32 /D _WINDOWS /D ACE_HAS_WINSOCK2_GQOS /D ACE_QoS_BUILD_DLL /FD /c
# SUBTRACT CPP /Fr /YX

# ADD MTL /D "_DEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d _DEBUG /d ACE_HAS_WINSOCK2_GQOS /i "..\.."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\..\lib\ACE_QoS.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACEd.lib /libpath:"." /libpath:"..\..\lib" /nologo /subsystem:windows /pdb:"..\..\lib\ACE_QoSd.pdb" /implib:"..\..\lib\ACE_QoSd.lib" /dll /debug /machine:I386 /out:"..\..\lib\ACE_QoSd.dll"

!ELSEIF  "$(CFG)" == "QoS - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Release\QoS"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /O2 /W3 /GX /MD /GR /I "..\.." /D NDEBUG /D WIN32 /D _WINDOWS /D ACE_HAS_WINSOCK2_GQOS /D ACE_QoS_BUILD_DLL /FD /c
# SUBTRACT CPP /YX

# ADD MTL /D "NDEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d NDEBUG /d ACE_HAS_WINSOCK2_GQOS /i "..\.."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\..\lib\ACE_QoS.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO ACE.lib /libpath:"." /libpath:"..\..\lib" /nologo /subsystem:windows /pdb:none /implib:"..\..\lib\ACE_QoS.lib" /dll  /machine:I386 /out:"..\..\lib\ACE_QoS.dll"

!ENDIF

# Begin Target

# Name "QoS - Win32 Debug"
# Name "QoS - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;cxx;c"
# Begin Source File

SOURCE="QoS_Decorator.cpp"
# End Source File
# Begin Source File

SOURCE="QoS_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="QoS_Session_Factory.cpp"
# End Source File
# Begin Source File

SOURCE="QoS_Session_Impl.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast_QoS.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hh"
# Begin Source File

SOURCE="ACE_QoS_Export.h"
# End Source File
# Begin Source File

SOURCE="QoS_Decorator.h"
# End Source File
# Begin Source File

SOURCE="QoS_Manager.h"
# End Source File
# Begin Source File

SOURCE="QoS_Session.h"
# End Source File
# Begin Source File

SOURCE="QoS_Session_Factory.h"
# End Source File
# Begin Source File

SOURCE="QoS_Session_Impl.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast_QoS.h"
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "i;inl"
# Begin Source File

SOURCE="QoS_Session_Impl.i"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast_QoS.i"
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE="README"
# End Source File
# End Group
# Begin Group "Pkgconfig Files"

# PROP Default_Filter "in"
# Begin Source File

SOURCE="ACE_QoS.pc.in"
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
