# Microsoft Developer Studio Project File - Name="ACE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ACE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE run the tool that generated this project file and specify the
!MESSAGE nmake output type.  You can then use the following command:
!MESSAGE
!MESSAGE NMAKE.
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE CFG="ACE - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "ACE - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ACE - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ACE - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug\ACE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /Ob0 /W3 /Gm /GX /Zi /MDd /GR /Gy /Fd"Debug\ACE/" /I ".." /D _DEBUG /D WIN32 /D _WINDOWS /D ACE_HAS_ACE_TOKEN /D ACE_HAS_ACE_SVCCONF /D ACE_BUILD_DLL /FD /c
# SUBTRACT CPP /Fr /YX

# ADD MTL /D "_DEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d _DEBUG /d ACE_HAS_ACE_TOKEN /d ACE_HAS_ACE_SVCCONF /i ".."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\lib\ACE.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO /libpath:"." /libpath:"..\lib" /nologo /subsystem:windows /pdb:"..\lib\ACEd.pdb" /implib:"..\lib\ACEd.lib" /dll /debug /machine:I386 /out:"..\lib\ACEd.dll"

!ELSEIF  "$(CFG)" == "ACE - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release\ACE"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /O2 /W3 /GX /MD /GR /I ".." /D NDEBUG /D WIN32 /D _WINDOWS /D ACE_HAS_ACE_TOKEN /D ACE_HAS_ACE_SVCCONF /D ACE_BUILD_DLL /FD /c
# SUBTRACT CPP /YX

# ADD MTL /D "NDEBUG" /nologo /mktyplib203 /win32
# ADD RSC /l 0x409 /d NDEBUG /d ACE_HAS_ACE_TOKEN /d ACE_HAS_ACE_SVCCONF /i ".."
BSC32=bscmake.exe
# ADD BSC32 /nologo /o"..\lib\ACE.bsc"
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO /libpath:"." /libpath:"..\lib" /nologo /subsystem:windows /pdb:none /implib:"..\lib\ACE.lib" /dll  /machine:I386 /out:"..\lib\ACE.dll"

!ENDIF

# Begin Target

# Name "ACE - Win32 Debug"
# Name "ACE - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;cxx;c"
# Begin Source File

SOURCE="ACE.cpp"
# End Source File
# Begin Source File

SOURCE="ACE_crc32.cpp"
# End Source File
# Begin Source File

SOURCE="ACE_crc_ccitt.cpp"
# End Source File
# Begin Source File

SOURCE="Activation_Queue.cpp"
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Addr.cpp"
# End Source File
# Begin Source File

SOURCE="Arg_Shifter.cpp"
# End Source File
# Begin Source File

SOURCE="ARGV.cpp"
# End Source File
# Begin Source File

SOURCE="Argv_Type_Converter.cpp"
# End Source File
# Begin Source File

SOURCE="Asynch_IO.cpp"
# End Source File
# Begin Source File

SOURCE="Asynch_IO_Impl.cpp"
# End Source File
# Begin Source File

SOURCE="Asynch_Pseudo_Task.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_Params.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_QoS.cpp"
# End Source File
# Begin Source File

SOURCE="ATM_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="Atomic_Op.cpp"
# End Source File
# Begin Source File

SOURCE="Auto_Event.cpp"
# End Source File
# Begin Source File

SOURCE="Barrier.cpp"
# End Source File
# Begin Source File

SOURCE="Base_Thread_Adapter.cpp"
# End Source File
# Begin Source File

SOURCE="Based_Pointer_Repository.cpp"
# End Source File
# Begin Source File

SOURCE="Basic_Stats.cpp"
# End Source File
# Begin Source File

SOURCE="Basic_Types.cpp"
# End Source File
# Begin Source File

SOURCE="Capabilities.cpp"
# End Source File
# Begin Source File

SOURCE="CDR_Base.cpp"
# End Source File
# Begin Source File

SOURCE="CDR_Size.cpp"
# End Source File
# Begin Source File

SOURCE="CDR_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="Cleanup.cpp"
# End Source File
# Begin Source File

SOURCE="Codecs.cpp"
# End Source File
# Begin Source File

SOURCE="Codeset_IBM1047.cpp"
# End Source File
# Begin Source File

SOURCE="Codeset_Registry.cpp"
# End Source File
# Begin Source File

SOURCE="Codeset_Registry_db.cpp"
# End Source File
# Begin Source File

SOURCE="Condition_Recursive_Thread_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Condition_Thread_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Configuration.cpp"
# End Source File
# Begin Source File

SOURCE="Configuration_Import_Export.cpp"
# End Source File
# Begin Source File

SOURCE="Connection_Recycling_Strategy.cpp"
# End Source File
# Begin Source File

SOURCE="Container_Instantiations.cpp"
# End Source File
# Begin Source File

SOURCE="Containers.cpp"
# End Source File
# Begin Source File

SOURCE="Copy_Disabled.cpp"
# End Source File
# Begin Source File

SOURCE="Countdown_Time.cpp"
# End Source File
# Begin Source File

SOURCE="Date_Time.cpp"
# End Source File
# Begin Source File

SOURCE="DEV.cpp"
# End Source File
# Begin Source File

SOURCE="DEV_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="DEV_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="DEV_IO.cpp"
# End Source File
# Begin Source File

SOURCE="Dev_Poll_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Dirent.cpp"
# End Source File
# Begin Source File

SOURCE="Dirent_Selector.cpp"
# End Source File
# Begin Source File

SOURCE="DLL.cpp"
# End Source File
# Begin Source File

SOURCE="DLL_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Dump.cpp"
# End Source File
# Begin Source File

SOURCE="Dynamic.cpp"
# End Source File
# Begin Source File

SOURCE="Dynamic_Service_Base.cpp"
# End Source File
# Begin Source File

SOURCE="Event.cpp"
# End Source File
# Begin Source File

SOURCE="Event_Handler.cpp"
# End Source File
# Begin Source File

SOURCE="FIFO.cpp"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv.cpp"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv_Msg.cpp"
# End Source File
# Begin Source File

SOURCE="FIFO_Send.cpp"
# End Source File
# Begin Source File

SOURCE="FIFO_Send_Msg.cpp"
# End Source File
# Begin Source File

SOURCE="FILE.cpp"
# End Source File
# Begin Source File

SOURCE="FILE_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="FILE_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="FILE_IO.cpp"
# End Source File
# Begin Source File

SOURCE="File_Lock.cpp"
# End Source File
# Begin Source File

SOURCE="Filecache.cpp"
# End Source File
# Begin Source File

SOURCE="Flag_Manip.cpp"
# End Source File
# Begin Source File

SOURCE="Framework_Component.cpp"
# End Source File
# Begin Source File

SOURCE="Functor.cpp"
# End Source File
# Begin Source File

SOURCE="Functor_String.cpp"
# End Source File
# Begin Source File

SOURCE="Get_Opt.cpp"
# End Source File
# Begin Source File

SOURCE="gethrtime.cpp"
# End Source File
# Begin Source File

SOURCE="Handle_Ops.cpp"
# End Source File
# Begin Source File

SOURCE="Handle_Set.cpp"
# End Source File
# Begin Source File

SOURCE="Hash_Map_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Hashable.cpp"
# End Source File
# Begin Source File

SOURCE="High_Res_Timer.cpp"
# End Source File
# Begin Source File

SOURCE="ICMP_Socket.cpp"
# End Source File
# Begin Source File

SOURCE="INET_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="Init_ACE.cpp"
# End Source File
# Begin Source File

SOURCE="IO_Cntl_Msg.cpp"
# End Source File
# Begin Source File

SOURCE="IO_SAP.cpp"
# End Source File
# Begin Source File

SOURCE="IOStream.cpp"
# End Source File
# Begin Source File

SOURCE="IPC_SAP.cpp"
# End Source File
# Begin Source File

SOURCE="Lib_Find.cpp"
# End Source File
# Begin Source File

SOURCE="Local_Memory_Pool.cpp"
# End Source File
# Begin Source File

SOURCE="Local_Name_Space.cpp"
# End Source File
# Begin Source File

SOURCE="Local_Tokens.cpp"
# End Source File
# Begin Source File

