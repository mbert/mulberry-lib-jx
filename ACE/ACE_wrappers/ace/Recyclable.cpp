//Recyclable.cpp,v 4.2 2001/12/15 02:39:33 shuston Exp
#include "ace/Recyclable.h"


#if !defined (__ACE_INLINE__)
#include "ace/Recyclable.inl"
#endif /* __ACE_INLINE __ */

ACE_RCSID(ace, Recyclable, "Recyclable.cpp,v 4.2 2001/12/15 02:39:33 shuston Exp")


ACE_Recyclable::ACE_Recyclable (ACE_Recyclable_State initial_state)
  : recycle_state_ (initial_state)
{
}

ACE_Recyclable::~ACE_Recyclable (void)
{
}
