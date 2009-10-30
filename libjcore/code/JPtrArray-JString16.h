/******************************************************************************
 JPtrArray-JString16.h

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPtrArray_JString16
#define _H_JPtrArray_JString16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#ifdef _TODO
#include <JString16PtrMap.h>
#endif
#include <JString16.h>

istream& operator>>(istream&, JPtrArray<JString16>&);
ostream& operator<<(ostream&, const JPtrArray<JString16>&);

#ifdef _TODO
istream& operator>>(istream&, JString16PtrMap<JString16>&);
ostream& operator<<(ostream&, const JString16PtrMap<JString16>&);
#endif

JBoolean JSameStrings(const JPtrArray<JString16>&, const JPtrArray<JString16>&,
					  const JBoolean caseSensitive);

JOrderedSetT::CompareResult
JCompareStringsCaseSensitive(JString16* const &, JString16* const &);

JOrderedSetT::CompareResult
JCompareStringsCaseInsensitive(JString16* const &, JString16* const &);

#endif