SOURCE="Lock.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg_Backend.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg_Callback.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg_IPC.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg_NT_Event_Log.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Msg_UNIX_Syslog.cpp"
# End Source File
# Begin Source File

SOURCE="Log_Record.cpp"
# End Source File
# Begin Source File

SOURCE="Logging_Strategy.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK_CODgram.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK_Dgram.cpp"
# End Source File
# Begin Source File

SOURCE="LSOCK_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="Malloc.cpp"
# End Source File
# Begin Source File

SOURCE="Malloc_Allocator.cpp"
# End Source File
# Begin Source File

SOURCE="Malloc_Instantiations.cpp"
# End Source File
# Begin Source File

SOURCE="Manual_Event.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_IO.cpp"
# End Source File
# Begin Source File

SOURCE="Mem_Map.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_SAP.cpp"
# End Source File
# Begin Source File

SOURCE="MEM_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="Message_Block.cpp"
# End Source File
# Begin Source File

SOURCE="Message_Queue.cpp"
# End Source File
# Begin Source File

SOURCE="Method_Request.cpp"
# End Source File
# Begin Source File

SOURCE="Metrics_Cache.cpp"
# End Source File
# Begin Source File

SOURCE="MMAP_Memory_Pool.cpp"
# End Source File
# Begin Source File

SOURCE="Msg_WFMO_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Multihomed_INET_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Name_Proxy.cpp"
# End Source File
# Begin Source File

SOURCE="Name_Request_Reply.cpp"
# End Source File
# Begin Source File

SOURCE="Name_Space.cpp"
# End Source File
# Begin Source File

SOURCE="Naming_Context.cpp"
# End Source File
# Begin Source File

SOURCE="Notification_Strategy.cpp"
# End Source File
# Begin Source File

SOURCE="NT_Service.cpp"
# End Source File
# Begin Source File

SOURCE="Obchunk.cpp"
# End Source File
# Begin Source File

SOURCE="Object_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Object_Manager_Base.cpp"
# End Source File
# Begin Source File

SOURCE="Obstack.cpp"
# End Source File
# Begin Source File

SOURCE="OS_Errno.cpp"
# End Source File
# Begin Source File

SOURCE="OS_Log_Msg_Attributes.cpp"
# End Source File
# Begin Source File

SOURCE="OS_main.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_arpa_inet.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_ctype.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_dirent.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_dlfcn.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_errno.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_fcntl.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_math.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_netdb.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_poll.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_pwd.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_regex.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_signal.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdio.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdlib.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_string.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_strings.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_stropts.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_mman.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_msg.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_resource.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_select.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_shm.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_socket.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_stat.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_time.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_uio.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_utsname.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_wait.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_Thread.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_time.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_unistd.cpp"
# End Source File
# Begin Source File

SOURCE="OS_NS_wchar.cpp"
# End Source File
# Begin Source File

SOURCE="OS_QoS.cpp"
# End Source File
# Begin Source File

SOURCE="OS_Thread_Adapter.cpp"
# End Source File
# Begin Source File

SOURCE="OS_TLI.cpp"
# End Source File
# Begin Source File

SOURCE="Pagefile_Memory_Pool.cpp"
# End Source File
# Begin Source File

SOURCE="Parse_Node.cpp"
# End Source File
# Begin Source File

SOURCE="PI_Malloc.cpp"
# End Source File
# Begin Source File

SOURCE="Ping_Socket.cpp"
# End Source File
# Begin Source File

SOURCE="Pipe.cpp"
# End Source File
# Begin Source File

SOURCE="POSIX_Asynch_IO.cpp"
# End Source File
# Begin Source File

SOURCE="POSIX_CB_Proactor.cpp"
# End Source File
# Begin Source File

SOURCE="POSIX_Proactor.cpp"
# End Source File
# Begin Source File

SOURCE="Priority_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Proactor.cpp"
# End Source File
# Begin Source File

SOURCE="Proactor_Impl.cpp"
# End Source File
# Begin Source File

SOURCE="Process.cpp"
# End Source File
# Begin Source File

SOURCE="Process_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Process_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Process_Semaphore.cpp"
# End Source File
# Begin Source File

SOURCE="Profile_Timer.cpp"
# End Source File
# Begin Source File

SOURCE="Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Reactor_Impl.cpp"
# End Source File
# Begin Source File

SOURCE="Reactor_Notification_Strategy.cpp"
# End Source File
# Begin Source File

SOURCE="Reactor_Timer_Interface.cpp"
# End Source File
# Begin Source File

SOURCE="Read_Buffer.cpp"
# End Source File
# Begin Source File

SOURCE="Recursive_Thread_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Recyclable.cpp"
# End Source File
# Begin Source File

SOURCE="Refcountable.cpp"
# End Source File
# Begin Source File

SOURCE="Registry.cpp"
# End Source File
# Begin Source File

SOURCE="Registry_Name_Space.cpp"
# End Source File
# Begin Source File

SOURCE="Remote_Name_Space.cpp"
# End Source File
# Begin Source File

SOURCE="Remote_Tokens.cpp"
# End Source File
# Begin Source File

SOURCE="RW_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="RW_Process_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="RW_Thread_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Sample_History.cpp"
# End Source File
# Begin Source File

SOURCE="Sbrk_Memory_Pool.cpp"
# End Source File
# Begin Source File

SOURCE="Sched_Params.cpp"
# End Source File
# Begin Source File

SOURCE="Select_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Select_Reactor_Base.cpp"
# End Source File
# Begin Source File

SOURCE="Semaphore.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Config.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Object.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Repository.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Templates.cpp"
# End Source File
# Begin Source File

SOURCE="Service_Types.cpp"
# End Source File
# Begin Source File

SOURCE="Shared_Memory.cpp"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_MM.cpp"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_Pool.cpp"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_SV.cpp"
# End Source File
# Begin Source File

SOURCE="Shared_Object.cpp"
# End Source File
# Begin Source File

SOURCE="Signal.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_CODgram.cpp"
# End Source File
# Begin Source File

SOURCE="Sock_Connect.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Bcast.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_IO.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Association.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="SOCK_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="SPIPE.cpp"
# End Source File
# Begin Source File

SOURCE="SPIPE_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="SPIPE_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="SPIPE_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="SPIPE_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="SString.cpp"
# End Source File
# Begin Source File

SOURCE="Stats.cpp"
# End Source File
# Begin Source File

SOURCE="String_Base_Const.cpp"
# End Source File
# Begin Source File

SOURCE="SUN_Proactor.cpp"
# End Source File
# Begin Source File

SOURCE="SV_Message.cpp"
# End Source File
# Begin Source File

SOURCE="SV_Message_Queue.cpp"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Complex.cpp"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Simple.cpp"
# End Source File
# Begin Source File

SOURCE="SV_Shared_Memory.cpp"
# End Source File
# Begin Source File

SOURCE="Svc_Conf_l.cpp"
# End Source File
# Begin Source File

SOURCE="Svc_Conf_Lexer_Guard.cpp"
# End Source File
# Begin Source File

SOURCE="Svc_Conf_y.cpp"
# End Source File
# Begin Source File

SOURCE="Synch_Options.cpp"
# End Source File
# Begin Source File

SOURCE="System_Time.cpp"
# End Source File
# Begin Source File

SOURCE="Task.cpp"
# End Source File
# Begin Source File

SOURCE="Thread.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Adapter.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Control.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Exit.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Hook.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Mutex.cpp"
# End Source File
# Begin Source File

SOURCE="Thread_Semaphore.cpp"
# End Source File
# Begin Source File

SOURCE="Time_Request_Reply.cpp"
# End Source File
# Begin Source File

SOURCE="Time_Value.cpp"
# End Source File
# Begin Source File

SOURCE="Timeprobe.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_Hash.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_Heap.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_List.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_Queue.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_Wheel.cpp"
# End Source File
# Begin Source File

SOURCE="TLI.cpp"
# End Source File
# Begin Source File

SOURCE="TLI_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="TLI_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="TLI_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="Token.cpp"
# End Source File
# Begin Source File

SOURCE="Token_Collection.cpp"
# End Source File
# Begin Source File

SOURCE="Token_Invariants.cpp"
# End Source File
# Begin Source File

