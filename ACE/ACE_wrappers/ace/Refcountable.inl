/* -*- C++ -*- */
//Refcountable.inl,v 4.2 2005/07/28 08:26:46 ossama Exp
ACE_INLINE
ACE_Refcountable::ACE_Refcountable (int refcount)
  : refcount_ (refcount)
{
}

ACE_INLINE int
ACE_Refcountable::increment (void)
{
  return ++this->refcount_;
}

ACE_INLINE int
ACE_Refcountable::decrement (void)
{
  return --this->refcount_;
}

ACE_INLINE int
ACE_Refcountable::refcount (void) const
{
  return this->refcount_;
}
