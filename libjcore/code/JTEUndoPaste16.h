/******************************************************************************
 JTEUndoPaste16.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoPaste16
#define _H_JTEUndoPaste16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoTextBase16.h>

class JTEUndoPaste16 : public JTEUndoTextBase16
{
public:

	JTEUndoPaste16(JTextEditor16* te, const JSize pasteLength);

	virtual ~JTEUndoPaste16();

	virtual void	SetPasteLength(const JSize length);
	virtual void	Undo();

protected:

	void	SetCurrentLength(const JSize length);

private:

	JIndex	itsOrigSelStart;
	JSize	itsLength;

private:

	// not allowed

	JTEUndoPaste16(const JTEUndoPaste16& source);
	const JTEUndoPaste16& operator=(const JTEUndoPaste16& source);
};


/******************************************************************************
 SetCurrentLength (protected)

 ******************************************************************************/

inline void
JTEUndoPaste16::SetCurrentLength
	(
	const JSize length
	)
{
	itsLength = length;
}

#endif
