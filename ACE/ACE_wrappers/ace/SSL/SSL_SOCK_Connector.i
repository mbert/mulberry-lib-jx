/* -*- C++ -*- */
// SSL_SOCK_Connector.i,v 1.8 2004/06/19 07:18:01 jwillemsen Exp

// SSL_SOCK_Connector.i

ACE_INLINE
ACE_SSL_SOCK_Connector::ACE_SSL_SOCK_Connector (void)
  : connector_ ()
{
  ACE_TRACE ("ACE_SSL_SOCK_Connector::ACE_SSL_SOCK_Connector");
}

ACE_INLINE int
ACE_SSL_SOCK_Connector::reset_new_handle (ACE_HANDLE handle)
{
  ACE_TRACE ("ACE_SSL_SOCK_Connector::reset_new_handle");
  return this->connector_.reset_new_handle (handle);
}

ACE_INLINE void
ACE_SSL_SOCK_Connector::dump (void) const
{
  ACE_TRACE ("ACE_SSL_SOCK_Connector::dump");
  this->connector_.dump ();
}
