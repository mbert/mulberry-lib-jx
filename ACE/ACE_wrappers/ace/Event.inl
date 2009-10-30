/* -*- C++ -*- */
// Event.inl,v 4.1 2003/08/04 03:53:51 dhinton Exp

ACE_INLINE ACE_event_t
ACE_Event::handle (void) const
{
  return this->handle_;
}

ACE_INLINE void
ACE_Event::handle (ACE_event_t new_handle)
{
  this->handle_ = new_handle;
}

