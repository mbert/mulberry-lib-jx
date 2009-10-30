// -*- C++ -*-

//=============================================================================
/**
 *  @file   Handle_Ops.h
 *
 *  Handle_Ops.h,v 1.8 2004/08/14 07:03:12 ossama Exp
 *
 *  This class consolidates the operations on the Handles.
 *
 *  @author Priyanka Gontla <pgontla@ece.uci.edu>
 */
//=============================================================================

#ifndef ACE_HANDLE_OPS_H
#define ACE_HANDLE_OPS_H

#include /**/ "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Global_Macros.h"

class ACE_Time_Value;

// = Operations on HANDLEs.
namespace ACE
{
  /**
   * Wait up to @a timeout> amount of time to actively open a device.
   * This method doesn't perform the @c connect, it just does the
   * timed wait.
   */
  extern ACE_Export ACE_HANDLE handle_timed_open (
    ACE_Time_Value *timeout,
    const ACE_TCHAR *name,
    int flags,
    int perms,
    LPSECURITY_ATTRIBUTES sa = 0);
}

#include /**/ "ace/post.h"

#endif /* ACE_HANDLE_OPS_H */
