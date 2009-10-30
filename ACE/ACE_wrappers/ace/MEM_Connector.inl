/* -*- C++ -*- */
// MEM_Connector.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// MEM_Connector.i

// Establish a connection.

ACE_INLINE ACE_MEM_IO::Signal_Strategy
ACE_MEM_Connector::preferred_strategy (void) const
{
  return this->preferred_strategy_;
}

ACE_INLINE void
ACE_MEM_Connector::preferred_strategy (ACE_MEM_IO::Signal_Strategy strategy)
{
  this->preferred_strategy_ = strategy;
}

ACE_INLINE ACE_MEM_SAP::MALLOC_OPTIONS &
ACE_MEM_Connector::malloc_options (void)
{
  // @@ This function has been deprecated and will be removed in the
  // future.

  return this->malloc_options_;
}
