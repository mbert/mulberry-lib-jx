/* -*- C++ -*- */
// TLI_Stream.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// TLI_Stream.i

#include "ace/TLI_Stream.h"

ACE_INLINE
void
ACE_TLI_Stream::set_rwflag (int value)
{
  ACE_TRACE ("ACE_TLI_Stream::set_rwflag");
  this->rwflag_ = value;
}

ACE_INLINE
int
ACE_TLI_Stream::get_rwflag (void)
{
  ACE_TRACE ("ACE_TLI_Stream::get_rwflag");
  return this->rwflag_;
}
