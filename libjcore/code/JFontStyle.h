/******************************************************************************
 JFontStyle.h

	Style information for drawing text.

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFontStyle
#define _H_JFontStyle

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <jColor.h>

typedef unsigned long	JFontID;

#ifdef _J_USE_XFT
const JSize kJDefaultFontSize = 8;
#else
const JSize kJDefaultFontSize = 8;
#endif

struct JFontStyle
{
private:

	static JColorIndex itsDefaultColorIndex;
	static JColorIndex itsDefaultBackColorIndex;

public:

	enum EUnderlineType
	{
		plain_Underline = 0,
		redwavy_Underline = 1
	};

	JBoolean			bold;
	JBoolean			italic;
	JSize				underlineCount;		// may change font height
	EUnderlineType		underlineType;
	JBoolean			strike;
	JColorIndex			color;
	JColorIndex			backcolor;

	JFontStyle()
		:
		bold(kJFalse),
		italic(kJFalse),
		underlineCount(0),
		underlineType(plain_Underline),
		strike(kJFalse),
		color(itsDefaultColorIndex),
		backcolor(itsDefaultBackColorIndex)
	{ };

	JFontStyle
		(
		const JBoolean		b,
		const JBoolean		i,
		const JSize			u,
		const JBoolean		s,
		const JColorIndex	c = JFontStyle::itsDefaultColorIndex
		)
		:
		bold(b),
		italic(i),
		underlineCount(u),
		underlineType(plain_Underline),
		strike(s),
		color(c),
		backcolor(itsDefaultBackColorIndex)
	{ };

	JFontStyle
		(
		const JColorIndex c
		)
		:
		bold(kJFalse),
		italic(kJFalse),
		underlineCount(0),
		underlineType(plain_Underline),
		strike(kJFalse),
		color(c),
		backcolor(itsDefaultBackColorIndex)
	{ };

	static void
	SetDefaultColorIndex
		(
		const JColorIndex c
		)
	{
		itsDefaultColorIndex = c;
	};

	static void
	SetDefaultBackColorIndex
		(
		const JColorIndex c
		)
	{
		itsDefaultBackColorIndex = c;
	};
};

int operator==(const JFontStyle& s1, const JFontStyle& s2);

inline int
operator!=
	(
	const JFontStyle& s1,
	const JFontStyle& s2
	)
{
	return !(s1 == s2);
}

#endif
