/* -*- C++ -*- */
// LSOCK_CODgram.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Do nothing constructor.

ACE_INLINE
ACE_LSOCK_CODgram::ACE_LSOCK_CODgram (void)
{
  ACE_TRACE ("ACE_LSOCK_CODgram::ACE_LSOCK_CODgram");
}

ACE_INLINE void
ACE_LSOCK_CODgram::set_handle (ACE_HANDLE h)
{
  ACE_TRACE ("ACE_LSOCK_CODgram::set_handle");
  this->ACE_SOCK_CODgram::set_handle (h);
  this->ACE_LSOCK::set_handle (h);
}

ACE_INLINE ACE_HANDLE
ACE_LSOCK_CODgram::get_handle (void) const
{
  ACE_TRACE ("ACE_LSOCK_CODgram::get_handle");
  return this->ACE_SOCK_CODgram::get_handle ();
}
