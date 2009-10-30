/* -*- C++ -*- */
// Service_Types.inl,v 4.2 2005/05/18 23:26:29 ossama Exp

#include "ace/ACE.h"

ACE_INLINE void *
ACE_Service_Type_Impl::object (void) const
{
  ACE_TRACE ("ACE_Service_Type_Impl::object");
  return this->obj_;
}

ACE_INLINE const ACE_TCHAR *
ACE_Service_Type_Impl::name (void) const
{
  ACE_TRACE ("ACE_Service_Type_Impl::name");
  return this->name_;
}

ACE_INLINE void
ACE_Service_Type_Impl::name (const ACE_TCHAR *n)
{
  ACE_TRACE ("ACE_Service_Type_Impl::name");

  ACE::strdelete (const_cast<ACE_TCHAR*> (this->name_));
  this->name_ = ACE::strnew (n);
}
