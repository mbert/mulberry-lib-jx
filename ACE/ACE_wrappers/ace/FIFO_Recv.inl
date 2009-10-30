/* -*- C++ -*- */
// FIFO_Recv.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// FIFO_Recv.i

#include "ace/ACE.h"
#include "ace/OS_NS_unistd.h"

ACE_INLINE ssize_t
ACE_FIFO_Recv::recv (void *buf, size_t len)
{
  ACE_TRACE ("ACE_FIFO_Recv::recv");
  return ACE_OS::read (this->get_handle (), (char *) buf, len);
}

ACE_INLINE ssize_t
ACE_FIFO_Recv::recv_n (void *buf, size_t n)
{
  ACE_TRACE ("ACE_FIFO_Recv::recv_n");
  return ACE::recv_n (this->get_handle (), buf, n);
}
