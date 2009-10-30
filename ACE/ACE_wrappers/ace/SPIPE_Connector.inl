/* -*- C++ -*- */
// SPIPE_Connector.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE int
ACE_SPIPE_Connector::reset_new_handle (ACE_HANDLE handle)
{
  ACE_UNUSED_ARG (handle);
  // Nothing to do here since the handle is not a socket
  return 0;
}

