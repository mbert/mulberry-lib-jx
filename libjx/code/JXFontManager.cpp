/******************************************************************************
 JXFontManager.cpp

	BASE CLASS = JFontManager

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFontManager.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <JPtrArray-JString.h>
#include <JOrderedSetUtil.h>
#include <JMinMax.h>
#include <JString16.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <JRegex.h>
#include <sstream>
#include <string.h>
#include <ctype.h>
#include <jAssert.h>

#define QUERY_FOR_MONOSPACE	0	// boolean

static const JCharacter* kItalicStr  = "-i";
static const JCharacter* kObliqueStr = "-o";

static JRegex nxmRegex = "^[0-9]+x[0-9]+$";		// non-const for SetMatchOnly()

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFontManager::JXFontManager
	(
	JXDisplay*	display,
	JXColormap*	colormap
	)
	:
	JFontManager()
{
	itsDisplay = display;

	itsFontList = new JArray<FontInfo>;
	assert( itsFontList != NULL );

	itsAllFontNames  = NULL;
	itsMonoFontNames = NULL;

	JFontStyle::SetDefaultColorIndex(colormap->GetBlackColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFontManager::~JXFontManager()
{
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FontInfo info = itsFontList->GetElement(i);
		delete info.name;
#ifdef _J_USE_XFT
		XftFontClose(*itsDisplay, info.xftfont);
#else
		XFreeFont(*itsDisplay, info.xfont);
#endif
		}
	delete itsFontList;

	delete itsAllFontNames;
	delete itsMonoFontNames;
}

/******************************************************************************
 GetFontNames (virtual)

	Returns the subset of available fonts that can be printed.

	By performing an insertion sort, we can automatically eliminate
	duplicate names.

 ******************************************************************************/

void
JXFontManager::GetFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	if (itsAllFontNames != NULL)
		{
		fontNames->CopyObjects(*itsAllFontNames, fontNames->GetCleanUpAction(), kJFalse);
		}
	else
		{
		fontNames->CleanOut();
		fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
		fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

#ifdef _J_USE_XFT
		FcBool scalable = 1;
		XftFontSet* fs = XftListFonts(*itsDisplay, DefaultScreen((Display*)*itsDisplay), XFT_SCALABLE, XftTypeBool, scalable, 0, XFT_FAMILY, 0);

		for (int i=0; i<fs->nfont; i++)
		{
			char* raw_name;
			if (XftPatternGetString(fs->fonts[i], XFT_FAMILY, 0, &raw_name) == XftResultMatch)
			{
				JString name(raw_name);
				JBoolean isDuplicate;
				const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
				if (!isDuplicate)
				{
					fontNames->InsertAtIndex(index, name);
				}
			}
		}
		
		XftFontSetDestroy(fs);		
#else
		int nameCount;
		char** nameList = XListFonts(*itsDisplay, "-*-*-*-*-*-*-*-*-75-75-*-*-*-*",
									 INT_MAX, &nameCount);
		if (nameList == NULL)
			{
			return;
			}

		JString name, charSet;
		for (int i=0; i<nameCount; i++)
			{
			const std::string s(nameList[i], strlen(nameList[i]));
			std::istringstream input(s);
			input.ignore();						// initial dash
			JIgnoreUntil(input, '-');			// foundry name
			name = JReadUntil(input, '-');		// font name
			JIgnoreUntil(input, "-75-75-");		// ignore specs
			JIgnoreUntil(input, '-');
			JIgnoreUntil(input, '-');
			JReadAll(input, &charSet);			// char set

			if (name.IsEmpty() || name == "nil")
				{
				continue;
				}

			ConvertToPSFontName(&name);

			// Until JPSPrinter can embed fonts in a Postscript file, we are limited
			// to only the standard Postscript fonts.

			if (name == JXGetCourierFontName()   ||
				name == JXGetHelveticaFontName() ||
				name == JXGetSymbolFontName()    ||
				name == JXGetTimesFontName())
				{
//				if (name != JXGetSymbolFontName())
//					{
//					name = CombineNameAndCharacterSet(name, charSet);
//					}

				JBoolean isDuplicate;
				const JIndex index =
					fontNames->GetInsertionSortIndex(&name, &isDuplicate);
				if (!isDuplicate)
					{
					fontNames->InsertAtIndex(index, name);
					}
				}
			}

		XFreeFontNames(nameList);
#endif

		// save names for next time

		JXFontManager* me = const_cast<JXFontManager*>(this);

		me->itsAllFontNames =
			new JPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll, kJTrue);
		assert( me->itsAllFontNames != NULL );
		}
}

