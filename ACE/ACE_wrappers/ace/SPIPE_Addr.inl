// -*- C++ -*-
//
// SPIPE_Addr.inl,v 4.2 2005/05/19 01:48:53 ossama Exp


#include "ace/SString.h"


// Compare two addresses for equality.

ACE_INLINE bool
ACE_SPIPE_Addr::operator == (const ACE_SPIPE_Addr &sap) const
{
  return ACE_OS::strcmp (this->SPIPE_addr_.rendezvous_,
                         sap.SPIPE_addr_.rendezvous_    ) == 0;
}

// Compare two addresses for inequality.

ACE_INLINE bool
ACE_SPIPE_Addr::operator != (const ACE_SPIPE_Addr &sap) const
{
  return !((*this) == sap);	// This is lazy, of course... ;-)
}

// Return the path name used for the rendezvous point.

ACE_INLINE const ACE_TCHAR *
ACE_SPIPE_Addr::get_path_name (void) const
{
  return this->SPIPE_addr_.rendezvous_;
}

ACE_INLINE uid_t
ACE_SPIPE_Addr::user_id (void) const
{
  return this->SPIPE_addr_.uid_;
}

ACE_INLINE void
ACE_SPIPE_Addr::user_id (uid_t uid)
{
  this->SPIPE_addr_.uid_ = uid;
}

ACE_INLINE gid_t
ACE_SPIPE_Addr::group_id (void) const
{
  return this->SPIPE_addr_.gid_;
}

ACE_INLINE void
ACE_SPIPE_Addr::group_id (gid_t gid)
{
  this->SPIPE_addr_.gid_ = gid;
}
