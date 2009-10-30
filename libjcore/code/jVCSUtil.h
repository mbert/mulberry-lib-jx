/******************************************************************************
 jVCSUtil.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jVCSUtil
#define _H_jVCSUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

enum JVCSType
{
	kJUnknownVCSType = 0,
	kJCVSType,
	kJSVNType,
	kJSCCSType
};

JBoolean	JIsVCSDirectory(const JCharacter* name);
JVCSType	JGetVCSType(const JCharacter* path);

void		JEditVCS(const JCharacter* fullName);
void		JRemoveVCS(const JCharacter* fullName);

// CVS specific

void		JUpdateCVSIgnore(const JCharacter* ignoreFullName);

#endif
