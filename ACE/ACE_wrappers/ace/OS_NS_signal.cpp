// -*- C++ -*-
// OS_NS_signal.cpp,v 1.2 2003/11/01 11:15:15 dhinton Exp

#include "ace/OS_NS_signal.h"

ACE_RCSID(ace, OS_NS_signal, "OS_NS_signal.cpp,v 1.2 2003/11/01 11:15:15 dhinton Exp")

#if !defined (ACE_HAS_INLINED_OSCALLS)
# include "ace/OS_NS_signal.inl"
#endif /* ACE_HAS_INLINED_OS_CALLS */

#if !defined (ACE_HAS_SIGINFO_T)
siginfo_t::siginfo_t (ACE_HANDLE handle)
  : si_handle_ (handle),
    si_handles_ (&handle)
{
}

siginfo_t::siginfo_t (ACE_HANDLE *handles)
  : si_handle_ (handles[0]),
    si_handles_ (handles)
{
}
#endif /* ACE_HAS_SIGINFO_T */
