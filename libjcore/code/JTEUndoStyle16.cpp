/******************************************************************************
 JTEUndoStyle16.cpp

	Class to undo style change in a JTextEditor16 object.

	BASE CLASS = JTEUndoBase16

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoStyle16.h>
#include <JString16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoStyle16::JTEUndoStyle16
	(
	JTextEditor16* te
	)
	:
	JTEUndoBase16(te)
{
	itsStartIndex = te->GetInsertionIndex();

	itsOrigStyles = new JRunArray<JTextEditor16::Font>;
	assert( itsOrigStyles != NULL );

	JString16 selText;
	const JBoolean hasSelection = te->GetSelection(&selText, itsOrigStyles);
	assert( hasSelection );
}

JTEUndoStyle16::JTEUndoStyle16
	(
	JTextEditor16* te,
	const JIndex range_start,
	const JIndex range_end
	)
	:
	JTEUndoBase16(te)
{
	itsStartIndex = range_start;

	itsOrigStyles = new JRunArray<JTextEditor16::Font>;
	assert( itsOrigStyles != NULL );

	JString16 selText;
	const JBoolean hasSelection = te->GetTextRange(&selText, itsOrigStyles, range_start, range_end);
	assert( hasSelection );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoStyle16::~JTEUndoStyle16()
{
	delete itsOrigStyles;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoStyle16::Undo()
{
	JTextEditor16* te = GetTE();
	te->SetSelection(itsStartIndex, itsStartIndex + itsOrigStyles->GetElementCount() - 1);

	JTEUndoStyle16* newUndo = new JTEUndoStyle16(te);
	assert( newUndo != NULL );

	te->SetFont(itsStartIndex, *itsOrigStyles, kJFalse);

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor16::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoStyle16::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JTEUndoBase16::SetFont(itsOrigStyles, name, size);
}

/******************************************************************************
 Cast to JTEUndoStyle16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoStyle16*
JTEUndoStyle16::CastToJTEUndoStyle16()
{
	return this;
}

const JTEUndoStyle16*
JTEUndoStyle16::CastToJTEUndoStyle16()
	const
{
	return this;
}