/******************************************************************************
 GetMonospaceFontNames (virtual)

	By performing an insertion sort, we can automatically eliminate
	duplicate names.

 ******************************************************************************/

static const JCharacter* kMonospaceFontPattern[] =
{
	"-*-*-*-*-*-*-*-*-72-72-*-*-*-*",
	"-*-*-*-*-*-*-*-*-75-75-*-*-iso*-*",
};

const int kMonospaceFontPatternCount =
	sizeof(kMonospaceFontPattern) / sizeof(JCharacter*);

void
JXFontManager::GetMonospaceFontNames
	(
	JPtrArray<JString>* fontNames
	)
	const
{
	if (itsMonoFontNames != NULL)
		{
		fontNames->CopyObjects(*itsMonoFontNames, fontNames->GetCleanUpAction(), kJFalse);
		}
	else
		{
		(JXGetApplication())->DisplayBusyCursor();

		fontNames->CleanOut();
		fontNames->SetCompareFunction(JCompareStringsCaseInsensitive);
		fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

#ifdef _J_USE_XFT
		FcBool scalable = 1;
		XftFontSet* fs = XftListFonts(*itsDisplay, DefaultScreen((Display*)*itsDisplay), XFT_SPACING, XftTypeInteger, XFT_MONO,
																			XFT_SCALABLE, XftTypeBool, scalable, 0, XFT_FAMILY, 0);

		for (int i=0; i<fs->nfont; i++)
		{
			char* raw_name;
			if (XftPatternGetString(fs->fonts[i], XFT_FAMILY, 0, &raw_name) == XftResultMatch)
			{
				JString name(raw_name);
				JBoolean isDuplicate;
				const JIndex index = fontNames->GetInsertionSortIndex(&name, &isDuplicate);
				if (!isDuplicate)
				{
					fontNames->InsertAtIndex(index, name);
				}
			}
		}
		
		XftFontSetDestroy(fs);		
#else
		JPtrArray<JString> allFontNames(JPtrArrayT::kDeleteAll);
		allFontNames.SetCompareFunction(JCompareStringsCaseInsensitive);
		allFontNames.SetSortOrder(JOrderedSetT::kSortAscending);

		for (int j=0; j<kMonospaceFontPatternCount; j++)
			{
			int nameCount;
			char** nameList = XListFonts(*itsDisplay, kMonospaceFontPattern[j],
										 INT_MAX, &nameCount);
			if (nameList == NULL)
				{
				return;
				}

			JString name, charSet;
			for (int i=0; i<nameCount; i++)
				{
				const std::string s(nameList[i], strlen(nameList[i]));
				std::istringstream input(s);
				input.ignore();						// initial dash
				JIgnoreUntil(input, '-');			// foundry name
				name = JReadUntil(input, '-');		// font name
				JIgnoreUntil(input, "-iso");		// ignore specs
				JReadAll(input, &charSet);			// char set

				if (name.IsEmpty() || name == "nil")
					{
					continue;
					}

				ConvertToPSFontName(&name);

#if ! QUERY_FOR_MONOSPACE
				if (name == "Clean" || name == "Courier" || name == "Fixed" ||
					name == "Terminal" || name == "Lucidatypewriter" ||
					name == "Profontwindows")
					{
#endif

//				charSet.Prepend("iso");
//				name = CombineNameAndCharacterSet(name, charSet);

				JBoolean isDuplicate;
				const JIndex index =
					allFontNames.GetInsertionSortIndex(&name, &isDuplicate);
				if (!isDuplicate)
					{
					allFontNames.InsertAtIndex(index, name);

//					cout << nameList[i] << endl;

					XFontStruct* xfont = XLoadQueryFont(*itsDisplay, nameList[i]);
					if (xfont != NULL)
						{
						if (IsMonospace(*xfont))
							{
							JString* n = new JString(name);
							assert( n != NULL );
							const JBoolean ok = fontNames->InsertSorted(n, kJFalse);
							assert( ok );
							}
						XFreeFont(*itsDisplay, xfont);
						}
					}
#if ! QUERY_FOR_MONOSPACE
					}
#endif
				}

			XFreeFontNames(nameList);
			}
#endif

		// save names for next time

		JXFontManager* me = const_cast<JXFontManager*>(this);

		me->itsMonoFontNames =
			new JPtrArray<JString>(*fontNames, JPtrArrayT::kDeleteAll, kJTrue);
		assert( me->itsMonoFontNames != NULL );
		}
}

