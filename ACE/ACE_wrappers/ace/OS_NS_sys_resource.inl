// -*- C++ -*-
// OS_NS_sys_resource.inl,v 1.5 2004/08/08 14:21:35 jtc Exp

#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_macros.h"

ACE_INLINE int
ACE_OS::getrlimit (int resource, struct rlimit *rl)
{
  ACE_OS_TRACE ("ACE_OS::getrlimit");

#if defined (ACE_LACKS_RLIMIT)
  ACE_UNUSED_ARG (resource);
  ACE_UNUSED_ARG (rl);

  ACE_NOTSUP_RETURN (-1);
#else
# if defined (ACE_HAS_RLIMIT_RESOURCE_ENUM)
  ACE_OSCALL_RETURN (::getrlimit ((ACE_HAS_RLIMIT_RESOURCE_ENUM) resource, rl), int, -1);
# else
  ACE_OSCALL_RETURN (::getrlimit (resource, rl), int, -1);
# endif /* ACE_HAS_RLIMIT_RESOURCE_ENUM */
#endif /* ACE_LACKS_RLIMIT */
}

ACE_INLINE int
ACE_OS::getrusage (int who, struct rusage *ru)
{
  ACE_OS_TRACE ("ACE_OS::getrusage");

#if defined (ACE_HAS_SYSCALL_GETRUSAGE)
  // This nonsense is necessary for HP/UX...
  ACE_OSCALL_RETURN (::syscall (SYS_GETRUSAGE, who, ru), int, -1);
#elif defined (ACE_HAS_GETRUSAGE)
# if defined (ACE_WIN32)
  ACE_UNUSED_ARG (who);

  FILETIME dummy_1;
  FILETIME dummy_2;
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::GetProcessTimes (::GetCurrentProcess(),
                                                             &dummy_1,   // start
                                                             &dummy_2,     // exited
                                                             &ru->ru_stime,
                                                             &ru->ru_utime),
                                          ace_result_),
                        int, -1);
# else
#   if defined (ACE_HAS_RUSAGE_WHO_ENUM)
  ACE_OSCALL_RETURN (::getrusage ((ACE_HAS_RUSAGE_WHO_ENUM) who, ru), int, -1);
#   else
  ACE_OSCALL_RETURN (::getrusage (who, ru), int, -1);
#   endif /* ACE_HAS_RUSAGE_WHO_ENUM */
# endif /* ACE_WIN32 */
#else
  who = who;
  ru = ru;
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSCALL_GETRUSAGE */
}

ACE_INLINE int
ACE_OS::setrlimit (int resource, const struct rlimit *rl)
{
  ACE_OS_TRACE ("ACE_OS::setrlimit");

#if defined (ACE_LACKS_RLIMIT)
  ACE_UNUSED_ARG (resource);
  ACE_UNUSED_ARG (rl);

  ACE_NOTSUP_RETURN (-1);
#else
# if defined (ACE_HAS_RLIMIT_RESOURCE_ENUM)
#  if defined (ACE_HAS_NONCONST_SETRLIMIT)
  ACE_OSCALL_RETURN (::setrlimit ((ACE_HAS_RLIMIT_RESOURCE_ENUM) resource,
                                  const_cast<struct rlimit *>(rl)
                                  ), int, -1);
# else
  ACE_OSCALL_RETURN (::setrlimit ((ACE_HAS_RLIMIT_RESOURCE_ENUM) resource,
                                  resource,
                                  ), int, -1);
#  endif /* ACE_HAS_NONCONST_SETRLIMIT */				  
# else /* ACE_HAS_RLIMIT_RESOURCE_ENUM */
#  if defined (ACE_HAS_NONCONST_SETRLIMIT)
  ACE_OSCALL_RETURN (::setrlimit (resource, const_cast<struct rlimit *>(rl)
                                  ), int, -1);
#  else
  ACE_OSCALL_RETURN (::setrlimit (resource, rl), int, -1);
#  endif /* ACE_HAS_NONCONST_SETRLIMIT */
# endif /* ACE_HAS_RLIMIT_RESOURCE_ENUM */  
#endif /* ACE_LACKS_RLIMIT */
}

