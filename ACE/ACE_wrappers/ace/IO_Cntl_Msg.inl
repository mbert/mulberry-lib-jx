// -*- C++ -*-
// IO_Cntl_Msg.inl,v 4.1 2005/06/14 12:28:30 jwillemsen Exp

ACE_INLINE
ACE_IO_Cntl_Msg::ACE_IO_Cntl_Msg (ACE_IO_Cntl_Cmds c)
{
  this->cmd_ = c;
}

ACE_INLINE ACE_IO_Cntl_Msg::ACE_IO_Cntl_Cmds
ACE_IO_Cntl_Msg::cmd (void)
{
  return this->cmd_;
}

ACE_INLINE void
ACE_IO_Cntl_Msg::cmd (ACE_IO_Cntl_Cmds c)
{
  this->cmd_ = c;
}

ACE_INLINE size_t
ACE_IO_Cntl_Msg::count (void)
{
  return this->count_;
}

ACE_INLINE void
ACE_IO_Cntl_Msg::count (size_t c)
{
  this->count_ = c;
}

ACE_INLINE int
ACE_IO_Cntl_Msg::error (void)
{
  return this->error_;
}

ACE_INLINE void
ACE_IO_Cntl_Msg::error (int e)
{
  this->error_ = e;
}

ACE_INLINE int
ACE_IO_Cntl_Msg::rval (void)
{
  return this->rval_;
}

ACE_INLINE void
ACE_IO_Cntl_Msg::rval (int r)
{
  this->rval_ = r;
}

