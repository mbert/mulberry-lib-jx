// Refcounted_Auto_Ptr.cpp,v 4.1 2005/05/31 01:11:28 ossama Exp

#ifndef ACE_REFCOUNTED_AUTO_PTR_CPP
#define ACE_REFCOUNTED_AUTO_PTR_CPP

#include "ace/Refcounted_Auto_Ptr.h"

template <class X, class ACE_LOCK>
ACE_Refcounted_Auto_Ptr<X, ACE_LOCK>::~ACE_Refcounted_Auto_Ptr (void)
{
  AUTO_REFCOUNTED_PTR_REP::detach (rep_);
}

#endif  /* !ACE_REFCOUNTED_AUTO_PTR_CPP */