/******************************************************************************
 GetXFontNames

	This provides raw output from XListFonts().  For example, ^[0-9]x[0-9]+$
	would yield a list of clean, monospace fonts.

	If compare is NULL, we use JCompareStringsCaseInsensitive().

 ******************************************************************************/

void
JXFontManager::GetXFontNames
	(
	const JRegex&		regex,
	JPtrArray<JString>*	fontNames,
	JSortXFontNamesFn	compare
	)
	const
{
	fontNames->CleanOut();
	fontNames->SetCompareFunction(
					compare != NULL ? compare : JCompareStringsCaseInsensitive);
	fontNames->SetSortOrder(JOrderedSetT::kSortAscending);

#ifdef _J_USE_XFT
#else
	int nameCount;
	char** nameList = XListFonts(*itsDisplay, "*", INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return;
		}

	for (int i=0; i<nameCount; i++)
		{
		if (regex.Match(nameList[i]) && strcmp(nameList[i], "nil") != 0)
			{
			JString name = nameList[i];
			JBoolean isDuplicate;
			const JIndex index =
				fontNames->GetInsertionSortIndex(&name, &isDuplicate);
			if (!isDuplicate)
				{
				JString* n = new JString(name);
				assert( n != NULL );
				fontNames->InsertAtIndex(index, n);
				}
			}
		}

	XFreeFontNames(nameList);
#endif
}

/******************************************************************************
 GetFontSizes (virtual)

	If all font sizes are supported (e.g. TrueType), returns reasonable
	min and max, and empty sizeList.

	Returns kJFalse if there is no font with the specified name.

 ******************************************************************************/

JBoolean
JXFontManager::GetFontSizes
	(
	const JCharacter*	name,
	JSize*				minSize,
	JSize*				maxSize,
	JArray<JSize>*		sizeList
	)
	const
{
	*minSize = *maxSize = 0;
	sizeList->RemoveAll();
	sizeList->SetCompareFunction(JCompareSizes);
	sizeList->SetSortOrder(JOrderedSetT::kSortAscending);

#ifdef _J_USE_XFT
	XftFontSet* fs = XftListFonts(*itsDisplay, DefaultScreen((Display*)*itsDisplay), XFT_FAMILY, XftTypeString, name, 0, XFT_SIZE, 0);

	if (fs->nfont == 0)
	{
		*minSize = 8;
		*maxSize = 24;
		XftFontSetDestroy(fs);		
		return kJTrue;
	}

	for (int i=0; i<fs->nfont; i++)
	{
		double raw_size;
		if (XftPatternGetDouble(fs->fonts[i], XFT_SIZE, 0, &raw_size) == XftResultMatch)
		{
			JSize fontSize = static_cast<JSize>(raw_size);
			if (sizeList->IsEmpty())
			{
				*minSize = *maxSize = fontSize;
			}

			JBoolean isDuplicate;
			const JIndex index =
				sizeList->GetInsertionSortIndex(fontSize, &isDuplicate);
			if (!isDuplicate)
			{
				sizeList->InsertElementAtIndex(index, fontSize);

				if (fontSize < *minSize)
				{
					*minSize = fontSize;
				}
				else if (fontSize > *maxSize)
				{
					*maxSize = fontSize;
				}
			}
		}
	}
	
	// Look for scalable
	if ((*minSize == 0) && (*maxSize == 0))
	{
		*minSize = 8;
		*maxSize = 24;
		XftFontSetDestroy(fs);		
		return kJTrue;
	}
	
	XftFontSetDestroy(fs);		
#else
	JString xFontName, charSet;
	if (!ConvertToXFontName(name, &xFontName, &charSet))
		{
		charSet = "*-*";
		}

	// check for rescalable font

	JString regexStr = "-*-" + xFontName + "-*-*-*-*-*-0-75-75-*-*-" + charSet;

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList != NULL)
		{
		*minSize = 8;
		*maxSize = 24;
		XFreeFontNames(nameList);
		return kJTrue;
		}

	// get list of available sizes

	regexStr = "-*-" + xFontName + "-*-*-*-*-*-*-75-75-*-*-" + charSet;

	nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return kJFalse;
		}

	JSize fontSize;
	for (int i=0; i<nameCount; i++)
		{
		const std::string s(nameList[i], strlen(nameList[i]));
		std::istringstream input(s);
		input.ignore();							// initial dash
		JIgnoreUntil(input, '-');				// foundry name
		JIgnoreUntil(input, '-');				// font name
		JIgnoreUntil(input, '-');				// medium/bold
		JIgnoreUntil(input, '-');				// roman/oblique/italic
		JIgnoreUntil(input, '-');				// character spacing
		input.ignore();							// extra dash
		JIgnoreUntil(input, '-');				// pixel height
		input >> fontSize;						// 10*(point size)

		if (fontSize < 10)
			{
			continue;		// we already checked for rescalable version
			}

		fontSize /= 10;
		if (sizeList->IsEmpty())
			{
			*minSize = *maxSize = fontSize;
			}

		JBoolean isDuplicate;
		const JIndex index =
			sizeList->GetInsertionSortIndex(fontSize, &isDuplicate);
		if (!isDuplicate)
			{
			sizeList->InsertElementAtIndex(index, fontSize);

			if (fontSize < *minSize)
				{
				*minSize = fontSize;
				}
			else if (fontSize > *maxSize)
				{
				*maxSize = fontSize;
				}
			}
		}

	XFreeFontNames(nameList);
