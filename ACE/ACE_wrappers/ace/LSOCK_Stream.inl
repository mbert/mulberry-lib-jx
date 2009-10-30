/* -*- C++ -*- */
// LSOCK_Stream.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Sets both the file descriptors... Overrides handle from the base
// classes.

ACE_INLINE void
ACE_LSOCK_Stream::set_handle (ACE_HANDLE fd)
{
  ACE_TRACE ("ACE_LSOCK_Stream::set_handle");
  this->ACE_SOCK_Stream::set_handle (fd);
  this->ACE_LSOCK::set_handle (fd);
}

ACE_INLINE ACE_HANDLE
ACE_LSOCK_Stream::get_handle (void) const
{
  ACE_TRACE ("ACE_LSOCK_Stream::get_handle");
  return this->ACE_SOCK_Stream::get_handle ();
}



