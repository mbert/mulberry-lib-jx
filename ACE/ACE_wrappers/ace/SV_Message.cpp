// SV_Message.cpp
// SV_Message.cpp,v 4.4 2004/06/16 07:57:21 jwillemsen Exp

#include "ace/SV_Message.h"

#if !defined (__ACE_INLINE__)
#include "ace/SV_Message.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, SV_Message, "SV_Message.cpp,v 4.4 2004/06/16 07:57:21 jwillemsen Exp")

ACE_ALLOC_HOOK_DEFINE(ACE_SV_Message)

void
ACE_SV_Message::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_SV_Message::dump");
#endif /* ACE_HAS_DUMP */
}

