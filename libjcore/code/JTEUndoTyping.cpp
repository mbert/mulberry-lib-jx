/******************************************************************************
 JTEUndoTyping.cpp

	Class to undo typing into a JTextEditor object.

	BASE CLASS = JTEUndoTextBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoTyping.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTyping::JTEUndoTyping
	(
	JTextEditor* te
	)
	:
	JTEUndoTextBase(te)
{
	itsOrigStartIndex = te->GetInsertionIndex();
	itsLength         = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTyping::~JTEUndoTyping()
{
}

/******************************************************************************
 Undo (virtual)

 ******************************************************************************/

void
JTEUndoTyping::Undo()
{
	PrepareForUndo(itsOrigStartIndex, itsLength);
	JTEUndoTextBase::Undo();
}

/******************************************************************************
 HandleCharacters

 ******************************************************************************/

void
JTEUndoTyping::HandleCharacters
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
JTEUndoTyping::HandleDelete()
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
JTEUndoTyping::HandleFwdDelete()
{
	assert( IsActive() );

	AppendToSave((GetTE())->GetInsertionIndex());
}

/******************************************************************************
 HandleAutoIndentDelete

	Call this -before- deleting the characters.

 ******************************************************************************/

void
JTEUndoTyping::HandleAutoIndentDelete
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
 Cast to JTEUndoTyping*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoTyping*
JTEUndoTyping::CastToJTEUndoTyping()
{
	return this;
}

const JTEUndoTyping*
JTEUndoTyping::CastToJTEUndoTyping()
	const
{
	return this;
}
