// -*- C++ -*-
//
// FILE_Addr.inl,v 4.2 2005/05/19 01:48:53 ossama Exp


#include "ace/SString.h"

// Compare two addresses for equality.

ACE_INLINE bool
ACE_FILE_Addr::operator == (const ACE_FILE_Addr &sap) const
{
  return ACE_OS::strcmp (this->filename_, sap.filename_) == 0;
}

// Compare two addresses for inequality.

ACE_INLINE bool
ACE_FILE_Addr::operator != (const ACE_FILE_Addr &sap) const
{
  return !((*this) == sap);	// This is lazy, of course... ;-)
}

// Return the path name used for the rendezvous point.

ACE_INLINE const ACE_TCHAR *
ACE_FILE_Addr::get_path_name (void) const
{
  return this->filename_;
}
