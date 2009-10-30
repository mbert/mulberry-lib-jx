#include "ace/Notification_Strategy.h"

#if !defined (__ACE_INLINE__)
#include "ace/Notification_Strategy.inl"
#endif /* __ACE_INLINE __ */

ACE_RCSID(ace, Strategies, "Notification_Strategy.cpp,v 4.1 2001/12/10 21:44:40 bala Exp")

ACE_Notification_Strategy::ACE_Notification_Strategy (ACE_Event_Handler *eh,
                                                      ACE_Reactor_Mask mask)
  : eh_ (eh),
    mask_ (mask)
{
}

ACE_Notification_Strategy::~ACE_Notification_Strategy (void)
{
}
