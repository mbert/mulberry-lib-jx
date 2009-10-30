// -*- C++ -*-
// Timeprobe.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

ACE_INLINE bool
ACE_Event_Descriptions::operator== (const ACE_Event_Descriptions &rhs) const
{
  return this->minimum_id_ == rhs.minimum_id_ &&
    this->descriptions_ == rhs.descriptions_;
}
