/* -*- C++ -*- */
// Pagefile_Memory_Pool.inl,v 4.1 2004/10/11 18:46:56 jtc Exp

#if defined (ACE_WIN32)

ACE_INLINE size_t
ACE_Pagefile_Memory_Pool::round_to_chunk_size (size_t nbytes)
{
  return (nbytes + ACE_DEFAULT_PAGEFILE_POOL_CHUNK - 1)
          & (~(ACE_DEFAULT_PAGEFILE_POOL_CHUNK - 1));
}

ACE_INLINE size_t
ACE_Pagefile_Memory_Pool::round_to_page_size (size_t nbytes)
{
  return ACE::round_to_pagesize (static_cast<off_t> (nbytes));
}

ACE_INLINE int
ACE_Pagefile_Memory_Pool::sync (ssize_t, int)
{
  return 0;
}

ACE_INLINE int
ACE_Pagefile_Memory_Pool::sync (void *, size_t, int)
{
  return 0;
}

ACE_INLINE int
ACE_Pagefile_Memory_Pool::protect (ssize_t, int)
{
  return 0;
}

ACE_INLINE int
ACE_Pagefile_Memory_Pool::protect (void *, size_t, int)
{
  return 0;
}

ACE_INLINE void *
ACE_Pagefile_Memory_Pool::base_addr (void) const
{
  return 0;
}
#endif /* ACE_WIN32 */
