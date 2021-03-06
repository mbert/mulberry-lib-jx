// -*- C++ -*-
// OS_NS_sys_select.inl,v 1.5 2004/11/12 00:03:08 gmaxey Exp

#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_macros.h"
#include "ace/Time_Value.h"

// It would be really cool to add another version of select that would
// function like the one we're defending against below!
ACE_INLINE int
ACE_OS::select (int width,
                fd_set *rfds, fd_set *wfds, fd_set *efds,
                const ACE_Time_Value *timeout)
{
  ACE_OS_TRACE ("ACE_OS::select");
#if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
  // We must defend against non-conformity!
  timeval copy;
  timeval *timep;

  if (timeout != 0)
    {
      copy = *timeout;
      timep = &copy;
    }
  else
    timep = 0;
#else
  const timeval *timep = (timeout == 0 ? (const timeval *)0 : *timeout);
#endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
#if defined(ACE_TANDEM_T1248_PTHREADS)
  ACE_SOCKCALL_RETURN (::spt_select (width,
                                 (ACE_FD_SET_TYPE *) rfds,
                                 (ACE_FD_SET_TYPE *) wfds,
                                 (ACE_FD_SET_TYPE *) efds,
                                 timep),
                       int, -1);
#else
  ACE_SOCKCALL_RETURN (::select (width,
                                 (ACE_FD_SET_TYPE *) rfds,
                                 (ACE_FD_SET_TYPE *) wfds,
                                 (ACE_FD_SET_TYPE *) efds,
                                 timep),
                       int, -1);
#endif
}

ACE_INLINE int
ACE_OS::select (int width,
                fd_set *rfds, fd_set *wfds, fd_set *efds,
                const ACE_Time_Value &timeout)
{
  ACE_OS_TRACE ("ACE_OS::select");
#if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
# define ___ACE_TIMEOUT &copy
  timeval copy = timeout;
#else
# define ___ACE_TIMEOUT timep
  const timeval *timep = timeout;
#endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
#if defined(ACE_TANDEM_T1248_PTHREADS)
  ACE_SOCKCALL_RETURN (::spt_select (width,
                                 (ACE_FD_SET_TYPE *) rfds,
                                 (ACE_FD_SET_TYPE *) wfds,
                                 (ACE_FD_SET_TYPE *) efds,
                                 ___ACE_TIMEOUT),
                       int, -1);
#else
  ACE_SOCKCALL_RETURN (::select (width,
                                 (ACE_FD_SET_TYPE *) rfds,
                                 (ACE_FD_SET_TYPE *) wfds,
                                 (ACE_FD_SET_TYPE *) efds,
                                 ___ACE_TIMEOUT),
                       int, -1);
#endif
#undef ___ACE_TIMEOUT
}

