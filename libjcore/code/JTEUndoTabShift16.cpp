/******************************************************************************
 JTEUndoTabShift16.cpp

	Class to undo indenting a block of text.  We have to use JTEUndoPaste16
	to get back the original pattern of spaces and tabs.

	BASE CLASS = JTEUndoPaste16

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoTabShift16.h>
#include <JTextEditor16.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoTabShift16::JTEUndoTabShift16
	(
	JTextEditor16* te
	)
	:
	JTEUndoPaste16(te, 0)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoTabShift16::~JTEUndoTabShift16()
{
}

/******************************************************************************
 UpdateEndChar

	Call this after SetSelection() to update the end of the affected text.
	(We can't store a line number, because this may change when word
	 wrapping is on.)

 ******************************************************************************/

void
JTEUndoTabShift16::UpdateEndChar()
{
	Activate();		// cancel SetSelection()

	JIndexRange r;
	const JBoolean ok = (GetTE())->GetSelection(&r);
	assert( ok );
	SetCurrentLength(r.GetLength());
}

/******************************************************************************
 Cast to JTEUndoTabShift16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoTabShift16*
JTEUndoTabShift16::CastToJTEUndoTabShift16()
{
	return this;
}

const JTEUndoTabShift16*
JTEUndoTabShift16::CastToJTEUndoTabShift16()
	const
{
	return this;
}
