/******************************************************************************
 JTEUndoPaste16.cpp

	Class to undo paste into a JTextEditor16 object.

	BASE CLASS = JTEUndoTextBase16

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoPaste16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoPaste16::JTEUndoPaste16
	(
	JTextEditor16*	te,
	const JSize		pasteLength
	)
	:
	JTEUndoTextBase16(te)
{
	itsOrigSelStart = te->GetInsertionIndex();
	itsLength       = pasteLength;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoPaste16::~JTEUndoPaste16()
{
}

/******************************************************************************
 SetPasteLength (virtual)

 ******************************************************************************/

void
JTEUndoPaste16::SetPasteLength
	(
	const JSize length
	)
{
	itsLength = length;
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoPaste16::Undo()
{
	PrepareForUndo(itsOrigSelStart, itsLength);
	JTEUndoTextBase16::Undo();
}
