/******************************************************************************
 JTEUndoTextBase16.h

	Interface for the JTEUndoTextBase16 class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoTextBase16
#define _H_JTEUndoTextBase16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoBase16.h>

class JTEUndoTextBase16 : public JTEUndoBase16
{
public:

	JTEUndoTextBase16(JTextEditor16* te);

	virtual ~JTEUndoTextBase16();

	virtual void	Undo();

	virtual void	SetFont(const JCharacter* name, const JSize size);

protected:

	void	PrepareForUndo(const JIndex start, const JSize length);

	void	PrependToSave(const JIndex index);
	void	AppendToSave(const JIndex index);

private:

	JString16*						itsOrigBuffer;
	JRunArray<JTextEditor16::Font>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoTextBase16(const JTEUndoTextBase16& source);
	const JTEUndoTextBase16& operator=(const JTEUndoTextBase16& source);
};

#endif
