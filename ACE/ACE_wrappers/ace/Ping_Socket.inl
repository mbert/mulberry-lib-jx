// -*- C++ -*-
//
// Ping_Socket.inl,v 1.2 2005/05/19 22:52:41 shuston Exp

ACE_INLINE char *
ACE_Ping_Socket::icmp_recv_buff (void)
{
  return this->icmp_recv_buff_;
}