#endif

	return JNegate( sizeList->IsEmpty() );
}

/******************************************************************************
 GetFontStyles (virtual)

	Sets available styles to kJTrue.  Underline, strike, and colors are always
	available.

 ******************************************************************************/

JFontStyle
JXFontManager::GetFontStyles
	(
	const JCharacter*	name,
	const JSize			size
	)
	const
{
	JFontStyle style(kJFalse, kJFalse, 0, kJTrue);

#ifdef _J_USE_XFT
	double raw_size = size;
	XftFontSet* fs = XftListFonts(*itsDisplay, DefaultScreen((Display*)*itsDisplay), XFT_FAMILY, XftTypeString, name, XFT_SIZE, XftTypeDouble, raw_size, 0, XFT_SLANT, XFT_WEIGHT, 0);

	if (fs->nfont == 0)
	{
		return style;
	}

	for (int i=0; i<fs->nfont; i++)
	{
		int raw_slant;
		if (XftPatternGetInteger(fs->fonts[i], XFT_SLANT, 0, &raw_slant) == XftResultMatch)
		{
			if (raw_slant == XFT_SLANT_ITALIC)
				style.italic = kJTrue;
		}

		int raw_weight;
		if (XftPatternGetInteger(fs->fonts[i], XFT_WEIGHT, 0, &raw_weight) == XftResultMatch)
		{
			if (raw_weight == XFT_WEIGHT_BOLD)
				style.bold = kJTrue;
		}
	}
	
	XftFontSetDestroy(fs);		
#else
	JString xFontName, charSet;
	if (!ConvertToXFontName(name, &xFontName, &charSet))
		{
		charSet = "*-*";
		}

	const JString regexStr =
		"-*-" + xFontName + "-*-*-*-*-*-" +
		JString(10*size, 0, JString::kForceNoExponent) +
		"-75-75-*-*-" + charSet;

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return style;
		}

	JString weight, slant;
	for (int i=0; i<nameCount; i++)
		{
		const std::string s(nameList[i], strlen(nameList[i]));
		std::istringstream input(s);
		input.ignore();							// initial dash
		JIgnoreUntil(input, '-');				// foundry name
		JIgnoreUntil(input, '-');				// font name
		weight = JReadUntil(input, '-');		// medium/bold
		slant  = JReadUntil(input, '-');		// roman/oblique/italic

		if (weight == "bold")
			{
			style.bold = kJTrue;
			}
		if (slant == "o" || slant == "i")
			{
			style.italic = kJTrue;
			}
		}

	XFreeFontNames(nameList);
#endif

	return style;
}

/******************************************************************************
 GetFontCharSets (virtual)

	Returns kJFalse if there are no available fonts.

 ******************************************************************************/

