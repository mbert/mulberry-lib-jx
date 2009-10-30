// -*- C++ -*-
//
// POSIX_Proactor.inl,v 4.2 2005/05/31 01:11:28 ossama Exp

ACE_INLINE 
ACE_Asynch_Pseudo_Task& ACE_POSIX_Proactor::get_asynch_pseudo_task (void)
{
  return this->pseudo_task_;
}