SOURCE="Token_Manager.cpp"
# End Source File
# Begin Source File

SOURCE="Token_Request_Reply.cpp"
# End Source File
# Begin Source File

SOURCE="TP_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="Trace.cpp"
# End Source File
# Begin Source File

SOURCE="TSS_Adapter.cpp"
# End Source File
# Begin Source File

SOURCE="TTY_IO.cpp"
# End Source File
# Begin Source File

SOURCE="UNIX_Addr.cpp"
# End Source File
# Begin Source File

SOURCE="UPIPE_Acceptor.cpp"
# End Source File
# Begin Source File

SOURCE="UPIPE_Connector.cpp"
# End Source File
# Begin Source File

SOURCE="UPIPE_Stream.cpp"
# End Source File
# Begin Source File

SOURCE="UUID.cpp"
# End Source File
# Begin Source File

SOURCE="WFMO_Reactor.cpp"
# End Source File
# Begin Source File

SOURCE="WIN32_Asynch_IO.cpp"
# End Source File
# Begin Source File

SOURCE="WIN32_Proactor.cpp"
# End Source File
# Begin Source File

SOURCE="XML_Svc_Conf.cpp"
# End Source File
# Begin Source File

SOURCE="XTI_ATM_Mcast.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hh"
# Begin Source File

SOURCE="Acceptor.h"
# End Source File
# Begin Source File

SOURCE="ACE.h"
# End Source File
# Begin Source File

SOURCE="ACE_export.h"
# End Source File
# Begin Source File

SOURCE="ace_wchar.h"
# End Source File
# Begin Source File

SOURCE="Activation_Queue.h"
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager.h"
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager_T.h"
# End Source File
# Begin Source File

SOURCE="Addr.h"
# End Source File
# Begin Source File

SOURCE="Arg_Shifter.h"
# End Source File
# Begin Source File

SOURCE="ARGV.h"
# End Source File
# Begin Source File

SOURCE="Argv_Type_Converter.h"
# End Source File
# Begin Source File

SOURCE="Array.h"
# End Source File
# Begin Source File

SOURCE="Array_Base.h"
# End Source File
# Begin Source File

SOURCE="Array_Map.h"
# End Source File
# Begin Source File

SOURCE="Asynch_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="Asynch_Connector.h"
# End Source File
# Begin Source File

SOURCE="Asynch_IO.h"
# End Source File
# Begin Source File

SOURCE="Asynch_IO_Impl.h"
# End Source File
# Begin Source File

SOURCE="Asynch_Pseudo_Task.h"
# End Source File
# Begin Source File

SOURCE="ATM_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="ATM_Addr.h"
# End Source File
# Begin Source File

SOURCE="ATM_Connector.h"
# End Source File
# Begin Source File

SOURCE="ATM_Params.h"
# End Source File
# Begin Source File

SOURCE="ATM_QoS.h"
# End Source File
# Begin Source File

SOURCE="ATM_Stream.h"
# End Source File
# Begin Source File

SOURCE="Atomic_Op.h"
# End Source File
# Begin Source File

SOURCE="Atomic_Op_T.h"
# End Source File
# Begin Source File

SOURCE="Auto_Event.h"
# End Source File
# Begin Source File

SOURCE="Auto_Functor.h"
# End Source File
# Begin Source File

SOURCE="Auto_IncDec_T.h"
# End Source File
# Begin Source File

SOURCE="Auto_Ptr.h"
# End Source File
# Begin Source File

SOURCE="Barrier.h"
# End Source File
# Begin Source File

SOURCE="Base_Thread_Adapter.h"
# End Source File
# Begin Source File

SOURCE="Based_Pointer_Repository.h"
# End Source File
# Begin Source File

SOURCE="Based_Pointer_T.h"
# End Source File
# Begin Source File

SOURCE="Basic_Stats.h"
# End Source File
# Begin Source File

SOURCE="Basic_Types.h"
# End Source File
# Begin Source File

SOURCE="Bound_Ptr.h"
# End Source File
# Begin Source File

SOURCE="Cache_Map_Manager_T.h"
# End Source File
# Begin Source File

SOURCE="Cached_Connect_Strategy_T.h"
# End Source File
# Begin Source File

SOURCE="Caching_Strategies_T.h"
# End Source File
# Begin Source File

SOURCE="Caching_Utility_T.h"
# End Source File
# Begin Source File

SOURCE="Capabilities.h"
# End Source File
# Begin Source File

SOURCE="CDR_Base.h"
# End Source File
# Begin Source File

SOURCE="CDR_Size.h"
# End Source File
# Begin Source File

SOURCE="CDR_Stream.h"
# End Source File
# Begin Source File

SOURCE="Cleanup.h"
# End Source File
# Begin Source File

SOURCE="Cleanup_Strategies_T.h"
# End Source File
# Begin Source File

SOURCE="Codecs.h"
# End Source File
# Begin Source File

SOURCE="Codeset_IBM1047.h"
# End Source File
# Begin Source File

SOURCE="Codeset_Registry.h"
# End Source File
# Begin Source File

SOURCE="Condition_Recursive_Thread_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Condition_T.h"
# End Source File
# Begin Source File

SOURCE="Condition_Thread_Mutex.h"
# End Source File
# Begin Source File

SOURCE="config-all.h"
# End Source File
# Begin Source File

SOURCE="config-borland-common.h"
# End Source File
# Begin Source File

SOURCE="config-lite.h"
# End Source File
# Begin Source File

SOURCE="config-minimal.h"
# End Source File
# Begin Source File

SOURCE="config-win32-borland.h"
# End Source File
# Begin Source File

SOURCE="config-win32-common.h"
# End Source File
# Begin Source File

SOURCE="config-win32-ghs.h"
# End Source File
# Begin Source File

SOURCE="config-win32-msvc-6.h"
# End Source File
# Begin Source File

SOURCE="config-win32-msvc-7.h"
# End Source File
# Begin Source File

SOURCE="config-win32-msvc.h"
# End Source File
# Begin Source File

SOURCE="config-win32-visualage.h"
# End Source File
# Begin Source File

SOURCE="config-win32.h"
# End Source File
# Begin Source File

SOURCE="config-WinCE.h"
# End Source File
# Begin Source File

SOURCE="config.h"
# End Source File
# Begin Source File

SOURCE="Configuration.h"
# End Source File
# Begin Source File

SOURCE="Configuration_Import_Export.h"
# End Source File
# Begin Source File

SOURCE="Connection_Recycling_Strategy.h"
# End Source File
# Begin Source File

SOURCE="Connector.h"
# End Source File
# Begin Source File

SOURCE="Containers.h"
# End Source File
# Begin Source File

SOURCE="Containers_T.h"
# End Source File
# Begin Source File

SOURCE="Copy_Disabled.h"
# End Source File
# Begin Source File

SOURCE="CORBA_macros.h"
# End Source File
# Begin Source File

SOURCE="Countdown_Time.h"
# End Source File
# Begin Source File

SOURCE="Date_Time.h"
# End Source File
# Begin Source File

SOURCE="Default_Constants.h"
# End Source File
# Begin Source File

SOURCE="DEV.h"
# End Source File
# Begin Source File

SOURCE="DEV_Addr.h"
# End Source File
# Begin Source File

SOURCE="DEV_Connector.h"
# End Source File
# Begin Source File

SOURCE="DEV_IO.h"
# End Source File
# Begin Source File

SOURCE="Dev_Poll_Reactor.h"
# End Source File
# Begin Source File

SOURCE="Dirent.h"
# End Source File
# Begin Source File

SOURCE="Dirent_Selector.h"
# End Source File
# Begin Source File

SOURCE="DLL.h"
# End Source File
# Begin Source File

SOURCE="DLL_Manager.h"
# End Source File
# Begin Source File

SOURCE="Dump.h"
# End Source File
# Begin Source File

SOURCE="Dump_T.h"
# End Source File
# Begin Source File

SOURCE="Dynamic.h"
# End Source File
# Begin Source File

SOURCE="Dynamic_Service.h"
# End Source File
# Begin Source File

SOURCE="Dynamic_Service_Base.h"
# End Source File
# Begin Source File

SOURCE="Env_Value_T.h"
# End Source File
# Begin Source File

