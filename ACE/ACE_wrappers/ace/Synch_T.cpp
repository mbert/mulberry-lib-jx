// Synch_T.cpp,v 4.62 2004/06/14 13:58:42 jwillemsen Exp

#ifndef ACE_SYNCH_T_C
#define ACE_SYNCH_T_C

#include "ace/Thread.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// FUZZ: disable check_for_synch_include
#include "ace/Synch_T.h"
#include "ace/Log_Msg.h"

ACE_RCSID(ace, Synch_T, "Synch_T.cpp,v 4.62 2004/06/14 13:58:42 jwillemsen Exp")

#include "ace/Lock_Adapter_T.cpp"
#include "ace/Reverse_Lock_T.cpp"
#include "ace/Guard_T.cpp"
#include "ace/TSS_T.cpp"
#include "ace/Condition_T.cpp"

#endif /* ACE_SYNCH_T_C */
