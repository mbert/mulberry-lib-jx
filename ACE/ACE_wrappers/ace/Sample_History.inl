// Sample_History.inl,v 4.2 2001/12/03 18:57:40 coryan Exp

ACE_INLINE int
ACE_Sample_History::sample (ACE_UINT64 value)
{
  if (this->sample_count_ >= this->max_samples_)
    return -1;

  this->samples_[this->sample_count_++] = value;
  return 0;
}

ACE_INLINE ACE_UINT64
ACE_Sample_History::get_sample (size_t i) const
{
  if (this->sample_count_ <= i)
    return 0;

  return this->samples_[i];
}
