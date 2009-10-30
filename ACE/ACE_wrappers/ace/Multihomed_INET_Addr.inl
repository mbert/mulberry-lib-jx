/* -*- C++ -*- */
// Multihomed_INET_Addr.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Accessor implementations
ACE_INLINE
size_t
ACE_Multihomed_INET_Addr::get_num_secondary_addresses() const
{
  return this->secondaries_.size();
}
