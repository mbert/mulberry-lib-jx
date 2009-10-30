/* -*- C++ -*- */
// Thread_Adapter.inl,v 4.4 2004/06/14 13:58:42 jwillemsen Exp

ACE_INLINE ACE_Thread_Manager *
ACE_Thread_Adapter::thr_mgr (void)
{
  return this->thr_mgr_;
}
