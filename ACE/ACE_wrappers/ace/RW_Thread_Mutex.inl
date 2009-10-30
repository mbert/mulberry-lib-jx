/* -*- C++ -*- */
// RW_Thread_Mutex.inl,v 4.1 2003/08/04 03:53:52 dhinton Exp

ACE_INLINE int
ACE_RW_Thread_Mutex::tryacquire_write_upgrade (void)
{
// ACE_TRACE ("ACE_RW_Thread_Mutex::tryacquire_write_upgrade");
  return ACE_OS::rw_trywrlock_upgrade (&this->lock_);
}

ACE_INLINE
ACE_RW_Thread_Mutex::~ACE_RW_Thread_Mutex (void)
{
}
