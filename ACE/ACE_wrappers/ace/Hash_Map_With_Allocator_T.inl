/* -*- C++ -*- */
// Hash_Map_With_Allocator_T.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Hash_Map_With_Allocator_T.i


template <class EXT_ID, class INT_ID> ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::close (ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::close");
  this->allocator_ = alloc;
  return this->close_i ();
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::bind (const EXT_ID &ext_id,
                                                   const INT_ID &int_id,
                                                   ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::bind");
  this->allocator_ = alloc;
  return this->bind_i (ext_id, int_id);
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::unbind (const EXT_ID &ext_id,
                                                     INT_ID &int_id,
                                                     ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::unbind");
  this->allocator_ = alloc;
  return this->unbind_i (ext_id, int_id);
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::unbind (const EXT_ID &ext_id,
                                                     ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID>::unbind");
  this->allocator_ = alloc;
  return this->unbind_i (ext_id);
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::rebind (const EXT_ID &ext_id,
                                                     const INT_ID &int_id,
                                                     EXT_ID &old_ext_id,
                                                     INT_ID &old_int_id,
                                                     ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::rebind");
  this->allocator_ = alloc;
  return this->rebind_i (ext_id, int_id, old_ext_id, old_int_id);
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::find (const EXT_ID &ext_id,
                                                   INT_ID &int_id,
                                                   ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::find");
  this->allocator_ = alloc;
  return this->find_i (ext_id, int_id);
}

template <class EXT_ID, class INT_ID>  ACE_INLINE int
ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::find (const EXT_ID &ext_id,
                                                   ACE_Allocator *alloc)
{
  ACE_TRACE ("ACE_Hash_Map_With_Allocator<EXT_ID, INT_ID>::find");
  this->allocator_ = alloc;
  return this->find_i (ext_id);
}
