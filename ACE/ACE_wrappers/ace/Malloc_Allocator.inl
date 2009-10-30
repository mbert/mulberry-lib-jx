/* -*- C++ -*- */
// Malloc_Allocator.inl,v 4.2 2004/10/08 00:40:05 jtc Exp

ACE_INLINE
ACE_Static_Allocator_Base::ACE_Static_Allocator_Base (char *buffer,
                                                      size_t size)
  : buffer_ (buffer),
    size_ (size),
    offset_ (0)
{
}
