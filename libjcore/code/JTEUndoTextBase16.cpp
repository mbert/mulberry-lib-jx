/******************************************************************************
 JTEUndoTextBase16.cpp

	Base class to support undoing any operation on a JTextEditor16 object
	that starts by deleting some text.

	BASE CLASS = JTEUndoBase16

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoTextBase16.h>
#include <JTEUndoPaste16.h>
#include <JString16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTextBase16::JTEUndoTextBase16
	(
	JTextEditor16* te
	)
	:
	JTEUndoBase16(te)
{
	itsOrigBuffer = new JString16;
	assert( itsOrigBuffer != NULL );

	itsOrigStyles = new JRunArray<JTextEditor16::Font>;
	assert( itsOrigStyles != NULL );

	te->GetSelection(itsOrigBuffer, itsOrigStyles);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTextBase16::~JTEUndoTextBase16()
{
	delete itsOrigBuffer;
	delete itsOrigStyles;
}

/******************************************************************************
 PrepareForUndo (protected)

	Call this to select the text to remove before calling Undo().

 ******************************************************************************/

void
JTEUndoTextBase16::PrepareForUndo
	(
	const JIndex	start,
	const JSize		length
	)
{
	JTextEditor16* te = GetTE();
	if (length == 0)
		{
		te->SetCaretLocation(start);
		}
	else
		{
		te->SetSelection(start, start + length - 1);
		}
}

/******************************************************************************
 Undo (virtual)

	Call this to put back the original text after selecting the new text.

 ******************************************************************************/

void
JTEUndoTextBase16::Undo()
{
	JTextEditor16* te = GetTE();

	JTEUndoPaste16* newUndo = new JTEUndoPaste16(te, itsOrigBuffer->GetLength());
	assert( newUndo != NULL );

	const JIndex selStart   = te->GetInsertionIndex();
	const JSize pasteLength = te->PrivatePaste(*itsOrigBuffer, itsOrigStyles);
	assert( pasteLength == itsOrigBuffer->GetLength() );

	if (!itsOrigBuffer->IsEmpty())
		{
		te->SetSelection(selStart, selStart + itsOrigBuffer->GetLength() - 1);
		}
	else
		{
		te->SetCaretLocation(selStart);
		}

	te->ReplaceUndo(this, newUndo);		// deletes us
}

/******************************************************************************
 PrependToSave (protected)

	Prepend the character in front of index to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase16::PrependToSave
	(
	const JIndex index
	)
{
	assert( index > 1 );

	JTextEditor16* te = GetTE();

	const JString16& text = te->GetText16();
	const JCharacter c  = text.GetCharacter(index-1);

	const JTextEditor16::Font f = te->GetFont(index-1);

	itsOrigBuffer->PrependCharacter(c);
	itsOrigStyles->PrependElement(f);
}

/******************************************************************************
 AppendToSave (protected)

	Append the character at index to the save buffer.

 ******************************************************************************/

void
JTEUndoTextBase16::AppendToSave
	(
	const JIndex index
	)
{
	JTextEditor16* te = GetTE();

	const JString16& text = te->GetText16();
	const JCharacter c  = text.GetCharacter(index);

	const JTextEditor16::Font f = te->GetFont(index);

	itsOrigBuffer->AppendCharacter(c);
	itsOrigStyles->AppendElement(f);
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor16::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoTextBase16::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
	JTEUndoBase16::SetFont(itsOrigStyles, name, size);
}
