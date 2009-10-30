/******************************************************************************
 JTEUndoTyping16.h

	Interface for the JTEUndoTyping16 class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoTyping16
#define _H_JTEUndoTyping16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoTextBase16.h>

class JTEUndoTyping16 : public JTEUndoTextBase16
{
public:

	JTEUndoTyping16(JTextEditor16* te);

	virtual ~JTEUndoTyping16();

	virtual void	Undo();

	void	HandleCharacter();
	void	HandleDelete();
	void	HandleFwdDelete();

	void	HandleCharacters(const JSize count);
	void	HandleAutoIndentDelete(const JIndex firstChar, const JIndex lastChar);

	// provides safe downcasting

	virtual JTEUndoTyping16*		CastToJTEUndoTyping16();
	virtual const JTEUndoTyping16*	CastToJTEUndoTyping16() const;

private:

	JIndex	itsOrigStartIndex;
	JSize	itsLength;

private:

	// not allowed

	JTEUndoTyping16(const JTEUndoTyping16& source);
	const JTEUndoTyping16& operator=(const JTEUndoTyping16& source);
};


/******************************************************************************
 HandleCharacter

 ******************************************************************************/

inline void
JTEUndoTyping16::HandleCharacter()
{
	HandleCharacters(1);
}

#endif