SOURCE="Event.h"
# End Source File
# Begin Source File

SOURCE="Event_Handler.h"
# End Source File
# Begin Source File

SOURCE="Event_Handler_T.h"
# End Source File
# Begin Source File

SOURCE="Exception_Macros.h"
# End Source File
# Begin Source File

SOURCE="FIFO.h"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv.h"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv_Msg.h"
# End Source File
# Begin Source File

SOURCE="FIFO_Send.h"
# End Source File
# Begin Source File

SOURCE="FIFO_Send_Msg.h"
# End Source File
# Begin Source File

SOURCE="FILE.h"
# End Source File
# Begin Source File

SOURCE="FILE_Addr.h"
# End Source File
# Begin Source File

SOURCE="FILE_Connector.h"
# End Source File
# Begin Source File

SOURCE="FILE_IO.h"
# End Source File
# Begin Source File

SOURCE="File_Lock.h"
# End Source File
# Begin Source File

SOURCE="Filecache.h"
# End Source File
# Begin Source File

SOURCE="Flag_Manip.h"
# End Source File
# Begin Source File

SOURCE="Framework_Component.h"
# End Source File
# Begin Source File

SOURCE="Framework_Component_T.h"
# End Source File
# Begin Source File

SOURCE="Free_List.h"
# End Source File
# Begin Source File

SOURCE="Functor.h"
# End Source File
# Begin Source File

SOURCE="Functor_String.h"
# End Source File
# Begin Source File

SOURCE="Functor_T.h"
# End Source File
# Begin Source File

SOURCE="Future.h"
# End Source File
# Begin Source File

SOURCE="Future_Set.h"
# End Source File
# Begin Source File

SOURCE="Get_Opt.h"
# End Source File
# Begin Source File

SOURCE="Global_Macros.h"
# End Source File
# Begin Source File

SOURCE="Guard_T.h"
# End Source File
# Begin Source File

SOURCE="Handle_Gobbler.h"
# End Source File
# Begin Source File

SOURCE="Handle_Ops.h"
# End Source File
# Begin Source File

SOURCE="Handle_Set.h"
# End Source File
# Begin Source File

SOURCE="Hash_Cache_Map_Manager_T.h"
# End Source File
# Begin Source File

SOURCE="Hash_Map_Manager.h"
# End Source File
# Begin Source File

SOURCE="Hash_Map_Manager_T.h"
# End Source File
# Begin Source File

SOURCE="Hash_Map_With_Allocator_T.h"
# End Source File
# Begin Source File

SOURCE="Hashable.h"
# End Source File
# Begin Source File

SOURCE="High_Res_Timer.h"
# End Source File
# Begin Source File

SOURCE="ICMP_Socket.h"
# End Source File
# Begin Source File

SOURCE="If_Then_Else.h"
# End Source File
# Begin Source File

SOURCE="INET_Addr.h"
# End Source File
# Begin Source File

SOURCE="Init_ACE.h"
# End Source File
# Begin Source File

SOURCE="Intrusive_List.h"
# End Source File
# Begin Source File

SOURCE="Intrusive_List_Node.h"
# End Source File
# Begin Source File

SOURCE="IO_Cntl_Msg.h"
# End Source File
# Begin Source File

SOURCE="IO_SAP.h"
# End Source File
# Begin Source File

SOURCE="iosfwd.h"
# End Source File
# Begin Source File

SOURCE="IOStream.h"
# End Source File
# Begin Source File

SOURCE="IOStream_T.h"
# End Source File
# Begin Source File

SOURCE="IPC_SAP.h"
# End Source File
# Begin Source File

SOURCE="Lib_Find.h"
# End Source File
# Begin Source File

SOURCE="Local_Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Local_Name_Space.h"
# End Source File
# Begin Source File

SOURCE="Local_Name_Space_T.h"
# End Source File
# Begin Source File

SOURCE="Local_Tokens.h"
# End Source File
# Begin Source File

SOURCE="Lock.h"
# End Source File
# Begin Source File

SOURCE="Lock_Adapter_T.h"
# End Source File
# Begin Source File

SOURCE="LOCK_SOCK_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg_Backend.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg_Callback.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg_IPC.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg_NT_Event_Log.h"
# End Source File
# Begin Source File

SOURCE="Log_Msg_UNIX_Syslog.h"
# End Source File
# Begin Source File

SOURCE="Log_Priority.h"
# End Source File
# Begin Source File

SOURCE="Log_Record.h"
# End Source File
# Begin Source File

SOURCE="Logging_Strategy.h"
# End Source File
# Begin Source File

SOURCE="LSOCK.h"
# End Source File
# Begin Source File

SOURCE="LSOCK_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="LSOCK_CODgram.h"
# End Source File
# Begin Source File

SOURCE="LSOCK_Connector.h"
# End Source File
# Begin Source File

SOURCE="LSOCK_Dgram.h"
# End Source File
# Begin Source File

SOURCE="LSOCK_Stream.h"
# End Source File
# Begin Source File

SOURCE="Malloc.h"
# End Source File
# Begin Source File

SOURCE="Malloc_Allocator.h"
# End Source File
# Begin Source File

SOURCE="Malloc_Base.h"
# End Source File
# Begin Source File

SOURCE="Malloc_T.h"
# End Source File
# Begin Source File

SOURCE="Managed_Object.h"
# End Source File
# Begin Source File

SOURCE="Manual_Event.h"
# End Source File
# Begin Source File

SOURCE="Map_Manager.h"
# End Source File
# Begin Source File

SOURCE="Map_T.h"
# End Source File
# Begin Source File

SOURCE="MEM_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="MEM_Addr.h"
# End Source File
# Begin Source File

SOURCE="MEM_Connector.h"
# End Source File
# Begin Source File

SOURCE="MEM_IO.h"
# End Source File
# Begin Source File

SOURCE="Mem_Map.h"
# End Source File
# Begin Source File

SOURCE="MEM_SAP.h"
# End Source File
# Begin Source File

SOURCE="MEM_Stream.h"
# End Source File
# Begin Source File

SOURCE="Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Message_Block.h"
# End Source File
# Begin Source File

SOURCE="Message_Block_T.h"
# End Source File
# Begin Source File

SOURCE="Message_Queue.h"
# End Source File
# Begin Source File

SOURCE="Message_Queue_T.h"
# End Source File
# Begin Source File

SOURCE="Method_Object.h"
# End Source File
# Begin Source File

SOURCE="Method_Request.h"
# End Source File
# Begin Source File

SOURCE="Metrics_Cache.h"
# End Source File
# Begin Source File

SOURCE="Min_Max.h"
# End Source File
# Begin Source File

SOURCE="MMAP_Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Module.h"
# End Source File
# Begin Source File

SOURCE="Msg_WFMO_Reactor.h"
# End Source File
# Begin Source File

SOURCE="Multihomed_INET_Addr.h"
# End Source File
# Begin Source File

SOURCE="Mutex.h"
# End Source File
# Begin Source File

SOURCE="Name_Proxy.h"
# End Source File
# Begin Source File

SOURCE="Name_Request_Reply.h"
# End Source File
# Begin Source File

SOURCE="Name_Space.h"
# End Source File
# Begin Source File

SOURCE="Naming_Context.h"
# End Source File
# Begin Source File

SOURCE="Node.h"
# End Source File
# Begin Source File

SOURCE="Notification_Strategy.h"
# End Source File
# Begin Source File

SOURCE="NT_Service.h"
# End Source File
# Begin Source File

SOURCE="Null_Barrier.h"
# End Source File
# Begin Source File

SOURCE="Null_Condition.h"
# End Source File
# Begin Source File

SOURCE="Null_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Null_Semaphore.h"
# End Source File
# Begin Source File

SOURCE="Obchunk.h"
# End Source File
# Begin Source File

SOURCE="Object_Manager.h"
# End Source File
# Begin Source File

SOURCE="Object_Manager_Base.h"
# End Source File
# Begin Source File

SOURCE="Obstack.h"
# End Source File
# Begin Source File

SOURCE="Obstack_T.h"
# End Source File
# Begin Source File

SOURCE="OS.h"
# End Source File
# Begin Source File

SOURCE="OS_Dirent.h"
# End Source File
# Begin Source File

