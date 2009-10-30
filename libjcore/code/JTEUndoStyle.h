/******************************************************************************
 JTEUndoStyle.h

	Interface for the JTEUndoStyle class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoStyle
#define _H_JTEUndoStyle

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoBase.h>

class JTEUndoStyle : public JTEUndoBase
{
public:

	JTEUndoStyle(JTextEditor* te);
	JTEUndoStyle(JTextEditor* te, const JIndex range_start, const JIndex range_end);

	virtual ~JTEUndoStyle();

	virtual void	Undo();

	virtual void	SetFont(const JCharacter* name, const JSize size);

	// provides safe downcasting

	virtual JTEUndoStyle*		CastToJTEUndoStyle();
	virtual const JTEUndoStyle*	CastToJTEUndoStyle() const;

private:

	JIndex							itsStartIndex;
	JRunArray<JTextEditor::Font>*	itsOrigStyles;

private:

	// not allowed

	JTEUndoStyle(const JTEUndoStyle& source);
	const JTEUndoStyle& operator=(const JTEUndoStyle& source);
};

#endif
