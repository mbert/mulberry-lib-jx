// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_loadavg.h
 *
 *  loadavg functions
 *
 *  os_loadavg.h,v 1.1 2004/08/25 07:06:20 jwillemsen Exp
 *
 *  @author Johnny Willemsen  <jwillemsen@remedy.nl>
 */
//=============================================================================

#ifndef ACE_OS_INCLUDE_SYS_OS_LOADAVG_H
#define ACE_OS_INCLUDE_SYS_OS_LOADAVG_H

#include /**/ "ace/pre.h"

#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_SYS_LOADAVG_H)
# include /**/ <sys/loadavg.h>
#endif /* ACE_HAS_SYS_LOADAVG_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include /**/ "ace/post.h"
#endif /* ACE_OS_INCLUDE_SYS_OS_LOADAVG_H */
