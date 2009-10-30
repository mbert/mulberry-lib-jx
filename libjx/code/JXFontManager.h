/******************************************************************************
 JXFontManager.h

	Interface for the JXFontManager class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFontManager
#define _H_JXFontManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFontManager.h>
#include <JArray.h>
#include <X11/Xlib.h>
#ifdef _J_USE_XFT
#include <X11/Xft/Xft.h>
#endif

class JString;
class JRegex;
class JXDisplay;
class JXColormap;

typedef JOrderedSetT::CompareResult
	(*JSortXFontNamesFn)(JString * const &, JString * const &);

class JXFontManager : public JFontManager
{
public:

	JXFontManager(JXDisplay* display, JXColormap* colormap);

	virtual ~JXFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JCharacter* name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;
	virtual JFontStyle	GetFontStyles(const JCharacter* name, const JSize size) const;
	virtual JBoolean	GetFontCharSets(const JCharacter* name, const JSize size,
										JPtrArray<JString>* charSetList) const;

	virtual JFontID				GetFontID(const JCharacter* name, const JSize size,
										  const JFontStyle& style) const;
	virtual const JCharacter*	GetFontName(const JFontID id) const;
	virtual JBoolean			IsExact(const JFontID id) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JSize size,
								 const JFontStyle& style, const JCharacter c) const;
	virtual JSize	GetCharWidth16(const JFontID fontID, const JSize size,
								 const JFontStyle& style, const JCharacter16 c) const;

	virtual JSize	GetStringWidth(const JFontID fontID, const JSize size,
								   const JFontStyle& style,
								   const JCharacter* str, const JSize charCount) const;
	virtual JSize	GetStringWidth16(const JFontID fontID, const JSize size,
								   const JFontStyle& style,
								   const JCharacter16* str, const JSize charCount) const;

	// for X Windows only

	void			GetXFontNames(const JRegex& regex,
								  JPtrArray<JString>* fontNames,
								  JSortXFontNamesFn compare = NULL) const;
	JBoolean		GetFontID(const JCharacter* xFontStr, JFontID* fontID) const;
#ifdef _J_USE_XFT
	XftFont*		GetXftFont(const JFontID id) const;
#else
	XFontStruct*	GetXFontInfo(const JFontID id) const;
#endif

private:

	struct FontInfo
	{
		JString*		name;
		JSize			size;
		JFontStyle		style;
#ifdef _J_USE_XFT
		XftFont*		xftfont;
#else
		XFontStruct*	xfont;
#endif
		JBoolean		exact;	// kJTrue => exact match to requested specs
	};

private:

	JXDisplay*			itsDisplay;
	JArray<FontInfo>*	itsFontList;

	JPtrArray<JString>*	itsAllFontNames;	// can be NULL
	JPtrArray<JString>*	itsMonoFontNames;	// can be NULL

private:

	JString	BuildFontName(const JCharacter* xName, const JCharacter* charSet,
						  const JSize size, const JFontStyle& style,
						  const JCharacter* italicStr, const JBoolean iso) const;

#ifdef _J_USE_XFT
	XftFont*		GetNewFont(const JCharacter* name, const JCharacter* charSet,
							   const JSize size, const JFontStyle& style) const;
	XftFont*		ApproximateFont(const JCharacter* name, const JCharacter* charSet,
									const JSize size, const JFontStyle& style) const;
#else
	XFontStruct*	GetNewFont(const JCharacter* name, const JCharacter* charSet,
							   const JSize size, const JFontStyle& style) const;
	XFontStruct*	ApproximateFont(const JCharacter* name, const JCharacter* charSet,
									const JSize size, const JFontStyle& style) const;
#endif

	JBoolean	ConvertToXFontName(const JCharacter* origName,
								   JString* fontName, JString* charSet) const;
	void		ConvertToPSFontName(JString* name) const;

#ifdef _J_USE_XFT
	int	IsMonospace(const XftFont& xftfont) const;
#else
	int	IsMonospace(const XFontStruct& xfont) const;
#endif

	// not allowed

	JXFontManager(const JXFontManager& source);
	const JXFontManager& operator=(const JXFontManager& source);
};



/******************************************************************************
 IsMonospace (private)

 ******************************************************************************/

#ifdef _J_USE_XFT
inline int
JXFontManager::IsMonospace
	(
	const XftFont& xftfont
	)
	const
{
	//  Not used by the Xft part of the code
	return false;
}
#else
inline int
JXFontManager::IsMonospace
	(
	const XFontStruct& xfont
	)
	const
{
	return (xfont.min_bounds.width == xfont.max_bounds.width);
}
#endif

#endif
