/* -*- C++ -*- */
// UPIPE_Stream.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE ACE_HANDLE
ACE_UPIPE_Stream::get_handle (void) const
{
  ACE_TRACE ("ACE_UPIPE_Stream::get_handle");
  return this->ACE_SPIPE::get_handle ();
}

