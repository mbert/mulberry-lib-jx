/* -*- C++ -*- */
// Reactor_Notification_Strategy.inl,v 4.3 2004/06/14 13:58:41 jwillemsen Exp

ACE_INLINE ACE_Reactor *
ACE_Reactor_Notification_Strategy::reactor (void)
{
  return this->reactor_;
}

ACE_INLINE void
ACE_Reactor_Notification_Strategy::reactor (ACE_Reactor *r)
{
  this->reactor_ = r;
}
