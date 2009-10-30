/* -*- C++ -*- */
// Process_Manager.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE size_t
ACE_Process_Manager::managed (void) const
{
  return current_count_;
}
