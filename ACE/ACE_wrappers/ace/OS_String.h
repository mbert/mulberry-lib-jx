// -*- C++ -*-
//=============================================================================
/**
 *  @file   OS_String.h
 *
 *  OS_String.h,v 4.32 2004/06/19 12:47:48 jwillemsen Exp
 *
 *  @brief  Contains definition of class ACE_OS_String.
 *
 *  @author Doug Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller <stophph@diku.dk>
 *  @author and a cast of thousands...
 */
//=============================================================================

#ifndef ACE_OS_STRING_H
#define ACE_OS_STRING_H
#include /**/ "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/OS_NS_string.h"
#include "ace/OS_NS_strings.h"
#include "ace/OS_NS_wchar.h"
#include "ace/OS_NS_ctype.h"

# if defined (ACE_HAS_INLINED_OSCALLS)
#   if defined (ACE_INLINE)
#     undef ACE_INLINE
#   endif /* ACE_INLINE */
#   define ACE_INLINE inline
#   include "ace/OS_String.inl"
# endif /* ACE_HAS_INLINED_OSCALLS */

#include /**/ "ace/post.h"
#endif /* ACE_OS_STRING_H */
