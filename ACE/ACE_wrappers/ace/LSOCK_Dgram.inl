/* -*- C++ -*- */
// LSOCK_Dgram.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// LSOCK_Dgram.i

ACE_INLINE void
ACE_LSOCK_Dgram::set_handle (ACE_HANDLE h)
{
  ACE_TRACE ("ACE_LSOCK_Dgram::set_handle");
  this->ACE_SOCK_Dgram::set_handle (h);
  this->ACE_LSOCK::set_handle (h);
}

ACE_INLINE ACE_HANDLE
ACE_LSOCK_Dgram::get_handle (void) const
{
  ACE_TRACE ("ACE_LSOCK_Dgram::get_handle");
  return this->ACE_SOCK_Dgram::get_handle ();
}