JBoolean
JXFontManager::GetFontCharSets
	(
	const JCharacter*	name,
	const JSize			size,
	JPtrArray<JString>*	charSetList
	)
	const
{
	charSetList->CleanOut();
	charSetList->SetCompareFunction(JCompareStringsCaseInsensitive);
	charSetList->SetSortOrder(JOrderedSetT::kSortAscending);

	JString xFontName, charSet;
	ConvertToXFontName(name, &xFontName, &charSet);		// strip charSet

	const JString regexStr =
		"-*-" + xFontName + "-*-*-*-*-*-" +
		JString(10*size, 0, JString::kForceNoExponent) +
		"-75-75-*-*-*-*";

	int nameCount;
	char** nameList = XListFonts(*itsDisplay, regexStr, INT_MAX, &nameCount);
	if (nameList == NULL)
		{
		return kJFalse;
		}

	for (int i=0; i<nameCount; i++)
		{
		const std::string s(nameList[i], strlen(nameList[i]));
		std::istringstream input(s);
		input.ignore();							// initial dash
		JIgnoreUntil(input, '-');				// foundry name
		JIgnoreUntil(input, '-');				// font name
		JIgnoreUntil(input, '-');				// medium/bold
		JIgnoreUntil(input, '-');				// roman/oblique/italic
		JIgnoreUntil(input, '-');				// character spacing
		input.ignore();							// extra dash
		JIgnoreUntil(input, '-');				// pixel height
		JIgnoreUntil(input, '-');				// 10*(point size)
		JIgnoreUntil(input, '-');				// x resolution
		JIgnoreUntil(input, '-');				// y resolution
		JIgnoreUntil(input, '-');				// ?
		JIgnoreUntil(input, '-');				// ?
		JReadAll(input, &charSet);

		JBoolean isDuplicate;
		const JIndex index =
			charSetList->GetInsertionSortIndex(&charSet, &isDuplicate);
		if (!isDuplicate)
			{
			JString* s = new JString(charSet);
			assert( s != NULL );
			charSetList->InsertElementAtIndex(index, s);
			}
		}

	XFreeFontNames(nameList);
	return JNegate( charSetList->IsEmpty() );
}

/******************************************************************************
 GetFontID (virtual)

 ******************************************************************************/

JFontID
JXFontManager::GetFontID
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FontInfo info = itsFontList->GetElement(i);
		if (*(info.name) == name && info.size == size &&
			info.style.bold == style.bold &&
			info.style.italic == style.italic)
			{
			return i;
			}
		}

	// falling through means we need to create a new entry

	JString xFontName, charSet;
	ConvertToXFontName(name, &xFontName, &charSet);

#ifdef _J_USE_XFT
	FontInfo info;
	info.xftfont = GetNewFont(xFontName, charSet, size, style);
	info.exact = kJTrue;

	if (info.xftfont == NULL)
		{
		info.exact = kJFalse;
		info.xftfont = ApproximateFont(xFontName, charSet, size, style);
		}
#else
	FontInfo info;
	info.xfont = GetNewFont(xFontName, charSet, size, style);
	info.exact = kJTrue;
	if (info.xfont == NULL)
		{
		info.exact = kJFalse;
		info.xfont = ApproximateFont(xFontName, charSet, size, style);
		}
#endif

	info.name = new JString(name);
	assert( info.name != NULL );

	info.size  = size;
	info.style = style;

	itsFontList->AppendElement(info);
	return itsFontList->GetElementCount();
}

/******************************************************************************
 GetFontID

	For X Windows only:  pass in complete string from XListFonts().

 ******************************************************************************/

JBoolean
JXFontManager::GetFontID
	(
	const JCharacter*	xFontStr,
	JFontID*			fontID
	)
	const
{
#ifdef _J_USE_XFT
	*fontID = 0;
	return kJFalse;
#else
	const JSize count = itsFontList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FontInfo info = itsFontList->GetElement(i);
		if (*(info.name) == xFontStr && info.size == 0)
			{
			*fontID = i;
			return kJTrue;
			}
		}

	// falling through means we need to create a new entry

	XFontStruct* xfont = XLoadQueryFont(*itsDisplay, xFontStr);
	if (xfont == NULL)
		{
		*fontID = 0;
		return kJFalse;
		}

	FontInfo info;

	info.name = new JString(xFontStr);
	assert( info.name != NULL );

	info.size  = 0;
	info.xfont = xfont;
	info.exact = kJTrue;

	itsFontList->AppendElement(info);

	*fontID = itsFontList->GetElementCount();
	return kJTrue;
