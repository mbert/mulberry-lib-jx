// -*- C++ -*-
// OS_NS_sys_shm.inl,v 1.7 2004/06/24 14:07:03 jtc Exp

#include "ace/OS_NS_errno.h"

ACE_INLINE void *
ACE_OS::shmat (int int_id, void *shmaddr, int shmflg)
{
  ACE_OS_TRACE ("ACE_OS::shmat");
#if defined (ACE_HAS_SYSV_IPC)
  ACE_OSCALL_RETURN (::shmat (int_id, static_cast <char *> (shmaddr), shmflg), void *, (void *) -1);
#else
  ACE_UNUSED_ARG (int_id);
  ACE_UNUSED_ARG (shmaddr);
  ACE_UNUSED_ARG (shmflg);

  ACE_NOTSUP_RETURN ((void *) -1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::shmctl (int int_id, int cmd, struct shmid_ds *buf)
{
  ACE_OS_TRACE ("ACE_OS::shmctl");
#if defined (ACE_HAS_SYSV_IPC)
  ACE_OSCALL_RETURN (::shmctl (int_id, cmd, buf), int, -1);
#else
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (int_id);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::shmdt (void *shmaddr)
{
  ACE_OS_TRACE ("ACE_OS::shmdt");
#if defined (ACE_HAS_SYSV_IPC)
  ACE_OSCALL_RETURN (::shmdt ((char *) shmaddr), int, -1);
#else
  ACE_UNUSED_ARG (shmaddr);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
ACE_OS::shmget (key_t key, int size, int flags)
{
  ACE_OS_TRACE ("ACE_OS::shmget");
#if defined (ACE_HAS_SYSV_IPC)
  ACE_OSCALL_RETURN (::shmget (key, size, flags), int, -1);
#else
  ACE_UNUSED_ARG (flags);
  ACE_UNUSED_ARG (size);
  ACE_UNUSED_ARG (key);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}
