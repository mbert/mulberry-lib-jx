/******************************************************************************
 JTEUndoStyle.cpp

	Class to undo style change in a JTextEditor object.

	BASE CLASS = JTEUndoBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoStyle.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoStyle::JTEUndoStyle
	(
	JTextEditor* te
	)
	:
	JTEUndoBase(te)
{
	itsStartIndex = te->GetInsertionIndex();

	itsOrigStyles = new JRunArray<JTextEditor::Font>;
	assert( itsOrigStyles != NULL );

	JString selText;
	const JBoolean hasSelection = te->GetSelection(&selText, itsOrigStyles);
	assert( hasSelection );
}

JTEUndoStyle::JTEUndoStyle
	(
	JTextEditor* te,
	const JIndex range_start,
	const JIndex range_end
	)
	:
	JTEUndoBase(te)
{
	itsStartIndex = range_start;

	itsOrigStyles = new JRunArray<JTextEditor::Font>;
	assert( itsOrigStyles != NULL );

	JString selText;
	const JBoolean hasSelection = te->GetTextRange(&selText, itsOrigStyles, range_start, range_end);
	assert( hasSelection );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoStyle::~JTEUndoStyle()
{
	delete itsOrigStyles;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoStyle::Undo()
{
	JTextEditor* te = GetTE();
	te->SetSelection(itsStartIndex, itsStartIndex + itsOrigStyles->GetElementCount() - 1);

	JTEUndoStyle* newUndo = new JTEUndoStyle(te);
	assert( newUndo != NULL );

	te->SetFont(itsStartIndex, *itsOrigStyles, kJFalse);

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoStyle::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JTEUndoBase::SetFont(itsOrigStyles, name, size);
}

/******************************************************************************
 Cast to JTEUndoStyle*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoStyle*
JTEUndoStyle::CastToJTEUndoStyle()
{
	return this;
}

const JTEUndoStyle*
JTEUndoStyle::CastToJTEUndoStyle()
	const
{
	return this;
}
