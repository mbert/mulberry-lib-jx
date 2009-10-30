/******************************************************************************
 JTEUndoBase16.cpp

	Base class to support undoing any operation on a JTextEditor16 object.

	BASE CLASS = JUndo

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTEUndoBase16.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTEUndoBase16::JTEUndoBase16
	(
	JTextEditor16* te
	)
	:
	JUndo()
{
	itsTE = te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTEUndoBase16::~JTEUndoBase16()
{
}

/******************************************************************************
 SetFont (virtual)

	Called by JTextEditor16::SetAllFontNameAndSize().

 ******************************************************************************/

void
JTEUndoBase16::SetFont
	(
	const JCharacter*	name,
	const JSize			size
	)
{
}

/******************************************************************************
 SetFont (protected)

	Convenience function for derived classes that need to implement SetFont().

 ******************************************************************************/

void
JTEUndoBase16::SetFont
	(
	JRunArray<JTextEditor16::Font>*	styles,
	const JCharacter*				name,
	const JSize						size
	)
{
	const JFontManager* fontMgr = itsTE->TEGetFontManager();

	const JSize runCount = styles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		JTextEditor16::Font f = styles->GetRunData(i);
		f.size = size;
		f.id   = fontMgr->GetFontID(name, f.size, f.style);
		styles->SetRunData(i, f);
		}
}

/******************************************************************************
 SetPasteLength (virtual)

	Required by some derived classes.

 ******************************************************************************/

void
JTEUndoBase16::SetPasteLength
	(
	const JSize length
	)
{
	assert( 0 /* programmer forgot to override JTEUndoBase16::SetPasteLength */ );
}

/******************************************************************************
 Cast to JTEUndoTyping16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoTyping16*
JTEUndoBase16::CastToJTEUndoTyping16()
{
	return NULL;
}

const JTEUndoTyping16*
JTEUndoBase16::CastToJTEUndoTyping16()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JTEUndoStyle16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoStyle16*
JTEUndoBase16::CastToJTEUndoStyle16()
{
	return NULL;
}

const JTEUndoStyle16*
JTEUndoBase16::CastToJTEUndoStyle16()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to JTEUndoTabShift16*

	Not inline because they are virtual

 ******************************************************************************/

JTEUndoTabShift16*
JTEUndoBase16::CastToJTEUndoTabShift16()
{
	return NULL;
}

const JTEUndoTabShift16*
JTEUndoBase16::CastToJTEUndoTabShift16()
	const
{
	return NULL;
}