#endif
}

/******************************************************************************
 GetNewFont (private)

 ******************************************************************************/

#ifdef _J_USE_XFT
XftFont*
#else
XFontStruct*
#endif
JXFontManager::GetNewFont
	(
	const JCharacter*	name,
	const JCharacter*	charSet,
	const JSize			size,
	const JFontStyle&	style
	)
	const
{
#ifdef _J_USE_XFT
	double raw_size = size;
	XftFont* font = XftFontOpen(*itsDisplay, DefaultScreen((Display*)*itsDisplay),
									XFT_FAMILY, XftTypeString, name,
									XFT_SIZE, XftTypeDouble, raw_size,
									XFT_WEIGHT, XftTypeInteger, style.bold ? XFT_WEIGHT_BOLD : XFT_WEIGHT_MEDIUM,
									XFT_SLANT, XftTypeInteger, style.italic ? XFT_SLANT_ITALIC : XFT_SLANT_ROMAN, 0);
	return font;
#else
	const JCharacter* italicStr = kObliqueStr;	// try oblique before italic
	JBoolean iso                = JStringEmpty(charSet);

	XFontStruct* xfont = NULL;
	while (1)
		{
		JString xFontStr = BuildFontName(name, charSet, size, style, italicStr, iso);
		xfont            = XLoadQueryFont(*itsDisplay, xFontStr);
		if (xfont != NULL)
			{
			break;
			}

		if (strcmp(italicStr, kObliqueStr) == 0 && style.italic)
			{
			italicStr = kItalicStr;
			}
		else if (iso)
			{
			iso       = kJFalse;
			italicStr = kObliqueStr;
			}
		else
			{
			break;	// give up and return NULL
			}
		}

	return xfont;
#endif
}

/******************************************************************************
 BuildFontName (private)

 ******************************************************************************/

JString
JXFontManager::BuildFontName
	(
	const JCharacter*	xName,
	const JCharacter*	charSet,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	italicStr,
	const JBoolean		iso
	)
	const
{
#ifdef _J_USE_XFT
	JString xFontName;
	return xFontName;
#else
	// handle NxM separately

	nxmRegex.SetMatchOnly(kJTrue);
	if (nxmRegex.Match(xName))
		{
		JString xFontName = xName;
		if (style.bold)
			{
			xFontName += "bold";
			}
		return xFontName;
		}

	// any foundry

	JString xFontName = "-*-";

	// given name

	xFontName.Append(xName);

	// regular or boldface

	if (style.bold)
		{
		xFontName.Append("-bold");
		}
	else
		{
		xFontName.Append("-medium");
		}

	// regular or italic

	if (style.italic)
		{
		xFontName.Append(italicStr);
		}
	else
		{
		xFontName.Append("-r");
		}

	// normal character spacing, any pixel size

	xFontName.Append("-normal-*-*-");

	// font size

	xFontName.Append( JString(10*size, 0, JString::kForceNoExponent) );

	// screen resolution (apparently, we should always just use 75 dpi fonts),
	// any spacing, any avg width

	xFontName.Append("-75-75-*-*-");

	JIndex latinIndex;
	if (!JStringEmpty(charSet))
		{
		xFontName.Append(charSet);
		}
	else if (iso && JXGetLatinCharacterSetIndex(&latinIndex))
		{
		JString s = "iso8859-";
		s += JString(latinIndex, 0);
		xFontName.Append(s);
		}
	else if (iso)
		{
		xFontName.Append("iso*-*");
		}
	else
		{
		xFontName.Append("*-*");
		}

	// return the result

	return xFontName;
#endif
}

/******************************************************************************
 ApproximateFont (private)

	This is called when a requested font is not available.  We are required
	to find an existing font that is reasonably close to the requested one.

	The result of this function is guaranteed to be a valid font.

 ******************************************************************************/

