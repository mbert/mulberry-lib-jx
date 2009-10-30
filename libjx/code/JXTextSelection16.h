/******************************************************************************
 JXTextSelection16.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextSelection16
#define _H_JXTextSelection16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionManager.h>
#include <JTextEditor16.h>

class JXColormap;

class JXTextSelection16 : public JXSelectionData
{
public:

	JXTextSelection16(JXDisplay* display, const JCharacter16* text,
					const JXColormap* colormap = NULL,
					const JRunArray<JTextEditor16::Font>* style = NULL);
	JXTextSelection16(JXDisplay* display, JString16* text,
					const JXColormap* colormap = NULL,
					JRunArray<JTextEditor16::Font>* style = NULL);
	JXTextSelection16(JXDisplay* display, const JPtrArray<JString16>& list);
	JXTextSelection16(JXWidget* widget, const JCharacter* id);

	virtual	~JXTextSelection16();

	JBoolean	GetText(const JString16** text) const;
	JBoolean	GetStyle(const JRunArray<JTextEditor16::Font>** style) const;
	JBoolean	GetColormap(const JXColormap** colormap) const;
	JBoolean	GetTextEditor(JTextEditor16** te) const;

	void	SetData(const JCharacter16* text,
					const JXColormap* colormap = NULL,
					const JRunArray<JTextEditor16::Font>* style = NULL);
	void	SetData(JString16* text, const JXColormap* colormap,
					JRunArray<JTextEditor16::Font>* style);
	void	SetData(const JPtrArray<JString16>& list);

	void	SetTextEditor(JTextEditor16* te, const JIndexRange& selection);

	static const JCharacter*	GetStyledText0XAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JString16*						itsText;		// NULL until Resolve()
	JRunArray<JTextEditor16::Font>*	itsStyle;		// can be NULL
	const JXColormap*				itsColormap;	// not owned; can be NULL
	JTextEditor16*					itsTE;			// not owned; can be NULL
	JIndexRange						itsSelection;

	Atom	itsStyledText0XAtom;

private:

	void	JXTextSelection16X();
	void	SetColormap(const JXColormap* colormap);

	// not allowed

	JXTextSelection16(const JXTextSelection16& source);
	const JXTextSelection16& operator=(const JXTextSelection16& source);
};


/******************************************************************************
 GetText

 ******************************************************************************/

inline JBoolean
JXTextSelection16::GetText
	(
	const JString16** text
	)
	const
{
	*text = itsText;
	return JI2B( itsText != NULL );
}

/******************************************************************************
 GetStyle

 ******************************************************************************/

inline JBoolean
JXTextSelection16::GetStyle
	(
	const JRunArray<JTextEditor16::Font>** style
	)
	const
{
	*style = itsStyle;
	return JI2B( itsStyle != NULL );
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JBoolean
JXTextSelection16::GetColormap
	(
	const JXColormap** colormap
	)
	const
{
	*colormap = itsColormap;
	return JI2B( itsColormap != NULL );
}

/******************************************************************************
 GetTextEditor

 ******************************************************************************/

inline JBoolean
JXTextSelection16::GetTextEditor
	(
	JTextEditor16** te
	)
	const
{
	*te = itsTE;
	return JI2B( itsTE != NULL );
}

#endif
