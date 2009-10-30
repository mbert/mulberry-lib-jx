/**
 * @file Swap.inl
 *
 * Swap.inl,v 1.1 2002/02/27 09:15:31 coryan Exp
 *
 * @author Carlos O'Ryan <coryan@uci.edu>
 */

template<class T> ACE_INLINE void
ACE_Swap<T>::swap (T &lhs, T& rhs)
{
  T tmp = lhs;
  lhs = rhs;
  rhs = tmp;
}