SOURCE="OS_Errno.h"
# End Source File
# Begin Source File

SOURCE="os_include\arpa\os_inet.h"
# End Source File
# Begin Source File

SOURCE="os_include\net\os_if.h"
# End Source File
# Begin Source File

SOURCE="os_include\netinet\os_in.h"
# End Source File
# Begin Source File

SOURCE="os_include\netinet\os_tcp.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_aio.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_assert.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_complex.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_cpio.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_ctype.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_dirent.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_dlfcn.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_errno.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_fcntl.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_fenv.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_float.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_fmtmsg.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_fnmatch.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_ftw.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_glob.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_grp.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_iconv.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_inttypes.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_iso646.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_langinfo.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_libgen.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_limits.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_local.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_math.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_monetary.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_mqueue.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_ndbm.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_netdb.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_nl_types.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_poll.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_pthread.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_pwd.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_regex.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_sched.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_search.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_semaphore.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_setjmp.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_signal.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_spawn.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stdarg.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stdbool.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stddef.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stdint.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stdio.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stdlib.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_string.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_strings.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_stropts.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_syslog.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_tar.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_termios.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_tgmath.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_time.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_trace.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_ucontext.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_ulimit.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_unistd.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_utime.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_utmpx.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_wchar.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_wctype.h"
# End Source File
# Begin Source File

SOURCE="os_include\os_wordexp.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_ipc.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_loadavg.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_mman.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_msg.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_pstat.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_resource.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_select.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_sem.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_shm.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_socket.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_stat.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_statvfs.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_time.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_timeb.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_times.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_types.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_uio.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_un.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_utsname.h"
# End Source File
# Begin Source File

SOURCE="os_include\sys\os_wait.h"
# End Source File
# Begin Source File

SOURCE="OS_Log_Msg_Attributes.h"
# End Source File
# Begin Source File

SOURCE="OS_main.h"
# End Source File
# Begin Source File

SOURCE="OS_Memory.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_arpa_inet.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_ctype.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_dirent.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_dlfcn.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_errno.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_fcntl.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_macros.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_math.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_netdb.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_poll.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_pwd.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_regex.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_signal.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdio.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdlib.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_string.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_strings.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_stropts.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_mman.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_msg.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_resource.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_select.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_shm.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_socket.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_stat.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_time.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_uio.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_utsname.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_wait.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_Thread.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_time.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_unistd.h"
# End Source File
# Begin Source File

SOURCE="OS_NS_wchar.h"
# End Source File
# Begin Source File

SOURCE="OS_QoS.h"
# End Source File
# Begin Source File

SOURCE="OS_String.h"
# End Source File
# Begin Source File

SOURCE="OS_Thread_Adapter.h"
# End Source File
# Begin Source File

SOURCE="OS_TLI.h"
# End Source File
# Begin Source File

SOURCE="Pagefile_Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Pair.h"
# End Source File
# Begin Source File

SOURCE="Pair_T.h"
# End Source File
# Begin Source File

SOURCE="Parse_Node.h"
# End Source File
# Begin Source File

SOURCE="PI_Malloc.h"
# End Source File
# Begin Source File

SOURCE="Ping_Socket.h"
# End Source File
# Begin Source File

SOURCE="Pipe.h"
# End Source File
# Begin Source File

SOURCE="POSIX_Asynch_IO.h"
# End Source File
# Begin Source File

SOURCE="POSIX_CB_Proactor.h"
# End Source File
# Begin Source File

SOURCE="POSIX_Proactor.h"
# End Source File
# Begin Source File

SOURCE="post.h"
# End Source File
# Begin Source File

SOURCE="pre.h"
# End Source File
# Begin Source File

SOURCE="Priority_Reactor.h"
# End Source File
# Begin Source File

SOURCE="Proactor.h"
# End Source File
# Begin Source File

SOURCE="Proactor_Impl.h"
# End Source File
# Begin Source File

SOURCE="Process.h"
# End Source File
# Begin Source File

SOURCE="Process_Manager.h"
# End Source File
# Begin Source File

SOURCE="Process_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Process_Semaphore.h"
# End Source File
# Begin Source File

SOURCE="Profile_Timer.h"
# End Source File
# Begin Source File

SOURCE="RB_Tree.h"
# End Source File
# Begin Source File

SOURCE="Reactor.h"
# End Source File
# Begin Source File

SOURCE="Reactor_Impl.h"
# End Source File
# Begin Source File

SOURCE="Reactor_Notification_Strategy.h"
# End Source File
# Begin Source File

SOURCE="Reactor_Timer_Interface.h"
# End Source File
# Begin Source File

SOURCE="Read_Buffer.h"
# End Source File
# Begin Source File

SOURCE="Recursive_Thread_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Recyclable.h"
# End Source File
# Begin Source File

SOURCE="Refcountable.h"
# End Source File
# Begin Source File

SOURCE="Refcounted_Auto_Ptr.h"
# End Source File
# Begin Source File

SOURCE="Registry.h"
# End Source File
# Begin Source File

SOURCE="Registry_Name_Space.h"
# End Source File
# Begin Source File

SOURCE="Remote_Name_Space.h"
# End Source File
# Begin Source File

SOURCE="Remote_Tokens.h"
# End Source File
# Begin Source File

SOURCE="Reverse_Lock_T.h"
# End Source File
# Begin Source File

SOURCE="RW_Mutex.h"
# End Source File
# Begin Source File

SOURCE="RW_Process_Mutex.h"
# End Source File
# Begin Source File

SOURCE="RW_Thread_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Sample_History.h"
# End Source File
# Begin Source File

SOURCE="Sbrk_Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Sched_Params.h"
# End Source File
# Begin Source File

SOURCE="Select_Reactor.h"
# End Source File
# Begin Source File

SOURCE="Select_Reactor_Base.h"
# End Source File
# Begin Source File

SOURCE="Select_Reactor_T.h"
# End Source File
# Begin Source File

SOURCE="Semaphore.h"
# End Source File
# Begin Source File

SOURCE="Service_Config.h"
# End Source File
# Begin Source File

SOURCE="Service_Manager.h"
# End Source File
# Begin Source File

SOURCE="Service_Object.h"
# End Source File
# Begin Source File

SOURCE="Service_Repository.h"
# End Source File
# Begin Source File

SOURCE="Service_Templates.h"
# End Source File
# Begin Source File

SOURCE="Service_Types.h"
# End Source File
# Begin Source File

SOURCE="Shared_Memory.h"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_MM.h"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_Pool.h"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_SV.h"
# End Source File
# Begin Source File

SOURCE="Shared_Object.h"
# End Source File
# Begin Source File

SOURCE="Signal.h"
# End Source File
# Begin Source File

SOURCE="Singleton.h"
# End Source File
# Begin Source File

SOURCE="SOCK.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="SOCK_CODgram.h"
# End Source File
# Begin Source File

SOURCE="Sock_Connect.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Connector.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Bcast.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast.h"
# End Source File
# Begin Source File

SOURCE="SOCK_IO.h"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Association.h"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Connector.h"
# End Source File
# Begin Source File

SOURCE="SOCK_Stream.h"
# End Source File
# Begin Source File

SOURCE="SPIPE.h"
# End Source File
# Begin Source File

SOURCE="SPIPE_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="SPIPE_Addr.h"
# End Source File
# Begin Source File

SOURCE="SPIPE_Connector.h"
# End Source File
# Begin Source File

SOURCE="SPIPE_Stream.h"
# End Source File
# Begin Source File

SOURCE="SString.h"
# End Source File
# Begin Source File

SOURCE="SStringfwd.h"
# End Source File
# Begin Source File

SOURCE="Static_Object_Lock.h"
# End Source File
# Begin Source File

SOURCE="Stats.h"
# End Source File
# Begin Source File

SOURCE="Strategies.h"
# End Source File
# Begin Source File

SOURCE="Strategies_T.h"
# End Source File
# Begin Source File

SOURCE="Stream.h"
# End Source File
# Begin Source File

SOURCE="Stream_Modules.h"
# End Source File
# Begin Source File

SOURCE="streams.h"
# End Source File
# Begin Source File

SOURCE="String_Base.h"
# End Source File
# Begin Source File

SOURCE="String_Base_Const.h"
# End Source File
# Begin Source File

