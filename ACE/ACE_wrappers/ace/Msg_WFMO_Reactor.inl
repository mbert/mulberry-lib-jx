/* -*- C++ -*- */
// Msg_WFMO_Reactor.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

#if defined (ACE_WIN32) && !defined (ACE_LACKS_MSG_WFMO)

ACE_INLINE int
ACE_Msg_WFMO_Reactor::handle_events (ACE_Time_Value &how_long)
{
  return this->event_handling (&how_long, 0);
}

ACE_INLINE int
ACE_Msg_WFMO_Reactor::alertable_handle_events (ACE_Time_Value &how_long)
{
  return this->event_handling (&how_long, MWMO_ALERTABLE);
}

ACE_INLINE int
ACE_Msg_WFMO_Reactor::handle_events (ACE_Time_Value *how_long)
{
  return this->event_handling (how_long, 0);
}

ACE_INLINE int
ACE_Msg_WFMO_Reactor::alertable_handle_events (ACE_Time_Value *how_long)
{
  return this->event_handling (how_long, MWMO_ALERTABLE);
}

#endif /* ACE_WIN32 && !ACE_LACKS_MSG_WFMO */
