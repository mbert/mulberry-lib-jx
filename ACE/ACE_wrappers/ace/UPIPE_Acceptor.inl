/* -*- C++ -*- */
// UPIPE_Acceptor.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE int
ACE_UPIPE_Acceptor::remove (void)
{
  ACE_TRACE ("ACE_UPIPE_Acceptor::remove");
  return this->ACE_SPIPE_Acceptor::remove ();
}