SOURCE="SUN_Proactor.h"
# End Source File
# Begin Source File

SOURCE="SV_Message.h"
# End Source File
# Begin Source File

SOURCE="SV_Message_Queue.h"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Complex.h"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Simple.h"
# End Source File
# Begin Source File

SOURCE="SV_Shared_Memory.h"
# End Source File
# Begin Source File

SOURCE="Svc_Conf.h"
# End Source File
# Begin Source File

SOURCE="Svc_Conf_Lexer_Guard.h"
# End Source File
# Begin Source File

SOURCE="Svc_Conf_Tokens.h"
# End Source File
# Begin Source File

SOURCE="svc_export.h"
# End Source File
# Begin Source File

SOURCE="Svc_Handler.h"
# End Source File
# Begin Source File

SOURCE="Swap.h"
# End Source File
# Begin Source File

SOURCE="Synch.h"
# End Source File
# Begin Source File

SOURCE="Synch_Options.h"
# End Source File
# Begin Source File

SOURCE="Synch_T.h"
# End Source File
# Begin Source File

SOURCE="Synch_Traits.h"
# End Source File
# Begin Source File

SOURCE="System_Time.h"
# End Source File
# Begin Source File

SOURCE="Task.h"
# End Source File
# Begin Source File

SOURCE="Task_Ex_T.h"
# End Source File
# Begin Source File

SOURCE="Task_T.h"
# End Source File
# Begin Source File

SOURCE="Test_and_Set.h"
# End Source File
# Begin Source File

SOURCE="Thread.h"
# End Source File
# Begin Source File

SOURCE="Thread_Adapter.h"
# End Source File
# Begin Source File

SOURCE="Thread_Control.h"
# End Source File
# Begin Source File

SOURCE="Thread_Exit.h"
# End Source File
# Begin Source File

SOURCE="Thread_Hook.h"
# End Source File
# Begin Source File

SOURCE="Thread_Manager.h"
# End Source File
# Begin Source File

SOURCE="Thread_Mutex.h"
# End Source File
# Begin Source File

SOURCE="Thread_Semaphore.h"
# End Source File
# Begin Source File

SOURCE="Time_Request_Reply.h"
# End Source File
# Begin Source File

SOURCE="Time_Value.h"
# End Source File
# Begin Source File

SOURCE="Timeprobe.h"
# End Source File
# Begin Source File

SOURCE="Timeprobe_T.h"
# End Source File
# Begin Source File

SOURCE="Timer_Hash.h"
# End Source File
# Begin Source File

SOURCE="Timer_Hash_T.h"
# End Source File
# Begin Source File

SOURCE="Timer_Heap.h"
# End Source File
# Begin Source File

SOURCE="Timer_Heap_T.h"
# End Source File
# Begin Source File

SOURCE="Timer_List.h"
# End Source File
# Begin Source File

SOURCE="Timer_List_T.h"
# End Source File
# Begin Source File

SOURCE="Timer_Queue.h"
# End Source File
# Begin Source File

SOURCE="Timer_Queue_Adapters.h"
# End Source File
# Begin Source File

SOURCE="Timer_Queue_T.h"
# End Source File
# Begin Source File

SOURCE="Timer_Queuefwd.h"
# End Source File
# Begin Source File

SOURCE="Timer_Wheel.h"
# End Source File
# Begin Source File

SOURCE="Timer_Wheel_T.h"
# End Source File
# Begin Source File

SOURCE="TLI.h"
# End Source File
# Begin Source File

SOURCE="TLI_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="TLI_Connector.h"
# End Source File
# Begin Source File

SOURCE="TLI_Stream.h"
# End Source File
# Begin Source File

SOURCE="Token.h"
# End Source File
# Begin Source File

SOURCE="Token_Collection.h"
# End Source File
# Begin Source File

SOURCE="Token_Invariants.h"
# End Source File
# Begin Source File

SOURCE="Token_Manager.h"
# End Source File
# Begin Source File

SOURCE="Token_Request_Reply.h"
# End Source File
# Begin Source File

SOURCE="TP_Reactor.h"
# End Source File
# Begin Source File

SOURCE="Trace.h"
# End Source File
# Begin Source File

SOURCE="TSS_Adapter.h"
# End Source File
# Begin Source File

SOURCE="TSS_T.h"
# End Source File
# Begin Source File

SOURCE="TTY_IO.h"
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message.h"
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message_Queue.h"
# End Source File
# Begin Source File

SOURCE="Unbounded_Queue.h"
# End Source File
# Begin Source File

SOURCE="Unbounded_Set.h"
# End Source File
# Begin Source File

SOURCE="UNIX_Addr.h"
# End Source File
# Begin Source File

SOURCE="UPIPE_Acceptor.h"
# End Source File
# Begin Source File

SOURCE="UPIPE_Addr.h"
# End Source File
# Begin Source File

SOURCE="UPIPE_Connector.h"
# End Source File
# Begin Source File

SOURCE="UPIPE_Stream.h"
# End Source File
# Begin Source File

SOURCE="UUID.h"
# End Source File
# Begin Source File

SOURCE="Value_Ptr.h"
# End Source File
# Begin Source File

SOURCE="Vector_T.h"
# End Source File
# Begin Source File

SOURCE="Version.h"
# End Source File
# Begin Source File

SOURCE="WFMO_Reactor.h"
# End Source File
# Begin Source File

SOURCE="WIN32_Asynch_IO.h"
# End Source File
# Begin Source File

SOURCE="WIN32_Proactor.h"
# End Source File
# Begin Source File

SOURCE="XML_Svc_Conf.h"
# End Source File
# Begin Source File

SOURCE="XTI_ATM_Mcast.h"
# End Source File
# End Group
# Begin Group "Inline Files"

# PROP Default_Filter "i;inl"
# Begin Source File

SOURCE="ACE.inl"
# End Source File
# Begin Source File

SOURCE="ace_wchar.inl"
# End Source File
# Begin Source File

SOURCE="Activation_Queue.inl"
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager_T.inl"
# End Source File
# Begin Source File

SOURCE="Addr.inl"
# End Source File
# Begin Source File

SOURCE="ARGV.inl"
# End Source File
# Begin Source File

SOURCE="Argv_Type_Converter.inl"
# End Source File
# Begin Source File

SOURCE="Array_Base.inl"
# End Source File
# Begin Source File

SOURCE="Array_Map.inl"
# End Source File
# Begin Source File

SOURCE="Asynch_IO_Impl.inl"
# End Source File
# Begin Source File

SOURCE="ATM_Acceptor.inl"
# End Source File
# Begin Source File

SOURCE="ATM_Addr.inl"
# End Source File
# Begin Source File

SOURCE="ATM_Connector.inl"
# End Source File
# Begin Source File

SOURCE="ATM_Params.inl"
# End Source File
# Begin Source File

SOURCE="ATM_QoS.inl"
# End Source File
# Begin Source File

SOURCE="ATM_Stream.inl"
# End Source File
# Begin Source File

SOURCE="Atomic_Op.inl"
# End Source File
# Begin Source File

SOURCE="Atomic_Op_T.inl"
# End Source File
# Begin Source File

SOURCE="Auto_Event.inl"
# End Source File
# Begin Source File

SOURCE="Auto_Functor.inl"
# End Source File
# Begin Source File

SOURCE="Auto_IncDec_T.inl"
# End Source File
# Begin Source File

SOURCE="Auto_Ptr.inl"
# End Source File
# Begin Source File

SOURCE="Barrier.inl"
# End Source File
# Begin Source File

SOURCE="Base_Thread_Adapter.inl"
# End Source File
# Begin Source File

SOURCE="Based_Pointer_T.inl"
# End Source File
# Begin Source File

SOURCE="Basic_Stats.inl"
# End Source File
# Begin Source File

SOURCE="Basic_Types.inl"
# End Source File
# Begin Source File

SOURCE="Bound_Ptr.inl"
# End Source File
# Begin Source File

SOURCE="Cache_Map_Manager_T.inl"
# End Source File
# Begin Source File

SOURCE="Caching_Strategies_T.inl"
# End Source File
# Begin Source File

SOURCE="Capabilities.inl"
# End Source File
# Begin Source File