#ifdef _J_USE_XFT
XftFont*
#else
XFontStruct*
#endif
JXFontManager::ApproximateFont
	(
	const JCharacter*	origName,
	const JCharacter*	origCharSet,
	const JSize			origSize,
	const JFontStyle&	origStyle
	)
	const
{
	JString name     = origName;
	JString charSet  = origCharSet;
	JSize size       = origSize;
	JFontStyle style = origStyle;

#ifdef _J_USE_XFT
	XftFont* xfont = NULL;
#else
	XFontStruct* xfont = NULL;
#endif
	while (xfont == NULL)
		{
		if (size % 2 == 1)
			{
			size--;		// even sizes are more common
			}
		else if (size < 12)
			{
			size += 2;
			}
		else if (size > 12)
			{
			size -= 2;
			}
		else if (style.bold)
			{
			style.bold = kJFalse;
			size       = origSize;
			}
		else if (style.italic)
			{
			style.italic = kJFalse;
			size         = origSize;
			}
		else if (name != JGetDefaultFontName())
			{
			name  = JGetDefaultFontName();
			size  = origSize;
			style = origStyle;
			}
		else if (!charSet.IsEmpty())
			{
			charSet.Clear();
			name  = origName;
			size  = origSize;
			style = origStyle;
			}
		else
			{
			// this should never happen, but it does with some Win95 X servers

			cerr << "The X server does not have any PostScript fonts at all, not even ";
			cerr << JGetDefaultFontName() << endl;
			JXApplication::Abort(JXDocumentManager::kServerDead, kJFalse);
			}

		xfont = GetNewFont(name, charSet, size, style);
		}

	return xfont;
}

/******************************************************************************
 GetFontName (virtual)

 ******************************************************************************/

const JCharacter*
JXFontManager::GetFontName
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return *(info.name);
}

/******************************************************************************
 GetFontName (virtual)

 ******************************************************************************/

JBoolean
JXFontManager::IsExact
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return info.exact;
}

/******************************************************************************
 GetLineHeight (virtual)

 ******************************************************************************/

JSize
JXFontManager::GetLineHeight
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,

	JCoordinate*		ascent,
	JCoordinate*		descent
	)
	const
{
#ifdef _J_USE_XFT
	XftFont* xftfont = GetXftFont(fontID);
	*ascent  = xftfont->ascent;
	*descent = xftfont->descent;
#else
	XFontStruct* xfont = GetXFontInfo(fontID);
	*ascent  = xfont->ascent;
	*descent = xfont->descent;
#endif

	// Look for underline types
	if (style.underlineType == JFontStyle::plain_Underline)
	{
		const JSize underlineThickness = GetUnderlineThickness(size);
		if (((JSize) *descent) < 2 * underlineThickness * style.underlineCount)
			{
			*descent = 2 * underlineThickness * style.underlineCount;
			}
	}
	else
	{
		// Special underline types always have height 3
		if (((JSize) *descent) < 3)
			*descent = 3;
	}

	return (*ascent + *descent);
}

/******************************************************************************
 GetCharWidth (virtual)

 ******************************************************************************/

JSize
JXFontManager::GetCharWidth
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter	c
	)
	const
{
#ifdef _J_USE_XFT
	XftFont* xftfont = GetXftFont(fontID);
	XGlyphInfo extents;
	//XftTextExtents8(*itsDisplay, xftfont, (XftChar8*)&c, 1, &extents);
	XftTextExtentsUtf8(*itsDisplay, xftfont, (XftChar8*)&c, 1, &extents);
	return extents.xOff;
#else
	XFontStruct* xfont = GetXFontInfo(fontID);
	if (xfont->min_bounds.width == xfont->max_bounds.width)
		{
		return xfont->min_bounds.width;
		}
	else
		{
		return XTextWidth(xfont, &c, 1);
		}
#endif
}

/******************************************************************************
 GetCharWidth16 (virtual)

 ******************************************************************************/

JSize
JXFontManager::GetCharWidth16
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter16	c
	)
	const
{
#ifdef _J_USE_XFT
	XftFont* xftfont = GetXftFont(fontID);
	XGlyphInfo extents;
	unsigned short test = 0x00FF;
	bool bigE = ((const char*)&test)[0] == 0x00;
	XftTextExtentsUtf16(*itsDisplay, xftfont, (XftChar8*)&c, bigE ? FcEndianBig : FcEndianLittle, 2, &extents);
	return extents.xOff;
#else
	XFontStruct* xfont = GetXFontInfo(fontID);
	if (xfont->min_bounds.width == xfont->max_bounds.width)
		{
		return xfont->min_bounds.width;
		}
	else
		{
		JCharacter cc = c & 0x00FF;
		return XTextWidth(xfont, &cc, 1);
		}
#endif
}

