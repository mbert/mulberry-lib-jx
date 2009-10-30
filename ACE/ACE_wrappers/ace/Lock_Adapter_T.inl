// -*- C++ -*-
//
// Lock_Adapter_T.inl,v 4.2 2005/05/31 01:11:28 ossama Exp

template <class ACE_LOCKING_MECHANISM>
ACE_INLINE
ACE_Lock_Adapter<ACE_LOCKING_MECHANISM>::ACE_Lock_Adapter (
  ACE_LOCKING_MECHANISM &lock)
  : lock_ (&lock),
    delete_lock_ (0)
{
}
