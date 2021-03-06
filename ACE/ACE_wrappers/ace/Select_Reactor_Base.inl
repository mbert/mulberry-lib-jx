/* -*- C++ -*- */
// Select_Reactor_Base.inl,v 4.3 2005/05/01 08:16:08 ossama Exp

#include "ace/Reactor.h"

ACE_INLINE
ACE_Event_Tuple::~ACE_Event_Tuple (void)
{
}

ACE_INLINE
ACE_Select_Reactor_Handler_Repository::~ACE_Select_Reactor_Handler_Repository (void)
{
}

ACE_INLINE
ACE_Select_Reactor_Handler_Repository_Iterator::~ACE_Select_Reactor_Handler_Repository_Iterator (void)
{
}

ACE_INLINE size_t
ACE_Select_Reactor_Handler_Repository::size (void) const
{
  return this->max_size_;
}

ACE_INLINE
ACE_Event_Tuple::ACE_Event_Tuple (void)
:  handle_ (ACE_INVALID_HANDLE),
   event_handler_ (0)
{
}

ACE_INLINE
ACE_Event_Tuple::ACE_Event_Tuple (ACE_Event_Handler* eh,
                                  ACE_HANDLE h)
: handle_ (h),
  event_handler_ (eh)
{
}

ACE_INLINE bool
ACE_Event_Tuple::operator== (const ACE_Event_Tuple &rhs) const
{
  return this->handle_ == rhs.handle_;
}

ACE_INLINE bool
ACE_Event_Tuple::operator!= (const ACE_Event_Tuple &rhs) const
{
  return !(*this == rhs);
}

ACE_INLINE
ACE_Select_Reactor_Impl::ACE_Select_Reactor_Impl (bool ms)
  : handler_rep_ (*this)
  , timer_queue_ (0)
  , delete_timer_queue_ (0)
  , delete_signal_handler_ (0)
  , delete_notify_handler_ (0)
  , requeue_position_ (-1) // Requeue at end of waiters by default.
  , initialized_ (0)
  , state_changed_ (0)
  , mask_signals_ (ms)
  , supress_renew_ (0)
{
}

ACE_INLINE int
ACE_Select_Reactor_Impl::supress_notify_renew (void)
{
  return this->supress_renew_;
}

ACE_INLINE void
ACE_Select_Reactor_Impl::supress_notify_renew (int sr)
{
  this->supress_renew_ = sr;
}
