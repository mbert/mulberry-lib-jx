/* -*- C++ -*- */
// Functor.cpp,v 4.6 2004/10/06 20:48:10 jtc Exp

// ============================================================================
//
// = LIBRARY
//    ace
//
// = FILENAME
//    Functor.cpp
//
// = DESCRIPTION
//    Non-inlinable method definitions for non-templatized classes
//    and template specializations implementing the GOF Command Pattern,
//    and STL-style functors.
//
// = AUTHOR
//    Chris Gill           <cdgill@cs.wustl.edu>
//
//    Based on Command Pattern implementations originally done by
//
//    Carlos O'Ryan        <coryan@cs.wustl.edu>  and
//    Douglas C. Schmidt   <schmidt@cs.wustl.edu> and
//    Sergio Flores-Gaitan <sergio@cs.wustl.edu>
//
//    and on STL-style functor implementations originally done by
//
//    Irfan Pyarali  <irfan@cs.wustl.edu>
//
// ============================================================================

#if !defined (ACE_FUNCTOR_C)
#define ACE_FUNCTOR_C

#include "ace/Functor_T.h"
#include "ace/Functor.h"

#if !defined (__ACE_INLINE__)
#include "ace/Functor.inl"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ace, Functor, "Functor.cpp,v 4.6 2004/10/06 20:48:10 jtc Exp")

ACE_Command_Base::~ACE_Command_Base (void)
{
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

#endif /* ACE_FUNCTOR_C */
