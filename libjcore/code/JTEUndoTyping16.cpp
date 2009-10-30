/******************************************************************************
 JTEUndoTyping16.cpp

	Class to undo typing into a JTextEditor16 object.

	BASE CLASS = JTEUndoTextBase16

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoTyping16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTyping16::JTEUndoTyping16
	(
	JTextEditor16* te
	)
	:
	JTEUndoTextBase16(te)
{
	itsOrigStartIndex = te->GetInsertionIndex();
	itsLength         = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTyping16::~JTEUndoTyping16()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoTyping16::Undo()
{
	PrepareForUndo(itsOrigStartIndex, itsLength);
	JTEUndoTextBase16::Undo();
}

/******************************************************************************
 HandleCharacters

 ******************************************************************************/

void
JTEUndoTyping16::HandleCharacters
	(
	const JSize count
	)
{
	assert( IsActive() );

	itsLength += count;
}

/******************************************************************************
 HandleDelete

	Call this -before- deleting the character.

 ******************************************************************************/

void
JTEUndoTyping16::HandleDelete()
{
	assert( IsActive() );

	if (itsLength > 0)
		{
		itsLength--;
		}
	else
		{
		itsOrigStartIndex = (GetTE())->GetInsertionIndex();
		PrependToSave(itsOrigStartIndex);
		itsOrigStartIndex--;
		}
}

/******************************************************************************
 HandleFwdDelete

	Call this -before- deleting the character.

 ******************************************************************************/

void
JTEUndoTyping16::HandleFwdDelete()
{
	assert( IsActive() );

	AppendToSave((GetTE())->GetInsertionIndex());
}

/******************************************************************************
 HandleAutoIndentDelete

	Call this -before- deleting the characters.

 ******************************************************************************/

void
JTEUndoTyping16::HandleAutoIndentDelete
	(
	const JIndex firstChar,
	const JIndex lastChar
	)
{
	assert( IsActive() &&
			firstChar <= lastChar &&
			lastChar  <= itsOrigStartIndex + itsLength - 1 );

	for (JIndex i=lastChar; i>=firstChar; i--)
		{
		if (i >= itsOrigStartIndex)
			{
			itsLength--;
			}
		else
			{
			PrependToSave(i+1);		// saves the character before given index
			itsOrigStartIndex--;
			}
		}
}

/******************************************************************************
 Cast to JTEUndoTyping16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoTyping16*
JTEUndoTyping16::CastToJTEUndoTyping16()
{
	return this;
}

const JTEUndoTyping16*
JTEUndoTyping16::CastToJTEUndoTyping16()
	const
{
	return this;
}