SOURCE="CDR_Base.inl"
# End Source File
# Begin Source File

SOURCE="CDR_Size.inl"
# End Source File
# Begin Source File

SOURCE="CDR_Stream.inl"
# End Source File
# Begin Source File

SOURCE="Cleanup.inl"
# End Source File
# Begin Source File

SOURCE="Codeset_Registry.inl"
# End Source File
# Begin Source File

SOURCE="Condition_T.inl"
# End Source File
# Begin Source File

SOURCE="Condition_Thread_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Containers.inl"
# End Source File
# Begin Source File

SOURCE="Containers_T.inl"
# End Source File
# Begin Source File

SOURCE="Date_Time.inl"
# End Source File
# Begin Source File

SOURCE="DEV.inl"
# End Source File
# Begin Source File

SOURCE="DEV_Addr.inl"
# End Source File
# Begin Source File

SOURCE="DEV_Connector.inl"
# End Source File
# Begin Source File

SOURCE="DEV_IO.inl"
# End Source File
# Begin Source File

SOURCE="Dev_Poll_Reactor.inl"
# End Source File
# Begin Source File

SOURCE="Dirent.inl"
# End Source File
# Begin Source File

SOURCE="Dirent_Selector.inl"
# End Source File
# Begin Source File

SOURCE="Dynamic.inl"
# End Source File
# Begin Source File

SOURCE="Dynamic_Service.inl"
# End Source File
# Begin Source File

SOURCE="Env_Value_T.inl"
# End Source File
# Begin Source File

SOURCE="Event.inl"
# End Source File
# Begin Source File

SOURCE="Event_Handler.inl"
# End Source File
# Begin Source File

SOURCE="Event_Handler_T.inl"
# End Source File
# Begin Source File

SOURCE="FIFO.inl"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv.inl"
# End Source File
# Begin Source File

SOURCE="FIFO_Recv_Msg.inl"
# End Source File
# Begin Source File

SOURCE="FIFO_Send.inl"
# End Source File
# Begin Source File

SOURCE="FIFO_Send_Msg.inl"
# End Source File
# Begin Source File

SOURCE="FILE.inl"
# End Source File
# Begin Source File

SOURCE="FILE_Addr.inl"
# End Source File
# Begin Source File

SOURCE="FILE_Connector.inl"
# End Source File
# Begin Source File

SOURCE="FILE_IO.inl"
# End Source File
# Begin Source File

SOURCE="File_Lock.inl"
# End Source File
# Begin Source File

SOURCE="Flag_Manip.inl"
# End Source File
# Begin Source File

SOURCE="Framework_Component.inl"
# End Source File
# Begin Source File

SOURCE="Functor.inl"
# End Source File
# Begin Source File

SOURCE="Functor_String.inl"
# End Source File
# Begin Source File

SOURCE="Functor_T.inl"
# End Source File
# Begin Source File

SOURCE="Get_Opt.inl"
# End Source File
# Begin Source File

SOURCE="Guard_T.inl"
# End Source File
# Begin Source File

SOURCE="Handle_Gobbler.inl"
# End Source File
# Begin Source File

SOURCE="Handle_Set.inl"
# End Source File
# Begin Source File

SOURCE="Hash_Cache_Map_Manager_T.inl"
# End Source File
# Begin Source File

SOURCE="Hash_Map_Manager_T.inl"
# End Source File
# Begin Source File

SOURCE="Hash_Map_With_Allocator_T.inl"
# End Source File
# Begin Source File

SOURCE="Hashable.inl"
# End Source File
# Begin Source File

SOURCE="High_Res_Timer.inl"
# End Source File
# Begin Source File

SOURCE="ICMP_Socket.inl"
# End Source File
# Begin Source File

SOURCE="INET_Addr.inl"
# End Source File
# Begin Source File

SOURCE="Intrusive_List.inl"
# End Source File
# Begin Source File

SOURCE="Intrusive_List_Node.inl"
# End Source File
# Begin Source File

SOURCE="IO_Cntl_Msg.inl"
# End Source File
# Begin Source File

SOURCE="IO_SAP.inl"
# End Source File
# Begin Source File

SOURCE="IOStream_T.inl"
# End Source File
# Begin Source File

SOURCE="IPC_SAP.inl"
# End Source File
# Begin Source File

SOURCE="Local_Tokens.inl"
# End Source File
# Begin Source File

SOURCE="Lock.inl"
# End Source File
# Begin Source File

SOURCE="Lock_Adapter_T.inl"
# End Source File
# Begin Source File

SOURCE="Log_Record.inl"
# End Source File
# Begin Source File

SOURCE="LSOCK.inl"
# End Source File
# Begin Source File

SOURCE="LSOCK_CODgram.inl"
# End Source File
# Begin Source File

SOURCE="LSOCK_Connector.inl"
# End Source File
# Begin Source File

SOURCE="LSOCK_Dgram.inl"
# End Source File
# Begin Source File

SOURCE="LSOCK_Stream.inl"
# End Source File
# Begin Source File

SOURCE="Malloc.inl"
# End Source File
# Begin Source File

SOURCE="Malloc_Allocator.inl"
# End Source File
# Begin Source File

SOURCE="Malloc_T.inl"
# End Source File
# Begin Source File

SOURCE="Managed_Object.inl"
# End Source File
# Begin Source File

SOURCE="Manual_Event.inl"
# End Source File
# Begin Source File

SOURCE="Map_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Map_T.inl"
# End Source File
# Begin Source File

SOURCE="MEM_Acceptor.inl"
# End Source File
# Begin Source File

SOURCE="MEM_Addr.inl"
# End Source File
# Begin Source File

SOURCE="MEM_Connector.inl"
# End Source File
# Begin Source File

SOURCE="MEM_IO.inl"
# End Source File
# Begin Source File

SOURCE="Mem_Map.inl"
# End Source File
# Begin Source File

SOURCE="MEM_SAP.inl"
# End Source File
# Begin Source File

SOURCE="MEM_Stream.inl"
# End Source File
# Begin Source File

SOURCE="Message_Block.inl"
# End Source File
# Begin Source File

SOURCE="Message_Block_T.inl"
# End Source File
# Begin Source File

SOURCE="Message_Queue.inl"
# End Source File
# Begin Source File

SOURCE="Module.inl"
# End Source File
# Begin Source File

SOURCE="Msg_WFMO_Reactor.inl"
# End Source File
# Begin Source File

SOURCE="Multihomed_INET_Addr.inl"
# End Source File
# Begin Source File

SOURCE="Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Notification_Strategy.inl"
# End Source File
# Begin Source File

SOURCE="NT_Service.inl"
# End Source File
# Begin Source File

SOURCE="Obchunk.inl"
# End Source File
# Begin Source File

SOURCE="Object_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Obstack_T.inl"
# End Source File
# Begin Source File

SOURCE="OS.inl"
# End Source File
# Begin Source File

SOURCE="OS_Errno.inl"
# End Source File
# Begin Source File

SOURCE="OS_Log_Msg_Attributes.inl"
# End Source File
# Begin Source File

SOURCE="OS_Memory.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_arpa_inet.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_ctype.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_dirent.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_dlfcn.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_errno.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_fcntl.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_math.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_netdb.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_poll.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_pwd.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_regex.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_signal.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdio.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_stdlib.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_string.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_strings.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_stropts.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_mman.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_msg.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_resource.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_select.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_shm.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_socket.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_stat.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_time.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_uio.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_utsname.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_sys_wait.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_Thread.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_time.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_unistd.inl"
# End Source File
# Begin Source File

SOURCE="OS_NS_wchar.inl"
# End Source File
# Begin Source File

SOURCE="OS_String.inl"
# End Source File
# Begin Source File

SOURCE="OS_TLI.inl"
# End Source File
# Begin Source File

SOURCE="Pagefile_Memory_Pool.inl"
# End Source File
# Begin Source File

SOURCE="Pair_T.inl"
# End Source File
# Begin Source File

SOURCE="PI_Malloc.inl"
# End Source File
# Begin Source File

SOURCE="Ping_Socket.inl"
# End Source File
# Begin Source File

SOURCE="Pipe.inl"
# End Source File
# Begin Source File

SOURCE="POSIX_Proactor.inl"
# End Source File
# Begin Source File

