/* -*- C++ -*- */
// FIFO_Send.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// FIFO_Send.i

#include "ace/ACE.h"
#include "ace/OS_NS_unistd.h"

ACE_INLINE ssize_t
ACE_FIFO_Send::send (const void *buf, size_t len)
{
  ACE_TRACE ("ACE_FIFO_Send::send");
  return ACE_OS::write (this->get_handle (), (const char *) buf, len);
}

ACE_INLINE ssize_t
ACE_FIFO_Send::send_n (const void *buf, size_t n)
{
  ACE_TRACE ("ACE_FIFO_Send::send_n");
  return ACE::send_n (this->get_handle (), buf, n);
}
