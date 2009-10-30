/******************************************************************************
 JXTextSelection16.cpp

	text/plain, text/x-jxstyled0 for all, and STRING, TEXT for all but DND.
	DELETE is supported if a JTextEditor16 is provided.

	BASE CLASS = JXSelectionData

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextSelection16.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <JString.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kStyledText0XAtomName = "text/x-jxstyled0-16";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextSelection16::JXTextSelection16
	(
	JXDisplay*							display,
	const JCharacter16*					text,
	const JXColormap*					colormap,
	const JRunArray<JTextEditor16::Font>*	style
	)
	:
	JXSelectionData(display)
{
	JXTextSelection16X();
	SetData(text, colormap, style);
}

JXTextSelection16::JXTextSelection16
	(
	JXDisplay*						display,
	JString16*						text,
	const JXColormap*				colormap,
	JRunArray<JTextEditor16::Font>*	style
	)
	:
	JXSelectionData(display)
{
	JXTextSelection16X();
	SetData(text, colormap, style);
}

JXTextSelection16::JXTextSelection16
	(
	JXDisplay*					display,
	const JPtrArray<JString16>&	list
	)
	:
	JXSelectionData(display)
{
	JXTextSelection16X();
	SetData(list);
}

JXTextSelection16::JXTextSelection16
	(
	JXWidget*			widget,
	const JCharacter*	id
	)
	:
	JXSelectionData(widget, id)
{
	JXTextSelection16X();
}

// private

void
JXTextSelection16::JXTextSelection16X()
{
	itsText     = NULL;
	itsStyle    = NULL;
	itsColormap = NULL;
	itsTE       = NULL;

	itsStyledText0XAtom = None;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextSelection16::~JXTextSelection16()
{
	delete itsText;
	delete itsStyle;
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXTextSelection16::AddTypes
	(
	const Atom selectionName
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	AddType(selMgr->GetMimePlainTextXAtom());
	itsStyledText0XAtom = AddType(kStyledText0XAtomName);

	if (selectionName != GetDNDSelectionName())
		{
		AddType(XA_STRING);
		AddType(selMgr->GetUTF8StringXAtom());
		AddType(selMgr->GetTextXAtom());
		}
}

/******************************************************************************
 SetData

	Makes a copy of text and style.

	colormap and style can be NULL.
	If style != NULL, colormap must not be NULL.

 ******************************************************************************/

void
JXTextSelection16::SetData
	(
	const JCharacter16*					text,
	const JXColormap*					colormap,
	const JRunArray<JTextEditor16::Font>*	style
	)
{
	assert( style == NULL || colormap != NULL );

	if (itsText != NULL)
		{
		*itsText = text;
		}
	else
		{
		itsText = new JString16(text);
		assert( itsText != NULL );
		}

	SetColormap(colormap);

	if (style != NULL && itsStyle != NULL)
		{
		*itsStyle = *style;
		}
	else if (style != NULL && itsStyle == NULL)
		{
		itsStyle = new JRunArray<JTextEditor16::Font>(*style);
		assert( itsStyle != NULL );
		}
	else
		{
		delete itsStyle;
		itsStyle = NULL;
		}

	SetTextEditor(NULL, JIndexRange());
}

/******************************************************************************
 SetData

	Takes ownership of text and style.

	colormap and style can be NULL.
	If style != NULL, colormap must not be NULL.

 ******************************************************************************/

void
JXTextSelection16::SetData
	(
	JString16*						text,
	const JXColormap*				colormap,
	JRunArray<JTextEditor16::Font>*	style
	)
{
	assert( style == NULL || colormap != NULL );

	delete itsText;
	itsText = text;

	SetColormap(colormap);

	delete itsStyle;
	itsStyle = style;

	SetTextEditor(NULL, JIndexRange());
}

/******************************************************************************
 SetData

	If there is a single string, this becomes the text.
	If there are multiple strings, they are separated with newlines.

 ******************************************************************************/

void
JXTextSelection16::SetData
	(
	const JPtrArray<JString16>& list
	)
{
	if (itsText == NULL)
		{
		itsText = new JString16;
		assert( itsText != NULL );
		}
	else
		{
		itsText->Clear();
		}

	SetColormap(NULL);

	delete itsStyle;
	itsStyle = NULL;

	SetTextEditor(NULL, JIndexRange());

	const JSize count = list.GetElementCount();
	JSize selectCount = 0;
	for (JIndex i=1; i<=count; i++)
		{
		if (selectCount > 0)
			{
			itsText->AppendCharacter('\n');
			}
		*itsText += *(list.NthElement(i));
		selectCount++;
		}

	if (selectCount > 1)
		{
		itsText->AppendCharacter('\n');
		}
}

/******************************************************************************
 SetColormap (private)

 ******************************************************************************/

void
JXTextSelection16::SetColormap
	(
	const JXColormap* colormap
	)
{
	if (itsColormap != NULL)
		{
		StopListening(itsColormap);
		}
	itsColormap = colormap;
	if (itsColormap != NULL)
		{
		ListenTo(itsColormap);
		}
}

/******************************************************************************
 SetTextEditor

	Call this to support DELETE.

 ******************************************************************************/

void
JXTextSelection16::SetTextEditor
	(
	JTextEditor16*		te,
	const JIndexRange&	selection
	)
{
	if (itsTE != NULL)
		{
		StopListening(itsTE);
		}
	itsTE = te;
	if (itsTE != NULL)
		{
		ListenTo(itsTE);
		}

	itsSelection = selection;
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
JXTextSelection16::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	const Atom dndName = GetDNDSelectionName();
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	const Atom mimeText        = selMgr->GetMimePlainTextXAtom();

	JIndexRange selection;

	if ((requestType == XA_STRING ||
		 requestType == mimeText ||
		 requestType == selMgr->GetTextXAtom()) &&
		itsText != NULL)
		{
		// Downgrade to ascii
		JString ascii = itsText->ToASCII();

		*returnType = (requestType == mimeText) ? mimeText : XA_STRING;
		*dataLength = ascii.GetLength();
		*data       = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, ascii.GetCString(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == selMgr->GetUTF8StringXAtom() &&
		itsText != NULL)
		{
		// Convert to utf8
		JString utf8 = itsText->ToUTF8();

		*returnType = requestType;
		*dataLength = utf8.GetLength();
		*data       = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, utf8.GetCString(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == itsStyledText0XAtom &&
			 itsText != NULL && itsColormap != NULL && itsStyle != NULL)
		{
		const JFileVersion vers = 1;
		std::ostringstream dataStream;
		JTextEditor16::WritePrivateFormat(dataStream, (GetDisplay())->GetFontManager(),
										itsColormap, vers, *itsText, *itsStyle,
										1, itsText->GetLength());

		const std::string s = dataStream.str();
		*returnType         = itsStyledText0XAtom;
		*dataLength         = s.length();
		*data               = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, s.data(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTE != NULL && IsCurrent() &&
			 itsTE->GetSelection(&selection) && selection == itsSelection)
		{
		itsTE->DeleteSelection();

		*data       = new unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return kJTrue;
		}

	*data       = NULL;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXTextSelection16::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == const_cast<JXColormap*>(itsColormap))
		{
		itsColormap = NULL;
		}
	else if (sender == itsTE)
		{
		itsTE = NULL;
		}
	else
		{
		JXSelectionData::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 GetStyledText0XAtomName (static)

 ******************************************************************************/

const JCharacter*
JXTextSelection16::GetStyledText0XAtomName()
{
	return kStyledText0XAtomName;
}
