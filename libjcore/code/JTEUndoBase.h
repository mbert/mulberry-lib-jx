/******************************************************************************
 JTEUndoBase.h

	Copyright � 1996-2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoBase
#define _H_JTEUndoBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUndo.h>
#include <JTextEditor.h>	// need defintion of JTextEditor::Font

class JTextEditor;
class JTEUndoTyping;
class JTEUndoStyle;
class JTEUndoTabShift;

class JTEUndoBase : public JUndo
{
public:

	JTEUndoBase(JTextEditor* te);

	virtual ~JTEUndoBase();

	// called by JTextEditor::SetAllFontNameAndSize()

	virtual void	SetFont(const JCharacter* name, const JSize size);

	// required by some derived classes

	virtual void	SetPasteLength(const JSize length);

	// provides safe downcasting

	virtual JTEUndoTyping*			CastToJTEUndoTyping();
	virtual const JTEUndoTyping*	CastToJTEUndoTyping() const;
	virtual JTEUndoStyle*			CastToJTEUndoStyle();
	virtual const JTEUndoStyle*		CastToJTEUndoStyle() const;
	virtual JTEUndoTabShift*		CastToJTEUndoTabShift();
	virtual const JTEUndoTabShift*	CastToJTEUndoTabShift() const;

protected:

	JTextEditor*	GetTE() const;

	void	SetFont(JRunArray<JTextEditor::Font>* styles,
					const JCharacter* name, const JSize size);

private:

	JTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JTEUndoBase(const JTEUndoBase& source);
	const JTEUndoBase& operator=(const JTEUndoBase& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JTextEditor*
JTEUndoBase::GetTE()
	const
{
	return itsTE;
}

#endif
