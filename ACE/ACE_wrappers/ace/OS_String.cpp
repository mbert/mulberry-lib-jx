//=============================================================================
/**
 *  @file   OS_String.cpp
 *
 *  OS_String.cpp,v 4.43 2003/11/01 11:15:16 dhinton Exp
 *
 *  @brief  Contains definitions for class ACE_OS_String.
 */
//=============================================================================

#include "ace/OS_String.h"
#include "ace/OS_Memory.h"

ACE_RCSID (ace, OS_String, "OS_String.cpp,v 4.43 2003/11/01 11:15:16 dhinton Exp")

#if !defined (ACE_HAS_INLINED_OSCALLS)
# include "ace/OS_String.inl"
#endif /* ACE_HAS_INLINED_OS_CALLS */

#if defined (ACE_LACKS_WCSDUP_PROTOTYPE)
extern "C" wchar_t *wcsdup __P ((__const wchar_t *__s));
#endif /* ACE_LACKS_WCSDUP_PROTOTYPE */

