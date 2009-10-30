/******************************************************************************
 JTEUndoDrop16.h

	Interface for the JTEUndoDrop16 class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoDrop16
#define _H_JTEUndoDrop16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoBase16.h>

class JTEUndoDrop16 : public JTEUndoBase16
{
public:

	JTEUndoDrop16(JTextEditor16* te, const JIndex origIndex,
				const JIndex newIndex, const JSize length);

	virtual ~JTEUndoDrop16();

	virtual void	SetPasteLength(const JSize length);
	virtual void	Undo();

private:

	JIndex	itsOrigCaretLoc;
	JIndex	itsNewSelStart;
	JIndex	itsNewSelEnd;

private:

	// not allowed

	JTEUndoDrop16(const JTEUndoDrop16& source);
	const JTEUndoDrop16& operator=(const JTEUndoDrop16& source);
};

#endif
