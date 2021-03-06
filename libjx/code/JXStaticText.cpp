/******************************************************************************
 JXStaticText.cpp

	Maintains a text string.  Note that the font changing routines are
	merely for convenience.  Full control over the styles is just a matter
	of using the JTextEditor functions.

	BASE CLASS = JXTEBase

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStaticText.h>
#include <jXConstants.h>
#include <strstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStaticText::JXStaticText
	(
	const JCharacter*	text,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase(kStaticText, JI2B(w==0), kJFalse, NULL,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	JXStaticTextX(text, w,h);
	SetBorderWidth(0);		// after margin width has been reduced
}

JXStaticText::JXStaticText
	(
	const JCharacter*	text,
	const JBoolean		wordWrap,
	const JBoolean		selectable,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTEBase((selectable ? kSelectableText : kStaticText),
			 JNegate(wordWrap), kJFalse, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y,
			 (w>0 ? w : 100), (h>0 ? h : 100))
{
	assert( !wordWrap || w > 0 );
	JXStaticTextX(text, w,h);
}

// private

void
JXStaticText::JXStaticTextX
	(
	const JCharacter*	text,
	const JCoordinate	origW,
	const JCoordinate	origH
	)
{
	SetDefaultFontSize(kJXDefaultFontSize);
	TESetLeftMarginWidth(kMinLeftMarginWidth);

	if (JCompareMaxN(text, "<html>", 6, kJFalse))
		{
		std::istrstream input(text, strlen(text));
		ReadHTML(input);
		}
	else
		{
		SetText(text);
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if (w == 0)
		{
		w = GetMinBoundsWidth() + 2*GetBorderWidth();
		SetSize(w, GetFrameHeight());
		}
	if (h == 0)
		{
		h = GetMinBoundsHeight() + 2*GetBorderWidth();
		SetSize(w, h);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStaticText::~JXStaticText()
{
}
