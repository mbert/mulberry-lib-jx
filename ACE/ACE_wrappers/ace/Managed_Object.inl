/* -*- C++ -*- */
// Managed_Object.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

template <class TYPE>
ACE_INLINE
ACE_Cleanup_Adapter<TYPE>::ACE_Cleanup_Adapter (void)
  // Note: don't explicitly initialize "object_", because TYPE may not
  // have a default constructor.  Let the compiler figure it out . . .
{
}

template <class TYPE>
ACE_INLINE
TYPE &
ACE_Cleanup_Adapter<TYPE>::object (void)
{
  return this->object_;
}