SOURCE="Proactor.inl"
# End Source File
# Begin Source File

SOURCE="Process.inl"
# End Source File
# Begin Source File

SOURCE="Process_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Process_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Process_Semaphore.inl"
# End Source File
# Begin Source File

SOURCE="Profile_Timer.inl"
# End Source File
# Begin Source File

SOURCE="RB_Tree.inl"
# End Source File
# Begin Source File

SOURCE="Reactor.inl"
# End Source File
# Begin Source File

SOURCE="Reactor_Notification_Strategy.inl"
# End Source File
# Begin Source File

SOURCE="Read_Buffer.inl"
# End Source File
# Begin Source File

SOURCE="Recursive_Thread_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Recyclable.inl"
# End Source File
# Begin Source File

SOURCE="Refcountable.inl"
# End Source File
# Begin Source File

SOURCE="Refcounted_Auto_Ptr.inl"
# End Source File
# Begin Source File

SOURCE="Remote_Tokens.inl"
# End Source File
# Begin Source File

SOURCE="Reverse_Lock_T.inl"
# End Source File
# Begin Source File

SOURCE="RW_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="RW_Process_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="RW_Thread_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Sample_History.inl"
# End Source File
# Begin Source File

SOURCE="Sched_Params.inl"
# End Source File
# Begin Source File

SOURCE="Select_Reactor_Base.inl"
# End Source File
# Begin Source File

SOURCE="Select_Reactor_T.inl"
# End Source File
# Begin Source File

SOURCE="Semaphore.inl"
# End Source File
# Begin Source File

SOURCE="Service_Config.inl"
# End Source File
# Begin Source File

SOURCE="Service_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Service_Object.inl"
# End Source File
# Begin Source File

SOURCE="Service_Repository.inl"
# End Source File
# Begin Source File

SOURCE="Service_Types.inl"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_MM.inl"
# End Source File
# Begin Source File

SOURCE="Shared_Memory_SV.inl"
# End Source File
# Begin Source File

SOURCE="Shared_Object.inl"
# End Source File
# Begin Source File

SOURCE="Signal.inl"
# End Source File
# Begin Source File

SOURCE="Singleton.inl"
# End Source File
# Begin Source File

SOURCE="SOCK.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Acceptor.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_CODgram.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Connector.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Bcast.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Dgram_Mcast.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_IO.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Acceptor.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Association.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_SEQPACK_Connector.inl"
# End Source File
# Begin Source File

SOURCE="SOCK_Stream.inl"
# End Source File
# Begin Source File

SOURCE="SPIPE.inl"
# End Source File
# Begin Source File

SOURCE="SPIPE_Addr.inl"
# End Source File
# Begin Source File

SOURCE="SPIPE_Connector.inl"
# End Source File
# Begin Source File

SOURCE="SPIPE_Stream.inl"
# End Source File
# Begin Source File

SOURCE="SString.inl"
# End Source File
# Begin Source File

SOURCE="Stats.inl"
# End Source File
# Begin Source File

SOURCE="Strategies_T.inl"
# End Source File
# Begin Source File

SOURCE="Stream.inl"
# End Source File
# Begin Source File

SOURCE="String_Base.inl"
# End Source File
# Begin Source File

SOURCE="SV_Message.inl"
# End Source File
# Begin Source File

SOURCE="SV_Message_Queue.inl"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Complex.inl"
# End Source File
# Begin Source File

SOURCE="SV_Semaphore_Simple.inl"
# End Source File
# Begin Source File

SOURCE="SV_Shared_Memory.inl"
# End Source File
# Begin Source File

SOURCE="Swap.inl"
# End Source File
# Begin Source File

SOURCE="Synch_Options.inl"
# End Source File
# Begin Source File

SOURCE="Task.inl"
# End Source File
# Begin Source File

SOURCE="Task_Ex_T.inl"
# End Source File
# Begin Source File

SOURCE="Task_T.inl"
# End Source File
# Begin Source File

SOURCE="Thread.inl"
# End Source File
# Begin Source File

SOURCE="Thread_Adapter.inl"
# End Source File
# Begin Source File

SOURCE="Thread_Control.inl"
# End Source File
# Begin Source File

SOURCE="Thread_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Thread_Mutex.inl"
# End Source File
# Begin Source File

SOURCE="Thread_Semaphore.inl"
# End Source File
# Begin Source File

SOURCE="Time_Value.inl"
# End Source File
# Begin Source File

SOURCE="Timeprobe.inl"
# End Source File
# Begin Source File

SOURCE="Timer_Queue_Adapters.inl"
# End Source File
# Begin Source File

SOURCE="Timer_Queue_T.inl"
# End Source File
# Begin Source File

SOURCE="TLI.inl"
# End Source File
# Begin Source File

SOURCE="TLI_Connector.inl"
# End Source File
# Begin Source File

SOURCE="TLI_Stream.inl"
# End Source File
# Begin Source File

SOURCE="Token.inl"
# End Source File
# Begin Source File

SOURCE="Token_Collection.inl"
# End Source File
# Begin Source File

SOURCE="Token_Manager.inl"
# End Source File
# Begin Source File

SOURCE="Token_Request_Reply.inl"
# End Source File
# Begin Source File

SOURCE="TP_Reactor.inl"
# End Source File
# Begin Source File

SOURCE="TSS_Adapter.inl"
# End Source File
# Begin Source File

SOURCE="TSS_T.inl"
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message.inl"
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message_Queue.inl"
# End Source File
# Begin Source File

SOURCE="Unbounded_Queue.inl"
# End Source File
# Begin Source File

SOURCE="Unbounded_Set.inl"
# End Source File
# Begin Source File

SOURCE="UNIX_Addr.inl"
# End Source File
# Begin Source File

SOURCE="UPIPE_Acceptor.inl"
# End Source File
# Begin Source File

SOURCE="UPIPE_Connector.inl"
# End Source File
# Begin Source File

SOURCE="UPIPE_Stream.inl"
# End Source File
# Begin Source File

SOURCE="UUID.inl"
# End Source File
# Begin Source File

SOURCE="Vector_T.inl"
# End Source File
# Begin Source File

SOURCE="WFMO_Reactor.inl"
# End Source File
# Begin Source File

SOURCE="XTI_ATM_Mcast.inl"
# End Source File
# End Group
# Begin Group "Template Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="Acceptor.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Active_Map_Manager_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Array_Base.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Array_Map.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Asynch_Acceptor.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Asynch_Connector.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Atomic_Op_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Auto_Functor.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Auto_IncDec_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Auto_Ptr.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Based_Pointer_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Cache_Map_Manager_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Cached_Connect_Strategy_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Caching_Strategies_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Caching_Utility_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Cleanup_Strategies_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Condition_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Connector.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Containers_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Dump_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Dynamic_Service.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Env_Value_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Event_Handler_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Framework_Component_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Free_List.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Functor_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Future.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Future_Set.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Guard_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Hash_Cache_Map_Manager_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Hash_Map_Manager_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Hash_Map_With_Allocator_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Intrusive_List.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Intrusive_List_Node.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="IOStream_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Local_Name_Space_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Lock_Adapter_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="LOCK_SOCK_Acceptor.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Malloc_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Managed_Object.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Map_Manager.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Map_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Message_Block_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Message_Queue_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Metrics_Cache_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Module.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Node.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Obstack_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Pair_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="RB_Tree.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Refcounted_Auto_Ptr.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Reverse_Lock_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Select_Reactor_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Singleton.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Strategies_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Stream.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Stream_Modules.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="String_Base.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Svc_Handler.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Synch_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Task_Ex_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Task_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Template_Instantiations.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Test_and_Set.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timeprobe_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_Hash_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_Heap_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_List_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_Queue_Adapters.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_Queue_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Timer_Wheel_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="TSS_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Typed_SV_Message_Queue.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Unbounded_Queue.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Unbounded_Set.cpp"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE="Vector_T.cpp"
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\VERSION"
# End Source File
# Begin Source File

SOURCE="README"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE="ace.rc"
# End Source File
# End Group
# Begin Group "Pkgconfig Files"

# PROP Default_Filter "in"
# Begin Source File

SOURCE="ACE.pc.in"
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
