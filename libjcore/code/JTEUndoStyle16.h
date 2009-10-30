/******************************************************************************
 JTEUndoStyle16.h

	Interface for the JTEUndoStyle16 class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoStyle16
#define _H_JTEUndoStyle16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoBase16.h>

class JTEUndoStyle16 : public JTEUndoBase16
{
public:

	JTEUndoStyle16(JTextEditor16* te);
	JTEUndoStyle16(JTextEditor16* te, const JIndex range_start, const JIndex range_end);

	virtual ~JTEUndoStyle16();

	virtual void	Undo();

	virtual void	SetFont(const JCharacter* name, const JSize size);

	// provides safe downcasting

	virtual JTEUndoStyle16*			CastToJTEUndoStyle16();
	virtual const JTEUndoStyle16*	CastToJTEUndoStyle16() const;

private:

	JIndex							itsStartIndex;
	JRunArray<JTextEditor16::Font>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoStyle16(const JTEUndoStyle16& source);
	const JTEUndoStyle16& operator=(const JTEUndoStyle16& source);
};

#endif
