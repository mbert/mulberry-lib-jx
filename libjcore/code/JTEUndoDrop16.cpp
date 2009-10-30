/******************************************************************************
 JTEUndoDrop16.cpp

	Class to undo dragging text from one place to another.

	BASE CLASS = JTEUndoBase16

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoDrop16.h>
#include <JTextEditor16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoDrop16::JTEUndoDrop16
	(
	JTextEditor16*	te,
	const JIndex	origIndex,
	const JIndex	newIndex,
	const JSize		length
	)
	:
	JTEUndoBase16(te)
{
	itsOrigCaretLoc = origIndex;
	itsNewSelStart  = newIndex;
	itsNewSelEnd    = itsNewSelStart + length - 1;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoDrop16::~JTEUndoDrop16()
{
}

/******************************************************************************
 SetPasteLength (virtual)

 ******************************************************************************/

void
JTEUndoDrop16::SetPasteLength
	(
	const JSize length
	)
{
	itsNewSelEnd = itsNewSelStart + length - 1;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoDrop16::Undo()
{
	JTextEditor16* te = GetTE();
	te->SetSelection(itsNewSelStart, itsNewSelEnd);
	te->DropSelection(itsOrigCaretLoc, kJFalse);			// deletes us
}