/******************************************************************************
 GetStringWidth (virtual)

 ******************************************************************************/

JSize
JXFontManager::GetStringWidth
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter*	str,
	const JSize			charCount
	)
	const
{
#ifdef _J_USE_XFT
	XftFont* xftfont = GetXftFont(fontID);
	XGlyphInfo extents;
	//XftTextExtents8(*itsDisplay, xftfont, (XftChar8*)str, charCount, &extents);
	XftTextExtentsUtf8(*itsDisplay, xftfont, (XftChar8*)str, charCount, &extents);
	return extents.xOff;
#else
	XFontStruct* xfont = GetXFontInfo(fontID);
	if (IsMonospace(*xfont))
		{
		return charCount * xfont->min_bounds.width;
		}
	else
		{
		const JSize maxStringLength = itsDisplay->GetMaxStringLength();

		JSize width  = 0;
		JSize offset = 0;
		while (offset < charCount)
			{
			const JSize count = JMin(charCount - offset, maxStringLength);
			width  += XTextWidth(xfont, str + offset, count);
			offset += count;
			}

		return width;
		}
#endif
}

/******************************************************************************
 GetStringWidth16 (virtual)

 ******************************************************************************/

JSize
JXFontManager::GetStringWidth16
	(
	const JFontID		fontID,
	const JSize			size,
	const JFontStyle&	style,
	const JCharacter16*	str,
	const JSize			charCount
	)
	const
{
#ifdef _J_USE_XFT
	XftFont* xftfont = GetXftFont(fontID);
	XGlyphInfo extents;
	unsigned short test = 0x00FF;
	bool bigE = ((const char*)&test)[0] == 0x00;
	XftTextExtentsUtf16(*itsDisplay, xftfont, (XftChar8*)str, bigE ? FcEndianBig : FcEndianLittle, charCount * sizeof(JCharacter16), &extents);
	return extents.xOff;
#else
	XFontStruct* xfont = GetXFontInfo(fontID);
	if (IsMonospace(*xfont))
		{
		return charCount * xfont->min_bounds.width;
		}
	else
		{
		JString16 utf16(str, charCount);
		JString str_ascii = utf16.ToASCII();
		const JCharacter* _str = str_ascii.GetCString();
		JSize actualCount = str_ascii.GetLength();
		const JSize maxStringLength = itsDisplay->GetMaxStringLength();

		JSize width  = 0;
		JSize offset = 0;
		while (offset < actualCount)
			{
			const JSize count = JMin(actualCount - offset, maxStringLength);
			width  += XTextWidth(xfont, _str + offset, count);
			offset += count;
			}

		return width;
		}
#endif
}

/******************************************************************************
 GetXFontInfo

 ******************************************************************************/

#ifdef _J_USE_XFT
XftFont*
JXFontManager::GetXftFont
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return info.xftfont;
}
#else
XFontStruct*
JXFontManager::GetXFontInfo
	(
	const JFontID id
	)
	const
{
	const FontInfo info = itsFontList->GetElement(id);
	return info.xfont;
}
#endif

/******************************************************************************
 ConvertToXFontName (private)

	Extract character set and fold name to lower case.
	Returns kJTrue if *charSet is not empty.

 ******************************************************************************/

JBoolean
JXFontManager::ConvertToXFontName
	(
	const JCharacter*	origName,
	JString*			fontName,
	JString*			charSet
	)
	const
{
	const JBoolean hasCharSet = ExtractCharacterSet(origName, fontName, charSet);
	fontName->ToLower();
	return hasCharSet;
}

/******************************************************************************
 ConvertToPSFontName (private)

	Capitalize the first character of every word.

 ******************************************************************************/

void
JXFontManager::ConvertToPSFontName
	(
	JString* name
	)
	const
{
	const JSize length = name->GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		if (i == 1 || name->GetCharacter(i-1) == ' ')
			{
			name->SetCharacter(i, JToUpper(name->GetCharacter(i)));
			}
		}
}

#define JTemplateType JXFontManager::FontInfo
#include <JArray.tmpls>
#undef JTemplateType
