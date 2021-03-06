// Intrusive_List.inl,v 4.1 2001/08/01 23:39:25 coryan Exp

template<class T> ACE_INLINE int
ACE_Intrusive_List<T>::empty (void) const
{
  return this->head_ == 0;
}

template<class T> ACE_INLINE T *
ACE_Intrusive_List<T>::head (void) const
{
  return this->head_;
}

template<class T> ACE_INLINE T *
ACE_Intrusive_List<T>::tail (void) const
{
  return this->tail_;
}
