/******************************************************************************
 JTEUndoBase16.h

	Copyright © 1996-2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoBase16
#define _H_JTEUndoBase16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUndo.h>
#include <JTextEditor16.h>	// need defintion of JTextEditor16::Font

class JTextEditor16;
class JTEUndoTyping16;
class JTEUndoStyle16;
class JTEUndoTabShift16;

class JTEUndoBase16 : public JUndo
{
public:

	JTEUndoBase16(JTextEditor16* te);

	virtual ~JTEUndoBase16();

	// called by JTextEditor16::SetAllFontNameAndSize()

	virtual void	SetFont(const JCharacter* name, const JSize size);

	// required by some derived classes

	virtual void	SetPasteLength(const JSize length);

	// provides safe downcasting

	virtual JTEUndoTyping16*			CastToJTEUndoTyping16();
	virtual const JTEUndoTyping16*		CastToJTEUndoTyping16() const;
	virtual JTEUndoStyle16*				CastToJTEUndoStyle16();
	virtual const JTEUndoStyle16*		CastToJTEUndoStyle16() const;
	virtual JTEUndoTabShift16*			CastToJTEUndoTabShift16();
	virtual const JTEUndoTabShift16*	CastToJTEUndoTabShift16() const;

protected:

	JTextEditor16*	GetTE() const;

	void	SetFont(JRunArray<JTextEditor16::Font>* styles,
					const JCharacter* name, const JSize size);

private:

	JTextEditor16*	itsTE;		// we don't own this

private:

	// not allowed

	JTEUndoBase16(const JTEUndoBase16& source);
	const JTEUndoBase16& operator=(const JTEUndoBase16& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JTextEditor16*
JTEUndoBase16::GetTE()
	const
{
	return itsTE;
}

#endif
