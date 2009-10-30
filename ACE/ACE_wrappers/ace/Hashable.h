// -*- C++ -*-

//=============================================================================
/**
 *  @file   Hashable.h
 *
 *  Hashable.h,v 4.5 2003/07/19 19:04:11 dhinton Exp
 *
 *  @author Doug Schmidt
 */
//=============================================================================

#ifndef ACE_HASHABLE_H
#define ACE_HASHABLE_H

#include /**/ "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


/**
 * @class ACE_Hashable
 *
 * @brief ACE_Hashable
 */
class ACE_Export ACE_Hashable
{
public:

  /// Destructor.
  virtual ~ACE_Hashable (void);

  /// Computes and returns hash value.  This "caches" the hash value to
  /// improve performance.
  virtual unsigned long hash (void) const;

protected:
  /// Protected constructor.
  ACE_Hashable (void);

  /// This is the method that actually performs the non-cached hash
  /// computation.
  virtual unsigned long hash_i (void) const = 0;

protected:

  /// Pre-computed hash-value.
  unsigned long hash_value_;

};


#if defined (__ACE_INLINE__)
#include "ace/Hashable.inl"
#endif /* __ACE_INLINE __ */

#include /**/ "ace/post.h"

#endif /*ACE_HASHABLE_H*/
