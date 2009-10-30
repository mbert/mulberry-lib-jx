/* -*- C++ -*- */
// Read_Buffer.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Accessor to the number of bytes in the buffer.

ACE_INLINE size_t
ACE_Read_Buffer::size (void) const
{
  ACE_TRACE ("ACE_Read_Buffer::size");
  return this->size_;
}

// The total number of characters replaced.

ACE_INLINE size_t
ACE_Read_Buffer::replaced (void) const
{
  ACE_TRACE ("ACE_Read_Buffer::replaced");
  return this->occurrences_;
}

ACE_INLINE ACE_Allocator *
ACE_Read_Buffer::alloc (void) const
{
  ACE_TRACE ("ACE_Read_Buffer::alloc");
  return this->allocator_;
}

