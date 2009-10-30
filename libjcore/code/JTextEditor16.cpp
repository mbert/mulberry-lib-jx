/******************************************************************************
 JTextEditor16.cpp

	Class to edit styled text.

	CaretLineChanged is broadcast by default.  If anybody requests
	CaretLocationChanged, CaretLineChanged will no longer be broadcast,
	since it duplicates the information.  Generic classes should accept
	either message to avoid depending on the setting.

	Only public and protected functions are required to call NewUndo(),
	and only if the action to be performed changes the text or styles.
	Private functions must -not- call NewUndo(), because several manipulations
	may be required to perform one user command, and only the user command
	as a whole is undoable.  (Otherwise, the user may get confused.)

	Because of this convention, public functions that affect undo should only
	be a public interface to a private function.  The public function calls
	NewUndo() and then calls the private function.  The private function
	does all the work, but doesn't modify the undo information.  This allows other
	private functions to use the routine (private version) without modifying the
	undo information.

	In order to guarantee that the TextChanged message means "text has already
	changed", NewUndo() must be called -after- making the modification.
	(even though the Undo object has to be created before the modifications)

	TextSet is different from TextChanged because documents will typically
	use only the latter for setting their "unsaved" flag.

	When the text ends with a newline, we have to draw the caret on the
	next line.  This is a special case because (charIndex == bufLength+1)
	would normally catch this.  The code required to handle this special
	case is marked with "ends with newline".  (Most, but not all, such
	code calls EndsWithNewline().)  We handle this special case internally
	rather than appending an extra item to itsLineStarts because this keeps
	the issue internal rather than forcing clients to deal with it.

	Derived classes must implement the following routines:

		TERefresh
			Put an update event in the queue to redraw the text.

		TERefreshRect
			Put an update event in the queue to redraw part of the text.

		TEUpdateDisplay
			Redraw whatever was requested by TERefresh() and TERefreshRect().
			(This is not pure virtual because some systems automatically
			 update the window after each event.)

		TERedraw
			Redraw the text immediately.

		TESetGUIBounds
			Set the bounds of the GUI object to match our size.  The original
			height and vertical position of the change can be used to update
			scrolltabs.  If change position is negative, don't update the
			scrolltabs.

		TEWidthIsBeyondDisplayCapacity
			Returns kJTrue if the given bounds width is too wide for the
			current graphics system to handle.  If so, breakCROnly is
			turned off.

		TEScrollToRect
			Scroll the text to make the given rectangle visible, possibly
			centering it in the middle of the display.  Return kJTrue
			if scrolling was necessary.

		TEScrollForDrag
			Scroll the text to make the given point visible.  Return kJTrue
			if scrolling was necessary.

		TESetVertScrollStep
			Set the vertical step size and page context used when scrolling
			the text.

		TECaretShouldBlink
			If blink is kJTrue, reset the timer and make the caret blink by
			calling TEShow/HideCaret().  Otherwise, call TEHideCaret().
			The derived class' constructor must call TECaretShouldBlink(kJTrue)
			because it is pure virtual for us.

		TEClipboardChanged
			Do whatever is appropriate to update the system clipboard
			after a Copy or Cut operation.  If ! itsStoreClipFlag, this
			should call GetSelection() and store the resulting text.

		TEOwnsClipboard
			Returns kJTrue if the internal clipboard should be used.

		TEGetExternalClipboard
			Returns kJTrue if there is something pasteable on the system clipboard.

		TEBeginDND
			Returns kJTrue if it is able to start a Drag-And-Drop session
			with the OS.  From then on, the derived class should call the
			TEHandleDND*() functions.  It must also call TEDNDFinished()
			when the drag ends.  If TEBeginDND() returns kJFalse, then we
			will manage an internal DND session.  In this case, the derived
			class should continue to call the usual TEHandleMouse*()
			functions.

		TEPasteDropData
			Get the data that was dropped and use Paste(text,style)
			to insert it.  (Before this is called, the insertion point
			is set so that Paste() will work correctly.)

		TEHasSearchText
			Return kJTrue if the user has entered any text for which to search.

	To draw page headers and footers while printing, override the
	following routines:

		GetPrintHeaderHeight
			Return the height required for the page header.

		DrawPrintHeader
			Draw the page header.  JTable will lock the header afterwards.

		GetPrintFooterHeight
			Return the height required for the page footer.

		DrawPrintFooter
			Draw the page footer.  JTable will lock the footer afterwards.

	The default implementation of tabs rounds the location up to the nearest
	multiple of itsDefTabWidth.  To implement non-uniform tabs or tabs on a
	line-by-line basis, override the following function:

		GetTabWidth
			Given the index of the tab character (charIndex) and the horizontal
			position on the line (in pixels) where the tab character starts (x),
			return the width of the tab character.

	Functionality yet to be implemented:

		Search & replace for text and styles combined

	BASE CLASS = virtual JBroadcaster

	Copyright  1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTextEditor16.h>
#include <JTEUndoTyping16.h>
#include <JTEUndoPaste16.h>
#include <JTEUndoDrop16.h>
#include <JTEUndoStyle16.h>
#include <JTEUndoTabShift16.h>
#include <JPagePrinter.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JOrderedSetUtil.h>
#include <JRunArrayIterator.h>
#include <JRegex.h>
#include <JLatentPG.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jMouseUtil.h>
#include <jTime.h>
#include <ctype.h>
#include <jGlobals.h>
#include <jAssert.h>

template <class T> class StValueChanger
{
public:
	StValueChanger(T &ioVariable, T inNewValue) :
		mVariable(ioVariable),
		mOrigValue(ioVariable)
		{ ioVariable = inNewValue; };
	~StValueChanger()
		{ mVariable = mOrigValue; };

	T	&mVariable;
	T	mOrigValue;
};

const JCoordinate kDefLeftMarginWidth = 10;
const JCoordinate kRightMarginWidth   = 2;

const JCoordinate kDraggedOutlineRadius = 10;

const JFileVersion kCurrentPrivateFormatVersion = 1;

const JSize kDefaultMaxUndoCount = 100;

const JSize kUNIXLineWidth    = 75;
const JSize kUNIXTabCharCount = 8;

static const JCharacter* kCRMCaretActionText     = "Clean paragraph margins";
static const JCharacter* kCRMSelectionActionText = "Clean margins for selection";

static const JCharacter* kCRM2CaretActionText     = "Coerce paragraph margins";
static const JCharacter* kCRM2SelectionActionText = "Coerce margins for selection";

JBoolean JTextEditor16::itsCopyWhenSelectFlag = kJFalse;
JBoolean JTextEditor16::itsReplaceIllegalChars = kJFalse;

JBoolean (*JTextEditor16::itsI18NCharInWordFn)(const JCharacter16) = NULL;

// JBroadcaster message types

const JCharacter* JTextEditor16::kTypeChanged          = "TypeChanged::JTextEditor16";
const JCharacter* JTextEditor16::kTextSet              = "TextSet::JTextEditor16";
const JCharacter* JTextEditor16::kTextChanged          = "TextChanged::JTextEditor16";
const JCharacter* JTextEditor16::kCaretLineChanged     = "CaretLineChanged::JTextEditor16";
const JCharacter* JTextEditor16::kCaretLocationChanged = "CaretLocationChanged::JTextEditor16";

/******************************************************************************
 Constructor

	We don't provide a constructor that accepts text because we
	can't call RecalcAll() due to pure virtual functions.

	*** Derived classes must call RecalcAll().

 ******************************************************************************/

JTextEditor16::JTextEditor16
	(
	const Type			type,
	const JBoolean		breakCROnly,
	const JBoolean		pasteStyledText,
	const JBoolean		useInternalClipboard,
	const JFontManager*	fontManager,
	JColormap*			colormap,
	const JColorIndex	caretColor,
	const JColorIndex	selectionColor,
	const JColorIndex	outlineColor,
	const JColorIndex	dragColor,
	const JColorIndex	wsColor,
	const JCoordinate	width
	)
	:
	itsType(type),
	itsPasteStyledTextFlag(pasteStyledText),

	itsFontMgr(fontManager),
	itsColormap(colormap),

	itsCustomColors( itsColormap ),
	itsCaretColor(caretColor),
	itsSelectionColor(selectionColor),
	itsSelectionOutlineColor(outlineColor),
	itsDragColor(dragColor),
	itsWhitespaceColor(wsColor),

	itsStoreClipFlag(useInternalClipboard)
{
	itsBuffer = new JString16;
	assert( itsBuffer != NULL );

	itsStyles = new JRunArray<Font>;
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = kDefaultMaxUndoCount;
	itsActiveFlag              = kJFalse;
	itsSelActiveFlag           = kJFalse;
	itsCaretVisibleFlag        = kJFalse;
	itsPerformDNDFlag          = kJFalse;
	itsAutoIndentFlag          = kJFalse;
	itsTabToSpacesFlag         = kJFalse;
	itsMoveToFrontOfTextFlag   = kJFalse;
	itsBcastLocChangedFlag     = kJFalse;
	itsBcastAllTextChangedFlag = kJFalse;
	itsBreakCROnlyFlag         = breakCROnly;
	itsIsPrintingFlag          = kJFalse;
	itsDoCopySelectFlag        = kJFalse;
	itsDrawWhitespaceFlag      = kJFalse;

	itsDefFont.size = kJDefaultFontSize;
	// itsDefFont.style is automatically empty
	itsDefFont.id = itsFontMgr->GetFontID(JGetDefaultFontName(), itsDefFont.size, itsDefFont.style);

	itsWidth           = width - kDefLeftMarginWidth - kRightMarginWidth;
	itsHeight          = 0;
	itsGUIWidth        = itsWidth;
	itsLeftMarginWidth = kDefLeftMarginWidth;
	itsDefTabWidth     = 36;	// 1/2 inch
	itsMaxWordWidth    = 0;

	itsLineStarts = new JArray<JIndex>;
	assert( itsLineStarts != NULL );
	itsLineStarts->SetCompareFunction(JCompareIndices);
	itsLineStarts->SetSortOrder(JOrderedSetT::kSortAscending);

	itsLineWidths = new JArray<JCoordinate>;
	assert( itsLineWidths != NULL );

	itsLineGeom = new JRunArray<LineGeometry>;
	assert( itsLineGeom != NULL );

	itsCaretLoc         = CaretLocation(1,1);
	itsCaretX           = 0;
	itsInsertionFont    = CalcInsertionFont(1);

	itsClipText         = NULL;
	itsClipStyle        = NULL;

	itsPrevDragType     = itsDragType = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;

	itsPrevBufLength    = 0;
	itsCRMLineWidth     = kUNIXLineWidth;
	itsCRMTabCharCount  = kUNIXTabCharCount;
	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;

	itsCharInWordFn     = DefaultIsCharacterInWord;

	if (type == kFullEditor)
		{
		itsBuffer->SetBlockSize(1024);
		itsStyles->SetBlockSize(128);
		itsLineStarts->SetBlockSize(128);
		itsLineWidths->SetBlockSize(100);
		itsLineGeom->SetBlockSize(128);
		}
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTextEditor16::JTextEditor16
	(
	const JTextEditor16& source
	)
	:
	JBroadcaster(source),

	itsType( source.itsType ),
	itsPasteStyledTextFlag( source.itsPasteStyledTextFlag ),

	itsFontMgr( source.itsFontMgr ),
	itsColormap( source.itsColormap ),

	itsCustomColors( source.itsCustomColors ),
	itsCaretColor( source.itsCaretColor ),
	itsSelectionColor( source.itsSelectionColor ),
	itsSelectionOutlineColor( source.itsSelectionOutlineColor ),
	itsDragColor( source.itsDragColor ),

	itsStoreClipFlag( source.itsStoreClipFlag )
{
	itsBuffer = new JString16(*(source.itsBuffer));
	assert( itsBuffer != NULL );

	itsStyles = new JRunArray<Font>(*(source.itsStyles));
	assert( itsStyles != NULL );

	itsUndo                    = NULL;
	itsUndoList                = NULL;
	itsFirstRedoIndex          = 1;
	itsLastSaveRedoIndex       = itsFirstRedoIndex;
	itsUndoState               = kIdle;
	itsMaxUndoCount            = source.itsMaxUndoCount;
	itsActiveFlag              = kJFalse;
	itsSelActiveFlag           = kJFalse;
	itsCaretVisibleFlag        = kJFalse;
	itsPerformDNDFlag          = source.itsPerformDNDFlag;
	itsAutoIndentFlag          = source.itsAutoIndentFlag;
	itsTabToSpacesFlag         = source.itsTabToSpacesFlag;
	itsMoveToFrontOfTextFlag   = source.itsMoveToFrontOfTextFlag;
	itsBcastLocChangedFlag     = source.itsBcastLocChangedFlag;
	itsBcastAllTextChangedFlag = source.itsBcastAllTextChangedFlag;
	itsBreakCROnlyFlag         = source.itsBreakCROnlyFlag;
	itsIsPrintingFlag          = kJFalse;

	itsDefFont         = source.itsDefFont;
	itsWidth           = source.itsWidth;
	itsHeight          = source.itsHeight;
	itsGUIWidth        = source.itsGUIWidth;
	itsLeftMarginWidth = source.itsLeftMarginWidth;
	itsDefTabWidth     = source.itsDefTabWidth;
	itsMaxWordWidth    = source.itsMaxWordWidth;

	itsLineStarts = new JArray<JIndex>(*(source.itsLineStarts));
	assert( itsLineStarts != NULL );

	itsLineWidths = new JArray<JCoordinate>(*(source.itsLineWidths));
	assert( itsLineWidths != NULL );

	itsLineGeom = new JRunArray<LineGeometry>(*(source.itsLineGeom));
	assert( itsLineGeom != NULL );

	itsPrevBufLength = source.itsPrevBufLength;

	itsCaretLoc         = CaretLocation(1,1);
	itsCaretX           = 0;
	itsInsertionFont    = CalcInsertionFont(1);

	itsClipText         = NULL;
	itsClipStyle        = NULL;

	itsPrevDragType     = itsDragType = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;

	itsCRMLineWidth     = source.itsCRMLineWidth;
	itsCRMTabCharCount  = source.itsCRMTabCharCount;

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
	if (source.itsCRMRuleList != NULL)
		{
		SetCRMRuleList(source.itsCRMRuleList, source.itsOwnsCRMRulesFlag);
		}

	itsCharInWordFn   = source.itsCharInWordFn;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTextEditor16::~JTextEditor16()
{
	delete itsBuffer;
	delete itsStyles;
	delete itsUndo;
	delete itsLineStarts;
	delete itsLineWidths;
	delete itsLineGeom;
	delete itsUndoList;

	delete itsClipText;
	delete itsClipStyle;

	ClearCRMRuleList();
}

/******************************************************************************
 SetType (protected)

	This is protected because most derived classes will not be written
	to expect the type to change.

	Broadcasts TypeChanged.

 ******************************************************************************/

void
JTextEditor16::SetType
	(
	const Type type
	)
{
	itsType = type;
	TERefresh();
	Broadcast(TypeChanged(type));
}

/******************************************************************************
 SetText16

	Returns kJFalse if illegal characters had to be removed.

	This is not accessible to the user, so we don't provide Undo.

 ******************************************************************************/

JBoolean
JTextEditor16::SetText16
	(
	const JString16&			text,
	const JRunArray<Font>*	style
	)
{
	*itsBuffer = text;
	return SetText1(style);
}

JBoolean
JTextEditor16::SetText16
	(
	const JCharacter16*		text,
	const JRunArray<Font>*	style
	)
{
	*itsBuffer = text;
	return SetText1(style);
}

/******************************************************************************
 SetText1 (private)

	Returns kJFalse if illegal characters had to be removed.

	style can be NULL or itsStyles.

 ******************************************************************************/

JBoolean
JTextEditor16::SetText1
	(
	const JRunArray<Font>* style
	)
{
	if (TEIsDragging())
		{
		return kJTrue;
		}

	ClearUndo();

	JBoolean cleaned = kJFalse;

	if (style != NULL)
		{
		assert( itsBuffer->GetLength() == style->GetElementCount() );
		*itsStyles = *style;
		cleaned = itsReplaceIllegalChars ?
							ReplaceIllegalChars(itsBuffer, itsStyles) :
							RemoveIllegalChars(itsBuffer, itsStyles);
		}
	else
		{
		cleaned = itsReplaceIllegalChars ?
							ReplaceIllegalChars(itsBuffer) :
							RemoveIllegalChars(itsBuffer);

		itsStyles->RemoveAll();
		if (!itsBuffer->IsEmpty())
			{
			itsStyles->AppendElements(itsDefFont, itsBuffer->GetLength());
			}
		else
			{
			itsInsertionFont = itsDefFont;
			}
		}

	if (NeedsToFilterText(*itsBuffer))
		{
		cleaned = kJTrue;
		if (!FilterText(itsBuffer, itsStyles))
			{
			itsBuffer->Clear();
			itsStyles->RemoveAll();
			}
		}

	RecalcAll(kJTrue);
	SetCaretLocation(1);
	Broadcast(TextSet());
	return JNegate(cleaned);
}

/******************************************************************************
 ReadPlainText

	If acceptBinaryFile == kJTrue, returns kJFalse if illegal characters had to
	be removed.  If acceptBinaryFile == kJFalse, returns kJFalse without loading
	the file if the file contains illegal characters.

	We don't call SetText to avoid making two copies of the file's data.
	(The file could be very large.)

 ******************************************************************************/

#define CONVERT_DOS_MEMORY	0
#define CONVERT_DOS_FILE	1

static const JCharacter16  kUNIXNewline[]        = {'\n', 0};
static const JCharacter16  kUNIXNewlineChar      = '\n';
static const JCharacter16  kMacintoshNewline[]   = {'\r', 0};
static const JCharacter16  kMacintoshNewlineChar = '\r';
static const JCharacter16  kDOSNewline[]        = {'\r', '\n', 0};
static const JCharacter16  k1stDOSNewlineChar    = '\r';
static const JCharacter16  k2ndDOSNewlineChar    = '\n';

JBoolean
JTextEditor16::ReadPlainText
	(
	const JCharacter*	fileName,
	PlainTextFormat*	format,
	const JBoolean		acceptBinaryFile
	)
{
	TEDisplayBusyCursor();

#ifdef _TODO
	JReadFile(fileName, itsBuffer);
#endif

	JIndex i;
	if (ContainsIllegalChars(*itsBuffer))
		{
		if (!acceptBinaryFile)
			{
			return kJFalse;
			}

		// It is probably a binary file, so we shouldn't mess with it.

		*format = kUNIXText;
		}

	else if (itsBuffer->LocateLastSubstring(kMacintoshNewline, &i))
		{
		// We work from the end since DOS strings will shrink.

		if (i < itsBuffer->GetLength() &&
			itsBuffer->GetCharacter(i+1) == k2ndDOSNewlineChar)
			{
			*format = kDOSText;

			const JSize origLength = itsBuffer->GetLength();
			JLatentPG pg(100);
			pg.FixedLengthProcessBeginning(origLength,
					"Converting from DOS format...", kJFalse, kJFalse);

			#if CONVERT_DOS_MEMORY

			do		// this is more than 100 times slower!
				{
				itsBuffer->ReplaceSubstring(i,i+1, kUNIXNewline);
				pg.IncrementProgress(origLength - i - pg.GetCurrentStepCount());
				}
				while (itsBuffer->LocatePrevSubstring(kDOSNewline, &i));

			#elif CONVERT_DOS_FILE

			const JSize saveSize = itsBuffer->GetBlockSize();
			itsBuffer->SetBlockSize(origLength);

			itsBuffer->Clear();
			ifstream input(fileName);
			while (1)
				{
				const JCharacter c = input.get();
				if (input.eof() || input.fail())
					{
					break;
					}

				if (c != k1stDOSNewlineChar)
					{
					itsBuffer->AppendCharacter(c);
					}
				else
					{
					pg.IncrementProgress(itsBuffer->GetLength() - pg.GetCurrentStepCount());
					}
				}
			input.close();

			itsBuffer->SetBlockSize(saveSize);

			#endif

			pg.ProcessFinished();
			}

		else
			{
			*format = kMacintoshText;

			const JCharacter16* s = itsBuffer->GetCString();
			while (i > 0)
				{
				i--;
				if (s[i] == kMacintoshNewlineChar)
					{
					itsBuffer->SetCharacter(i+1, kUNIXNewlineChar);
					}
				}
			}
		}

	else
		{
		*format = kUNIXText;
		}

	return SetText1(NULL);
}

/******************************************************************************
 WritePlainText

 ******************************************************************************/

void
JTextEditor16::WritePlainText
	(
	const JCharacter*		fileName,
	const PlainTextFormat	format
	)
	const
{
	ofstream output(fileName);
	WritePlainText(output, format);
}

void
JTextEditor16::WritePlainText
	(
	ostream&				output,
	const PlainTextFormat	format
	)
	const
{
	if (format == kUNIXText)
		{
		itsBuffer->Print(output);
		return;
		}

	const JCharacter16* newlineStr = NULL;
	if (format == kDOSText)
		{
		newlineStr = kDOSNewline;
		}
	else if (format == kMacintoshText)
		{
		newlineStr = kMacintoshNewline;
		}
	assert( newlineStr != NULL );

	const JCharacter16* buffer = itsBuffer->GetCString();
	const JSize bufLength    = itsBuffer->GetLength();
	JIndex start             = 0;
	for (JIndex i=0; i<bufLength; i++)
		{
		if (buffer[i] == '\n')
			{
			if (start < i)
				{
				output.write((const char*)(buffer + start), (i - start) * sizeof(JCharacter16));
				}
			output << newlineStr;
			start = i+1;
			}
		}

	if (start < bufLength)
		{
		output.write((const char*)(buffer + start), (bufLength - start) * sizeof(JCharacter16));
		}
}

/******************************************************************************
 ReadPrivateFormat

	See WritePrivateFormat() for version information.

 ******************************************************************************/

JBoolean
JTextEditor16::ReadPrivateFormat
	(
	istream& input
	)
{
	TEDisplayBusyCursor();

	JString16 text;
	JRunArray<Font> style;
	if (ReadPrivateFormat(input, this, &text, &style))
		{
		*itsBuffer = text;
		SetText1(&style);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 ReadPrivateFormat (static protected)

	See WritePrivateFormat() for version information.

 ******************************************************************************/

JBoolean
JTextEditor16::ReadPrivateFormat
	(
	istream&			input,
	const JTextEditor16*	te,
	JString16*			text,
	JRunArray<Font>*	style
	)
{
JIndex i;

	text->Clear();
	style->RemoveAll();

	// version

	JFileVersion vers;
	input >> vers;

	if (vers > kCurrentPrivateFormatVersion)
		{
		return kJFalse;
		}

	// text

	JSize textLength;
	input >> textLength;
	input.ignore(1);
	if (textLength > 0)
		{
		text->Read(input, textLength);
		}

	// list of font names

	JSize fontCount;
	input >> fontCount;

	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	for (i=1; i<=fontCount; i++)
		{
		JString* name = new JString;
		assert( name != NULL );
		input >> *name;
		fontList.Append(name);
		}

	// list of rgb values

	JSize colorCount;
	input >> colorCount;

	JArray<JColorIndex> colorList;

	JRGB color;
	for (i=1; i<=colorCount; i++)
		{
		input >> color;
		colorList.AppendElement(te->RGBToColorIndex(color));
		}

	// styles

	JSize runCount;
	input >> runCount;

	const JFontManager* fontMgr = te->TEGetFontManager();

	for (i=1; i<=runCount; i++)
		{
		JSize charCount;
		input >> charCount;

		JIndex fontIndex;
		input >> fontIndex;

		Font f;
		input >> f.size;
		input >> f.style.bold >> f.style.italic >> f.style.strike;
		input >> f.style.underlineCount;

		JIndex colorIndex;
		input >> colorIndex;
		f.style.color = colorList.GetElement(colorIndex);

		const JString* name = fontList.NthElement(fontIndex);
		f.id = fontMgr->GetFontID(*name, f.size, f.style);

		style->AppendElements(f, charCount);
		}

	return kJTrue;
}

/******************************************************************************
 WritePrivateFormat

 ******************************************************************************/

void
JTextEditor16::WritePrivateFormat
	(
	ostream& output
	)
	const
{
	if (!IsEmpty())
		{
		WritePrivateFormat(output, kCurrentPrivateFormatVersion,
						   1, itsBuffer->GetLength());
		}
	else
		{
		output << kCurrentPrivateFormatVersion;
		output << " 0 0 0 0";
		}
}

/******************************************************************************
 WritePrivateFormat (protected)

 ******************************************************************************/

void
JTextEditor16::WritePrivateFormat
	(
	ostream&			output,
	const JFileVersion	vers,
	const JIndex		startIndex,
	const JIndex		endIndex
	)
	const
{
	WritePrivateFormat(output, itsFontMgr, itsColormap, vers,
					   *itsBuffer, *itsStyles, startIndex, endIndex);
}

void
JTextEditor16::WritePrivateFormat
	(
	ostream&				output,
	const JFileVersion		vers,
	const JString16&			text,
	const JRunArray<Font>&	style
	)
	const
{
	if (!text.IsEmpty() && text.GetLength() == style.GetElementCount())
		{
		WritePrivateFormat(output, itsFontMgr, itsColormap, vers,
						   text, style, 1, text.GetLength());
		}
}

/******************************************************************************
 WriteClipboardPrivateFormat (protected)

	Returns kJFalse if the clipboard is empty.

 ******************************************************************************/

JBoolean
JTextEditor16::WriteClipboardPrivateFormat
	(
	ostream&			output,
	const JFileVersion	vers
	)
	const
{
	if (itsClipText != NULL && itsClipStyle != NULL &&
		!itsClipText->IsEmpty())
		{
		WritePrivateFormat(output, itsFontMgr, itsColormap, vers, *itsClipText,
						   *itsClipStyle, 1, itsClipText->GetLength());
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 WritePrivateFormat (static)

	We have to be able to write each version because this is what we
	put on the clipboard.

	version 1:  initial version.

 ******************************************************************************/

void
JTextEditor16::WritePrivateFormat
	(
	ostream&				output,
	const JFontManager*		fontMgr,
	const JColormap*		colormap,
	const JFileVersion		vers,
	const JString16&			text,
	const JRunArray<Font>&	style,
	const JIndex			startIndex,
	const JIndex			endIndex
	)
{
	assert( text.IndexValid(startIndex) );
	assert( text.IndexValid(endIndex) );
	assert( startIndex <= endIndex );
	assert( vers <= kCurrentPrivateFormatVersion );

	// write version

	output << vers;

	// write text efficiently

	const JSize textLength = endIndex-startIndex+1;
	output << ' ' << textLength << ' ';
	output.write((const char*)(text.GetCString() + startIndex-1), textLength * sizeof(JCharacter16));

	// build lists of font names and colors

	JSize styleRunCount = 0;
	JPtrArray<JString> fontList(JPtrArrayT::kDeleteAll);
	fontList.SetCompareFunction(JCompareStringsCaseSensitive);
	JArray<JColorIndex> colorList;
	colorList.SetCompareFunction(JCompareIndices);

	JIndex startRunIndex, startFirstInRun;
	JBoolean found = style.FindRun(startIndex, &startRunIndex, &startFirstInRun);
	assert( found );

	JIndex i          = startIndex;
	JIndex runIndex   = startRunIndex;
	JIndex firstInRun = startFirstInRun;
	do
		{
		const Font& f    = style.GetRunDataRef(runIndex);
		JString fontName = fontMgr->GetFontName(f.id);
		const JIndex fontIndex =
			fontList.SearchSorted1(&fontName, JOrderedSetT::kAnyMatch, &found);
		if (!found)
			{
			fontList.InsertAtIndex(fontIndex, fontName);
			}

		const JIndex colorIndex =
			colorList.SearchSorted1(f.style.color, JOrderedSetT::kAnyMatch, &found);
		if (!found)
			{
			colorList.InsertElementAtIndex(colorIndex, f.style.color);
			}

		i += style.GetRunLength(runIndex) - (i - firstInRun);
		runIndex++;
		firstInRun = i;
		styleRunCount++;
		}
		while (i <= endIndex);

	// write list of font names

	const JSize fontCount = fontList.GetElementCount();
	output << ' ' << fontCount;

	for (i=1; i<=fontCount; i++)
		{
		output << ' ' << *(fontList.NthElement(i));
		}

	// write list of rgb values

	const JSize colorCount = colorList.GetElementCount();
	output << ' ' << colorCount;

	for (i=1; i<=colorCount; i++)
		{
		output << ' ' << colormap->GetRGB(colorList.GetElement(i));
		}

	// write styles

	output << ' ' << styleRunCount;

	i          = startIndex;
	runIndex   = startRunIndex;
	firstInRun = startFirstInRun;
	do
		{
		JSize charCount = style.GetRunLength(runIndex) - (i - firstInRun);
		if (endIndex < i + charCount - 1)
			{
			charCount = endIndex - i + 1;
			}

		const Font& f    = style.GetRunDataRef(runIndex);
		JString fontName = fontMgr->GetFontName(f.id);

		JIndex fontIndex;
		found = fontList.SearchSorted(&fontName, JOrderedSetT::kAnyMatch, &fontIndex);
		assert( found );

		JIndex colorIndex;
		found = colorList.SearchSorted(f.style.color, JOrderedSetT::kAnyMatch, &colorIndex);
		assert( found );

		output << ' ' << charCount;
		output << ' ' << fontIndex;
		output << ' ' << f.size;
		output << ' ' << f.style.bold << f.style.italic << f.style.strike;
		output << ' ' << f.style.underlineCount;
		output << ' ' << colorIndex;

		i += charCount;
		runIndex++;
		firstInRun = i;
		}
		while (i <= endIndex);
}

/******************************************************************************
 ColorNameToColorIndex (private)

 ******************************************************************************/

JColorIndex
JTextEditor16::ColorNameToColorIndex
	(
	const JCharacter* name
	)
	const
{
	return (const_cast<JTextEditor16*>(this))->itsCustomColors.Add(name);
}

/******************************************************************************
 RGBToColorIndex (private)

 ******************************************************************************/

JColorIndex
JTextEditor16::RGBToColorIndex
	(
	const JRGB& color
	)
	const
{
	return (const_cast<JTextEditor16*>(this))->itsCustomColors.Add(color);
}

/******************************************************************************
 CalcWSFont (private)

	Calculates the appropriate style for whitespace between two styled words.
	We don't recalculate the font id because we only change underline and strike.

 ******************************************************************************/

JTextEditor16::Font
JTextEditor16::CalcWSFont
	(
	const Font& prevFont,
	const Font& nextFont
	)
	const
{
	Font f = nextFont;

	const JBoolean ulMatch =
		JI2B( prevFont.style.underlineCount == nextFont.style.underlineCount );

	const JBoolean sMatch =
		JI2B( prevFont.style.strike == nextFont.style.strike );

	if (!ulMatch && !sMatch &&
		prevFont.style.underlineCount == 0 && !prevFont.style.strike)
		{
		f = prevFont;
		}
	else if (!ulMatch && !sMatch &&
			 nextFont.style.underlineCount == 0 && !nextFont.style.strike)
		{
		// f = nextFont;
		}
	else if (!ulMatch && !sMatch)
		{
		f.style.underlineCount = 0;
		f.style.strike         = kJFalse;
		}
	else if (!ulMatch && prevFont.style.underlineCount == 0)
		{
		f = prevFont;
		}
	else if (!ulMatch && nextFont.style.underlineCount == 0)
		{
		// f = nextFont;
		}
	else if (!ulMatch)
		{
		f.style.underlineCount = 0;
		}
	else if (!sMatch && !prevFont.style.strike)
		{
		f = prevFont;
		}
	else if (!sMatch && !nextFont.style.strike)
		{
		// f = nextFont;
		}

	return f;
}

/******************************************************************************
 Search and replace

 ******************************************************************************/

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor16::SearchForward
	(
	const JCharacter16*	searchStr,
	const JBoolean		caseSensitive,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	JIndex startIndex;
	if (!itsSelection.IsEmpty())
		{
		startIndex = itsSelection.last + 1;
		}
	else
		{
		startIndex = itsCaretLoc.charIndex;
		}

	const JIndex origStartIndex = startIndex;

	*wrapped = kJFalse;
	const JSize bufLength = itsBuffer->GetLength();
	if (startIndex > bufLength && wrapSearch)
		{
		startIndex = 1;
		*wrapped   = kJTrue;
		}
	else if (startIndex > bufLength)
		{
		return kJFalse;
		}

	const JSize searchLength = strlen16(searchStr);

	JBoolean found = kJFalse;
	while (1)
		{
		found = itsBuffer->LocateNextSubstring(searchStr, searchLength,
											   caseSensitive, &startIndex);
		if (found && entireWord)
			{
			found = IsEntireWord(startIndex, startIndex + searchLength - 1);
			}

		if (found)
			{
			break;
			}

		startIndex++;
		if (!found && startIndex > bufLength && wrapSearch && !(*wrapped) &&
			origStartIndex > 1)
			{
			startIndex = 1;
			*wrapped   = kJTrue;
			}
		else if ((!found && startIndex > bufLength) ||
				 (!found && *wrapped && startIndex >= origStartIndex))
			{
			break;
			}
		}

	if (found)
		{
		StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
		SetSelection(startIndex, startIndex + searchLength - 1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor16::SearchBackward
	(
	const JCharacter16*	searchStr,
	const JBoolean		caseSensitive,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	const JSize searchLength = strlen16(searchStr);
	if (searchLength == 0)
		{
		return kJFalse;
		}

	JIndex startIndex;
	if (itsSelection.IsEmpty())
		{
		startIndex = itsCaretLoc.charIndex - 1;
		}
	else
		{
		const JBoolean selectedMatch =
			JCompareMaxN(itsBuffer->GetCString() + itsSelection.first-1, searchStr, searchLength);
		if (selectedMatch && itsSelection.first > searchLength)
			{
			startIndex = itsSelection.first - searchLength;
			}
		else if (selectedMatch)
			{
			startIndex = 0;
			}
		else
			{
			startIndex = itsSelection.first - 1;
			}
		}

	const JIndex origStartIndex = startIndex;

	*wrapped = kJFalse;
	const JSize bufLength = itsBuffer->GetLength();
	if (startIndex == 0 && wrapSearch)
		{
		startIndex = bufLength;
		*wrapped   = kJTrue;
		}
	else if (startIndex == 0)
		{
		return kJFalse;
		}

	JBoolean found = kJFalse;
	while (1)
		{
		found = itsBuffer->LocatePrevSubstring(searchStr, searchLength,
											   caseSensitive, &startIndex);
		if (found && entireWord)
			{
			found = IsEntireWord(startIndex, startIndex + searchLength - 1);
			}

		if (found)
			{
			break;
			}

		if (startIndex > 0)
			{
			startIndex--;
			}
		if (!found && startIndex == 0 && wrapSearch && !(*wrapped) &&
			origStartIndex < bufLength)
			{
			startIndex = bufLength;
			*wrapped   = kJTrue;
			}
		else if ((!found && startIndex == 0) ||
				 (!found && *wrapped && startIndex <= origStartIndex))
			{
			break;
			}
		}

	if (found)
		{
		StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
		SetSelection(startIndex, startIndex + searchLength - 1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SelectionMatches

	Returns kJTrue if the current selection matches the given search criteria.
	This should always be checked before doing a replace.

 ******************************************************************************/

JBoolean
JTextEditor16::SelectionMatches
	(
	const JCharacter16*	searchStr,
	const JBoolean		caseSensitive,
	const JBoolean		entireWord
	)
{
	const JSize searchLength = strlen16(searchStr);
	if (itsSelection.IsEmpty() ||
		itsSelection.GetLength() != searchLength ||
		(entireWord && !IsEntireWord(itsSelection)))
		{
		return kJFalse;
		}

	JIndex startIndex = itsSelection.first;
	return JConvertToBoolean(
		itsBuffer->LocateNextSubstring(searchStr, searchLength,
									   caseSensitive, &startIndex) &&
		startIndex == itsSelection.first);
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the given character range is a single, complete word.

 ******************************************************************************/

JBoolean
JTextEditor16::IsEntireWord
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
	const
{
	if ((startIndex > 1 && IsCharacterInWord(*itsBuffer, startIndex-1)) ||
		(endIndex < itsBuffer->GetLength() &&
		 IsCharacterInWord(*itsBuffer, endIndex+1)))
		{
		return kJFalse;
		}

	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		if (!IsCharacterInWord(*itsBuffer, i))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 ReplaceAllForward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

	If !searchRange.IsNothing(), the search is restricted to searchRange.

 ******************************************************************************/

JBoolean
JTextEditor16::ReplaceAllForward
	(
	const JCharacter16*	searchStr,
	const JBoolean		searchIsRegex,
	const JBoolean		caseSensitive,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	const JCharacter16*	replaceStr,
	const JIndexRange&	origSearchRange
	)
{
	const JSize searchStrLength = strlen16(searchStr);

	JArray<JIndexRange> submatchList;
		{
		submatchList.AppendElement(JIndexRange());
		}

	JIndexRange searchRange       = origSearchRange;
	const JBoolean useSearchRange = JNegate(searchRange.IsNothing());
	if (useSearchRange)
		{
		SetCaretLocation(searchRange.first);
		}
	else if (wrapSearch)
		{
		SetCaretLocation(1);
		}
	else
		{
		SetCaretLocation(GetInsertionIndex());
		}

	JLatentPG pg(50);
	pg.VariableLengthProcessBeginning("Replacing text...", kJTrue, kJFalse);

	JBoolean foundAny = kJFalse;
	JBoolean found, wrapped;
	do
		{
			{
			found = SearchForward(searchStr, caseSensitive, entireWord,
								  kJFalse, &wrapped);
			submatchList.SetElement(1, JIndexRange(1, searchStrLength));
			}

		if (found)
			{
			if (useSearchRange && !searchRange.Contains(itsSelection))
				{
				break;
				}
			const JSize origLength = GetTextLength();

			foundAny = kJTrue;
			ReplaceSelection(replaceStr, caseSensitive, submatchList);

			if (useSearchRange)
				{
				searchRange.last += GetTextLength() - origLength;
				}
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}
		while (found);

	pg.ProcessFinished();

	return foundAny;
}

/******************************************************************************
 ReplaceAllBackward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns kJTrue if it replaced anything.

 ******************************************************************************/

JBoolean
JTextEditor16::ReplaceAllBackward
	(
	const JCharacter16*	searchStr,
	const JBoolean		searchIsRegex,
	const JBoolean		caseSensitive,
	const JBoolean		entireWord,
	const JBoolean		wrapSearch,
	const JCharacter16*	replaceStr
	)
{
	const JSize searchStrLength = strlen16(searchStr);

	JArray<JIndexRange> submatchList;
		{
		submatchList.AppendElement(JIndexRange());
		}

	JIndex selStart, selEnd;
	if (wrapSearch)
		{
		SetCaretLocation(GetTextLength()+1);
		}
	else if (GetSelection(&selStart, &selEnd))
		{
		SetCaretLocation(selStart+1);
		}

	JLatentPG pg(10);	// MatchBackward() is slow
	pg.VariableLengthProcessBeginning("Replacing text...", kJTrue, kJFalse);

	JBoolean foundAny = kJFalse;
	JBoolean found, wrapped;
	do
		{
			{
			found = SearchBackward(searchStr, caseSensitive, entireWord,
								   kJFalse, &wrapped);
			submatchList.SetElement(1, JIndexRange(1, searchStrLength));
			}

		if (found)
			{
			foundAny = kJTrue;
			ReplaceSelection(replaceStr, caseSensitive, submatchList);
			}

		if (!pg.IncrementProgress())
			{
			break;
			}
		}
		while (found);

	pg.ProcessFinished();

	return foundAny;
}

/******************************************************************************
 ReplaceSelection (protected)

	Replace the current selection with the given replace text.

	*** Something must be selected.
		We do not check that it matches the search string.

 ******************************************************************************/

void
JTextEditor16::ReplaceSelection
	(
	const JCharacter16*			replaceStr,
	const JBoolean				preserveCase,
	const JArray<JIndexRange>&	submatchList
	)
{
	assert( HasSelection() );

	JString16 replaceText;
		{
		replaceText = replaceStr;
		if (preserveCase)
			{
			JString16 sourceText;
			const JBoolean ok = GetSelection(&sourceText);
			assert( ok );
			replaceText.MatchCase(sourceText, submatchList.GetElement(1));
			}
		}

	const JIndex selStart = GetInsertionIndex();
	Paste(replaceText);
	if (!replaceText.IsEmpty())
		{
		StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
		SetSelection(selStart, selStart + replaceText.GetLength()-1);
		}
}

/******************************************************************************
 AdjustRangesForReplace (private)

	Shift all the ranges by the same amount so that the first one starts
	at index 1.

 ******************************************************************************/

void
JTextEditor16::AdjustRangesForReplace
	(
	JArray<JIndexRange>* list
	)
{
	const JInteger delta = (list->GetElement(1)).first - 1;
	assert( delta >= 0 );

	if (delta > 0)
		{
		const JSize count = list->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JIndexRange r = list->GetElement(i);
			r -= delta;
			list->SetElement(i, r);
			}
		}
}

/******************************************************************************
 SearchForward

	Look for the next match beyond the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor16::SearchForward
	(
	const FontMatch&	match,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	JIndex startIndex;
	if (!itsSelection.IsEmpty())
		{
		startIndex = itsSelection.last + 1;
		}
	else
		{
		startIndex = itsCaretLoc.charIndex;
		}

	const JSize runCount = itsStyles->GetRunCount();
	JIndex endRun        = runCount;

	*wrapped = kJFalse;
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex > firstIndexInRun)
				{
				firstIndexInRun += itsStyles->GetRunLength(startRun);
				startRun++;
				}

			for (JIndex i=startRun; i<=endRun; i++)
				{
				const JTextEditor16::Font& f = itsStyles->GetRunDataRef(i);
				const JSize runLength      = itsStyles->GetRunLength(i);
				if (match.Match(itsFontMgr->GetFontName(f.id), f.size, f.style))
					{
					StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
					SetSelection(firstIndexInRun, firstIndexInRun + runLength-1);
					return kJTrue;
					}

				firstIndexInRun += runLength;
				}

			if (wrapSearch && !(*wrapped) && origStartRun > 1)
				{
				startIndex = 1;
				endRun     = origStartRun-1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = 1;
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 SearchBackward

	Look for the match before the current position.
	If we find it, we select it and return kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor16::SearchBackward
	(
	const FontMatch&	match,
	const JBoolean		wrapSearch,
	JBoolean*			wrapped
	)
{
	JIndex startIndex;
	if (!itsSelection.IsEmpty())
		{
		startIndex = itsSelection.first;
		}
	else
		{
		startIndex = itsCaretLoc.charIndex;
		}
	startIndex--;

	JIndex endRun = 1;

	*wrapped = kJFalse;
	while (1)
		{
		JIndex runIndex, firstIndexInRun;
		if (itsStyles->FindRun(startIndex, &runIndex, &firstIndexInRun))
			{
			const JIndex origStartRun = runIndex;
			JIndex startRun           = runIndex;
			if (startIndex < firstIndexInRun + itsStyles->GetRunLength(startRun)-1)
				{
				startRun--;
				if (startRun > 0)
					{
					firstIndexInRun -= itsStyles->GetRunLength(startRun);
					}
				}

			for (JIndex i=startRun; i>=endRun; i--)
				{
				const JTextEditor16::Font& f = itsStyles->GetRunDataRef(i);
				if (match.Match(itsFontMgr->GetFontName(f.id), f.size, f.style))
					{
					StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
					SetSelection(firstIndexInRun,
								 firstIndexInRun + itsStyles->GetRunLength(i)-1);
					return kJTrue;
					}

				if (i > endRun)
					{
					firstIndexInRun -= itsStyles->GetRunLength(i-1);
					}
				}

			if (wrapSearch && !(*wrapped) && origStartRun < itsStyles->GetRunCount())
				{
				startIndex = GetTextLength();
				endRun     = origStartRun+1;
				*wrapped   = kJTrue;
				}
			else
				{
				break;
				}
			}

		else if (wrapSearch && !(*wrapped))
			{
			startIndex = GetTextLength();
			*wrapped   = kJTrue;
			}
		else
			{
			break;
			}
		}

	return kJFalse;
}

/******************************************************************************
 FontMatch destructor

 ******************************************************************************/

JTextEditor16::FontMatch::~FontMatch()
{
}

/******************************************************************************
 SetBreakCROnly

 ******************************************************************************/

void
JTextEditor16::SetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	if (breakCROnly != itsBreakCROnlyFlag)
		{
		PrivateSetBreakCROnly(breakCROnly);
		RecalcAll(kJFalse);
		}
}

// private

void
JTextEditor16::PrivateSetBreakCROnly
	(
	const JBoolean breakCROnly
	)
{
	itsBreakCROnlyFlag = breakCROnly;
	if (!itsBreakCROnlyFlag)
		{
		itsWidth = itsGUIWidth;
		}
}

/******************************************************************************
 TEGetMinPreferredGUIWidth

 ******************************************************************************/

JCoordinate
JTextEditor16::TEGetMinPreferredGUIWidth()
	const
{
	if (itsBreakCROnlyFlag)
		{
		return itsLeftMarginWidth + itsWidth + kRightMarginWidth;
		}
	else
		{
		return itsLeftMarginWidth + itsMaxWordWidth + kRightMarginWidth;
		}
}

/******************************************************************************
 TESetBoundsWidth (protected)

	Call this to notify us of a change in aperture width.

 ******************************************************************************/

void
JTextEditor16::TESetBoundsWidth
	(
	const JCoordinate width
	)
{
	itsGUIWidth = width - itsLeftMarginWidth - kRightMarginWidth;
	assert( itsGUIWidth > 0 );
	TEGUIWidthChanged();
}

/******************************************************************************
 TESetLeftMarginWidth

	For multi-line input areas it helps to have a wide left margin where
	one can click and drag down to select entire lines.

 ******************************************************************************/

void
JTextEditor16::TESetLeftMarginWidth
	(
	const JCoordinate origWidth
	)
{
	const JCoordinate width = JMax((JCoordinate) kMinLeftMarginWidth, origWidth);

	if (width != itsLeftMarginWidth)
		{
		itsGUIWidth       += itsLeftMarginWidth;
		itsLeftMarginWidth = width;
		itsGUIWidth       -= itsLeftMarginWidth;
		TEGUIWidthChanged();
		}
}

/******************************************************************************
 TEGUIWidthChanged (private)

 ******************************************************************************/

void
JTextEditor16::TEGUIWidthChanged()
{
	if (itsBreakCROnlyFlag)
		{
		TERefresh();
		}
	else if (itsWidth != itsGUIWidth)
		{
		itsWidth = itsGUIWidth;
		RecalcAll(kJFalse);
		}
}

/******************************************************************************
 Get current font

 ******************************************************************************/

const JCharacter*
JTextEditor16::GetCurrentFontName()
	const
{
	const Font f = GetCurrentFont();
	return itsFontMgr->GetFontName(f.id);
}

JSize
JTextEditor16::GetCurrentFontSize()
	const
{
	const Font f = GetCurrentFont();
	return f.size;
}

JFontStyle
JTextEditor16::GetCurrentFontStyle()
	const
{
	const Font f = GetCurrentFont();
	return f.style;
}

JTextEditor16::AlignmentType
JTextEditor16::GetCurrentFontAlign()
	const
{
	const Font f = GetCurrentFont();
	return f.align;
}

void
JTextEditor16::GetCurrentFont
	(
	JString*	name,
	JSize*		size,
	JFontStyle*	style,
	AlignmentType*	align
	)
	const
{
	const Font f = GetCurrentFont();
	*name  = itsFontMgr->GetFontName(f.id);
	*size  = f.size;
	*style = f.style;
	*align = f.align;
}

void
JTextEditor16::GetCurrentFont
	(
	JFontID*	id,
	JSize*		size,
	JFontStyle*	style,
	AlignmentType*	align
	)
	const
{
	const Font f = GetCurrentFont();
	*id    = f.id;
	*size  = f.size;
	*style = f.style;
	*align = f.align;
}

// protected

JTextEditor16::Font
JTextEditor16::GetCurrentFont()
	const
{
	if (!itsSelection.IsEmpty())
		{
		return itsStyles->GetElement(itsSelection.first);
		}
	else
		{
		return itsInsertionFont;
		}
}

/******************************************************************************
 Set current font

 ******************************************************************************/

void
JTextEditor16::SetCurrentFontName
	(
	const JCharacter* name
	)
{
	if (!itsSelection.IsEmpty())
		{
		JBoolean isNew;
		JTEUndoStyle16* undo = GetStyleUndo(&isNew);

		const JBoolean changed =
			SetFontName(itsSelection.first, itsSelection.last, name, kJFalse);

		if (changed)
			{
			NewUndo(undo, isNew);
			}
		else if (isNew)
			{
			delete undo;
			}
		}
	else
		{
		itsInsertionFont.id =
			itsFontMgr->GetFontID(name, itsInsertionFont.size, itsInsertionFont.style);
		}
}

void
JTextEditor16::SetCurrentFontSize
	(
	const JSize size
	)
{
	#define LocalVarName      size
	#define StructElementName size
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontBold
	(
	const JBoolean bold
	)
{
	#define LocalVarName      bold
	#define StructElementName style.bold
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontItalic
	(
	const JBoolean italic
	)
{
	#define LocalVarName      italic
	#define StructElementName style.italic
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontUnderline
	(
	const JSize count
	)
{
	#define LocalVarName      count
	#define StructElementName style.underlineCount
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontUnderlineType
	(
	const JFontStyle::EUnderlineType type
	)
{
	#define LocalVarName      type
	#define StructElementName style.underlineType
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
	
	// Set underline count as well
	if (type == JFontStyle::plain_Underline)
		SetCurrentFontUnderline(0);
	else
		SetCurrentFontUnderline(1);
}

void
JTextEditor16::SetCurrentFontStrike
	(
	const JBoolean strike
	)
{
	#define LocalVarName      strike
	#define StructElementName style.strike
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontColor
	(
	const JColorIndex color
	)
{
	#define LocalVarName      color
	#define StructElementName style.color
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontBackColor
	(
	const JColorIndex color
	)
{
	#define LocalVarName      color
	#define StructElementName style.backcolor
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontStyle
	(
	const JFontStyle& style
	)
{
	#define LocalVarName      style
	#define StructElementName style
	#include <JTESetCurrentFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetCurrentFontAlign
	(
	const AlignmentType align
	)
{
	// Get the range for the current paragraphs and set alignment for all of those
	JIndex start;
	JIndex stop;
	if (!itsSelection.IsEmpty())
	{
		start = GetParagraphStart(itsSelection.first);
		stop = GetParagraphEnd(itsSelection.last);
	}
	else
	{
		start = GetParagraphStart(itsCaretLoc.charIndex);
		stop = GetParagraphEnd(itsCaretLoc.charIndex);
	}
	
	if (start < stop)
	  SetRangeFontAlign(start, stop, align);
	else if (align != itsInsertionFont.align)
	  {
	    itsInsertionFont.align = align;
	    itsInsertionFont.id =
		itsFontMgr->UpdateFontID(itsInsertionFont.id, itsInsertionFont.size,
								 itsInsertionFont.style);

	  }
}

void
JTextEditor16::SetCurrentFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType align
	)
{
	SetCurrentFont(Font(itsFontMgr->GetFontID(name, size, style), size, style, align));
}

void
JTextEditor16::SetCurrentFont
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType align
	)
{
	SetCurrentFont(Font(id, size, style, align));
}

// protected

void
JTextEditor16::SetCurrentFont
	(
	const Font& f
	)
{
	if (!itsSelection.IsEmpty())
		{
		JBoolean isNew;
		JTEUndoStyle16* undo = GetStyleUndo(&isNew);
		SetFont(itsSelection.first, itsSelection.last, f, kJFalse);
		NewUndo(undo, isNew);
		}
	else
		{
		itsInsertionFont = f;
		}
}

/******************************************************************************
 Set range font

 ******************************************************************************/

void
JTextEditor16::SetRangeFontName
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JCharacter* name
	)
{
	JBoolean isNew;
	JTEUndoStyle16* undo = GetStyleUndo(&isNew);

	const JBoolean changed =
		SetFontName(startIndex, endIndex, name, kJFalse);

	if (changed)
		{
		NewUndo(undo, isNew);
		}
	else if (isNew)
		{
		delete undo;
		}
}

void
JTextEditor16::SetRangeFontSize
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JSize size
	)
{
	#define LocalVarName      size
	#define StructElementName size
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontBold
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JBoolean bold
	)
{
	#define LocalVarName      bold
	#define StructElementName style.bold
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontItalic
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JBoolean italic
	)
{
	#define LocalVarName      italic
	#define StructElementName style.italic
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontUnderline
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JSize count
	)
{
	#define LocalVarName      count
	#define StructElementName style.underlineCount
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontUnderlineType
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JFontStyle::EUnderlineType type
	)
{
	#define LocalVarName      type
	#define StructElementName style.underlineType
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
	
	// Set underline count as well
	SetRangeFontUnderline(startIndex, endIndex, (type == JFontStyle::plain_Underline) ? 0 : 1);
}

void
JTextEditor16::SetRangeFontStrike
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JBoolean strike
	)
{
	#define LocalVarName      strike
	#define StructElementName style.strike
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontColor
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JColorIndex color
	)
{
	#define LocalVarName      color
	#define StructElementName style.color
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontColor
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JRGB& color
	)
{
	SetRangeFontColor(startIndex, endIndex, RGBToColorIndex(color));
}

void
JTextEditor16::SetRangeFontBackColor
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JColorIndex color
	)
{
	#define LocalVarName      color
	#define StructElementName style.backcolor
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontBackColor
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JRGB& color
	)
{
	SetRangeFontBackColor(startIndex, endIndex, RGBToColorIndex(color));
}

void
JTextEditor16::SetRangeFontStyle
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JFontStyle& style
	)
{
	#define LocalVarName      style
	#define StructElementName style
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFontAlign
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const AlignmentType align
	)
{
	#define LocalVarName      align
	#define StructElementName align
	#define RangeStart		  startIndex
	#define RangeStop		  endIndex
	#include <JTESetFontRange16.th>
	#undef RangeStop
	#undef RangeStart
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetRangeFont
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType align
	)
{
	SetRangeFont(startIndex, endIndex, Font(itsFontMgr->GetFontID(name, size, style), size, style, align));
}

// protected

void
JTextEditor16::SetRangeFont
	(
	const JIndex startIndex,
	const JIndex endIndex,
	const Font& f
	)
{
	JBoolean isNew;
	JTEUndoStyle16* undo = GetStyleUndo(&isNew);
	SetFont(startIndex, endIndex, f, kJFalse);
	NewUndo(undo, isNew);
}

/******************************************************************************
 Get font

 ******************************************************************************/

const JCharacter*
JTextEditor16::GetFontName
	(
	const JIndex charIndex
	)
	const
{
	const Font f = itsStyles->GetElement(charIndex);
	return itsFontMgr->GetFontName(f.id);
}

JSize
JTextEditor16::GetFontSize
	(
	const JIndex charIndex
	)
	const
{
	const Font f = itsStyles->GetElement(charIndex);
	return f.size;
}

JFontStyle
JTextEditor16::GetFontStyle
	(
	const JIndex charIndex
	)
	const
{
	const Font f = itsStyles->GetElement(charIndex);
	return f.style;
}

void
JTextEditor16::GetFont
	(
	const JIndex	charIndex,
	JString*		name,
	JSize*			size,
	JFontStyle*		style
	)
	const
{
	const Font f = itsStyles->GetElement(charIndex);
	*name  = itsFontMgr->GetFontName(f.id);
	*size  = f.size;
	*style = f.style;
}

/******************************************************************************
 Set font

	Returns kJTrue if anything actually changed

 ******************************************************************************/

JBoolean
JTextEditor16::SetFontName
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JCharacter*	name,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	Font f;
	JBoolean changed = kJFalse;
	JRunArrayIterator<Font> iter(itsStyles, kJIteratorStartBefore, startIndex);
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		const JBoolean ok = iter.Next(&f);
		assert( ok );

		const JFontID newID = itsFontMgr->GetFontID(name, f.size, f.style);
		if (newID != f.id)
			{
			f.id = newID;
			iter.SetPrev(f);
			changed = kJTrue;
			}
		}

	if (changed)
		{
		Recalc(startIndex, endIndex - startIndex + 1, kJFalse);
		}

	return changed;
}

JBoolean
JTextEditor16::SetFontSize
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		size,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName      size
	#define StructElementName size
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontBold
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	bold,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName      bold
	#define StructElementName style.bold
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontItalic
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	italic,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName      italic
	#define StructElementName style.italic
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontUnderline
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JSize		count,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName      count
	#define StructElementName style.underlineCount
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontStrike
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	strike,
	const JBoolean	clearUndo

	)
{
	#define LocalVarName      strike
	#define StructElementName style.strike
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontColor
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JColorIndex	color,
	const JBoolean		clearUndo

	)
{
	#define LocalVarName      color
	#define StructElementName style.color
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontStyle
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JFontStyle&	style,
	const JBoolean		clearUndo
	)
{
	#define LocalVarName      style
	#define StructElementName style
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

JBoolean
JTextEditor16::SetFontAlign
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const AlignmentType	align,
	const JBoolean		clearUndo
	)
{
	#define LocalVarName      align
	#define StructElementName align
	#include <JTESetFont16.th>
	#undef LocalVarName
	#undef StructElementName
}

void
JTextEditor16::SetFont
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType	align,
	const JBoolean		clearUndo
	)
{
	SetFont(startIndex, endIndex,
			Font(itsFontMgr->GetFontID(name, size, style), size, style, align), clearUndo);
}

// protected

void
JTextEditor16::SetFont
	(
	const JIndex		startIndex,
	const JIndex		endIndex,
	const Font&			f,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	if (endIndex > startIndex)
		{
		const JSize charCount = endIndex - startIndex + 1;
		itsStyles->SetNextElements(startIndex, charCount, f);
		Recalc(startIndex, charCount, kJFalse);
		}
	else
		{
		assert( startIndex == endIndex );

		itsStyles->SetElement(startIndex, f);
		Recalc(startIndex, 1, kJFalse);
		}
}

// protected

void
JTextEditor16::SetFont
	(
	const JIndex			startIndex,
	const JRunArray<Font>&	fontList,
	const JBoolean			clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	Font f;
	JRunArrayIterator<Font> fIter(fontList);
	JRunArrayIterator<Font> sIter(itsStyles, kJIteratorStartBefore, startIndex);

	while (fIter.Next(&f) && sIter.SetNext(f))
		{
		sIter.SkipNext();
		}

	Recalc(startIndex, fontList.GetElementCount(), kJFalse);
}

/******************************************************************************
 SetAllFontNameAndSize (protected)

	This function is useful for unstyled text editors that allow the user
	to change the font and size.

	It preserves the styles, in case they are controlled by the program.
	(e.g. context sensitive hilighting)

	You can choose whether or not to throw out all Undo information.
	Unstyled text editors can usually preserve Undo, since they will not
	allow the user to modify styles.  (We explicitly ask for this because
	it is too easy to forget about the issue.)

 ******************************************************************************/

void
JTextEditor16::SetAllFontNameAndSize
	(
	const JCharacter*	name,
	const JSize			size,
	const JCoordinate	tabWidth,
	const JBoolean		breakCROnly,
	const JBoolean		clearUndo
	)
{
	if (clearUndo)
		{
		ClearUndo();
		}

	const JSize runCount = itsStyles->GetRunCount();
	for (JIndex i=1; i<=runCount; i++)
		{
		Font f = itsStyles->GetRunData(i);
		f.size = size;
		f.id   = itsFontMgr->GetFontID(name, f.size, f.style);
		itsStyles->SetRunData(i, f);
		}

	if (itsUndoList != NULL)
		{
		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=1; i<=undoCount; i++)
			{
			(itsUndoList->NthElement(i))->SetFont(name, size);
			}
		}
	else if (itsUndo != NULL)
		{
		itsUndo->SetFont(name, size);
		}

	itsInsertionFont.size = size;
	itsInsertionFont.id =
		itsFontMgr->GetFontID(name, itsInsertionFont.size, itsInsertionFont.style);

	SetDefaultFont(name, size, itsDefFont.style);

	itsDefTabWidth = tabWidth;
	PrivateSetBreakCROnly(breakCROnly);
	RecalcAll(kJFalse);
}

/******************************************************************************
 Get default font

 ******************************************************************************/

const JCharacter*
JTextEditor16::GetDefaultFontName()
	const
{
	return itsFontMgr->GetFontName(itsDefFont.id);
}

void
JTextEditor16::GetDefaultFont
	(
	JString*		name,
	JSize*			size,
	JFontStyle*		style,
	AlignmentType*	align
	)
	const
{
	*name  = GetDefaultFontName();
	*size  = itsDefFont.size;
	*style = itsDefFont.style;
	*align = itsDefFont.align;
}

/******************************************************************************
 Set default font

 ******************************************************************************/

void
JTextEditor16::SetDefaultFontName
	(
	const JCharacter* name
	)
{
	itsDefFont.id = itsFontMgr->GetFontID(name, itsDefFont.size, itsDefFont.style);
	if (itsBuffer->IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(1);
		}
}

void
JTextEditor16::SetDefaultFontSize
	(
	const JSize size
	)
{
	itsDefFont.size = size;
	itsDefFont.id   =
		itsFontMgr->UpdateFontID(itsDefFont.id, itsDefFont.size, itsDefFont.style);
	if (itsBuffer->IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(1);
		}
}

void
JTextEditor16::SetDefaultFontStyle
	(
	const JFontStyle& style
	)
{
	itsDefFont.style = style;
	itsDefFont.id    =
		itsFontMgr->UpdateFontID(itsDefFont.id, itsDefFont.size, itsDefFont.style);
	if (itsBuffer->IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(1);
		}
}

void
JTextEditor16::SetDefaultFontAlign
	(
	const AlignmentType align
	)
{
	itsDefFont.align = align;
	if (itsBuffer->IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(1);
		}
}

void
JTextEditor16::SetDefaultFont
	(
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType	align
	)
{
	itsDefFont.id    = id;
	itsDefFont.size  = size;
	itsDefFont.style = style;
	itsDefFont.align = align;

	if (itsBuffer->IsEmpty())
		{
		itsInsertionFont = CalcInsertionFont(1);
		}
}

/******************************************************************************
 Cut

 ******************************************************************************/

void
JTextEditor16::Cut()
{
	if (itsStoreClipFlag)
		{
		TECreateClipboard();

		if (Cut(itsClipText, itsClipStyle))
			{
			TEClipboardChanged();
			}
		}
	else if (!TEIsDragging())
		{
		TEClipboardChanged();
		DeleteSelection();
		DeactivateCurrentUndo();
		}
}

/******************************************************************************
 Cut

	Returns kJTrue if there was anything to cut.  style can be NULL.
	If function returns kJFalse, text and style are not modified.

 ******************************************************************************/

JBoolean
JTextEditor16::Cut
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
{
	if (!TEIsDragging() && Copy(text, style))
		{
		DeleteSelection();
		DeactivateCurrentUndo();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JTextEditor16::Copy()
{
	if (itsStoreClipFlag)
		{
		TECreateClipboard();

		if (Copy(itsClipText, itsClipStyle))
			{
			TEClipboardChanged();
			}
		}
	else
		{
		TEClipboardChanged();
		}
}

/******************************************************************************
 Copy

	Returns kJTrue if there was anything to copy.  style can be NULL.
	If function returns kJFalse, text and style are not modified, so
	we cannot call GetSelection().

 ******************************************************************************/

JBoolean
JTextEditor16::Copy
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	if (!itsSelection.IsEmpty())
		{
		*text = itsBuffer->GetSubstring(itsSelection);
		style->RemoveAll();
		style->InsertElementsAtIndex(1, *itsStyles, itsSelection.first,
									 itsSelection.GetLength());
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetClipboard

	Returns the text and style that would be pasted if Paste() were called.

	style can be NULL.  If it is not NULL, it can come back empty even if
	the function returns kJTrue.

 ******************************************************************************/

JBoolean
JTextEditor16::GetClipboard
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	if (TEOwnsClipboard() && itsClipText != NULL)
		{
		*text = *itsClipText;
		if (style != NULL)
			{
			*style = *itsClipStyle;
			}
		return kJTrue;
		}
	else if (style != NULL)
		{
		return TEGetExternalClipboard(text, style);
		}
	else
		{
		JRunArray<Font> tempStyle;
		return TEGetExternalClipboard(text, &tempStyle);
		}
}

/******************************************************************************
 Paste

 ******************************************************************************/

void
JTextEditor16::Paste()
{
	JString16 text;
	JRunArray<Font> style;
	if (TEOwnsClipboard() && itsClipText != NULL)
		{
		Paste(*itsClipText, itsClipStyle);
		}
	else if (TEGetExternalClipboard(&text, &style))
		{
		JRunArray<Font>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);
		}
}

/******************************************************************************
 Paste

	style can be NULL

 ******************************************************************************/

void
JTextEditor16::Paste
	(
	const JCharacter16*		text,
	const JRunArray<Font>*	style
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	JSize pasteLength =
		(style != NULL ? style->GetElementCount() : strlen16(text));

	JTEUndoPaste16* newUndo = new JTEUndoPaste16(this, pasteLength);
	assert( newUndo != NULL );

	pasteLength = PrivatePaste(text, style);
	newUndo->SetPasteLength(pasteLength);

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 PrivatePaste (private)

	Returns number of characters that were actually inserted.

	style can be NULL.

 ******************************************************************************/

JSize
JTextEditor16::PrivatePaste
	(
	const JCharacter16*		text,
	const JRunArray<Font>*	style
	)
{
	const JBoolean hadSelection = HasSelection();
	if (hadSelection)
		{
		itsInsertionFont = itsStyles->GetElement(itsSelection.first);
		DeleteText(itsSelection);
		itsCaretLoc = CalcCaretLocation(itsSelection.first);
		itsSelection.SetToNothing();
		}

	JSize pasteLength;
	if (itsPasteStyledTextFlag)
		{
		pasteLength = InsertText(itsCaretLoc.charIndex, text, style);
		}
	else
		{
		pasteLength = InsertText(itsCaretLoc.charIndex, text);
		}

	const JSize textLen = strlen16(text);
	Recalc(itsCaretLoc, textLen, hadSelection, kJFalse);
	SetCaretLocation(itsCaretLoc.charIndex + textLen);
	return pasteLength;
}

/******************************************************************************
 GetInternalClipboard (protected)

	Returns kJTrue if our internal clipboard contains something.

	style can be NULL.

 ******************************************************************************/

JBoolean
JTextEditor16::GetInternalClipboard
	(
	const JString16**			text,
	const JRunArray<Font>**	style
	)
	const
{
	if (itsClipText != NULL && itsClipStyle != NULL)
		{
		*text = itsClipText;
		if (style != NULL)
			{
			*style = itsClipStyle;
			}
		return kJTrue;
		}
	else
		{
		*text = NULL;
		if (style != NULL)
			{
			*style = NULL;
			}
		return kJFalse;
		}
}

/******************************************************************************
 TECreateClipboard (private)

	Allocate itsClipText and itsClipStyle.

 ******************************************************************************/

void
JTextEditor16::TECreateClipboard()
{
	assert( (itsClipText == NULL && itsClipStyle == NULL) ||
			(itsClipText != NULL && itsClipStyle != NULL) );

	if (itsClipText == NULL)
		{
		itsClipText = new JString16;
		assert( itsClipText != NULL );

		itsClipStyle = new JRunArray<Font>;
		assert( itsClipStyle != NULL );
		}
}

/******************************************************************************
 TEClearClipboard (protected)

	Delete itsClipText and itsClipStyle.

 ******************************************************************************/

void
JTextEditor16::TEClearClipboard()
{
	delete itsClipText;
	itsClipText = NULL;

	delete itsClipStyle;
	itsClipStyle = NULL;
}

/******************************************************************************
 GetInsertionIndex

	Return the index where new text will be typed or pasted.

 ******************************************************************************/

JIndex
JTextEditor16::GetInsertionIndex()
	const
{
	if (!itsSelection.IsEmpty())
		{
		return itsSelection.first;
		}
	else
		{
		return itsCaretLoc.charIndex;
		}
}

/******************************************************************************
 GetSelection

 ******************************************************************************/

JBoolean
JTextEditor16::GetSelection
	(
	JString16* text
	)
	const
{
	if (!itsSelection.IsEmpty())
		{
		*text = itsBuffer->GetSubstring(itsSelection);
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

JBoolean
JTextEditor16::GetSelection
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	style->RemoveAll();

	if (!itsSelection.IsEmpty())
		{
		*text = itsBuffer->GetSubstring(itsSelection);
		style->InsertElementsAtIndex(1, *itsStyles, itsSelection.first,
									 itsSelection.GetLength());
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

JBoolean
JTextEditor16::GetTextRange
	(
	JString16*			text,
	JRunArray<Font>*	style,
	const JIndex		range_start,
	const JIndex		range_end
	)
	const
{
	style->RemoveAll();

	if ((range_start <= range_end) && (range_end != 0))
		{
		*text = itsBuffer->GetSubstring(range_start, range_end);
		style->InsertElementsAtIndex(1, *itsStyles, range_start, range_end - range_start + 1);
		return kJTrue;
		}
	else
		{
		text->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 SetSelection

 ******************************************************************************/

void
JTextEditor16::SetSelection
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	const JBoolean	needCaretBcast
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	DeactivateCurrentUndo();
	itsPrevDragType = kInvalidDrag;		// avoid wordSel and lineSel pivots

	if (itsBuffer->IsEmpty() || itsSelection.Is(startIndex, endIndex))
		{
		return;
		}

	assert( IndexValid(startIndex) );
	assert( IndexValid(endIndex) );
	assert( startIndex <= endIndex );

	const JBoolean hadSelection      = JNegate( itsSelection.IsEmpty() );
	const CaretLocation origCaretLoc = itsCaretLoc;
	const JIndexRange origSelection  = itsSelection;

	itsCaretLoc = CaretLocation(0,0);
	itsSelection.Set(startIndex, endIndex);

	const JIndex newStartLine = GetLineForChar(itsSelection.first);
	const JIndex newEndLine   = GetLineForChar(itsSelection.last);

	if (needCaretBcast)
		{
		BroadcastCaretMessages(CaretLocation(itsSelection.first, newStartLine), kJTrue);
		}

	TECaretShouldBlink(kJFalse);

	if (itsCopyWhenSelectFlag && itsDoCopySelectFlag && itsType != kStaticText)
		{
		Copy();
		}

	// We only optimize heavily for the case when one end of the
	// selection remains fixed because this is the case during mouse drags.

	if (hadSelection && origSelection.first == itsSelection.first)
		{
		const JIndex origEndLine = GetLineForChar(origSelection.last);
		TERefreshLines(JMin(origEndLine, newEndLine),
					   JMax(origEndLine, newEndLine));
		}
	else if (hadSelection && origSelection.last == itsSelection.last)
		{
		const JIndex origStartLine = GetLineForChar(origSelection.first);
		TERefreshLines(JMin(origStartLine, newStartLine),
					   JMax(origStartLine, newStartLine));
		}
	else if (hadSelection)
		{
		const JIndex origStartLine = GetLineForChar(origSelection.first);
		const JIndex origEndLine   = GetLineForChar(origSelection.last);
		TERefreshLines(origStartLine, origEndLine);
		TERefreshLines(newStartLine, newEndLine);
		}
	else
		{
		TERefreshCaret(origCaretLoc);
		TERefreshLines(newStartLine, newEndLine);
		}
}

/******************************************************************************
 EditSelectAll - called in response to user Edit menu command

 ******************************************************************************/

void
JTextEditor16::EditSelectAll()
{
	StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
	SelectAll();
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JTextEditor16::SelectAll()
{
	if (!itsBuffer->IsEmpty())
		{
		SetSelection(1, itsBuffer->GetLength());
		}
}

/******************************************************************************
 DeleteSelection

	We create JTEUndoTyping16 so keys pressed after the delete key count
	as part of the undo task.

 ******************************************************************************/

void
JTextEditor16::DeleteSelection()
{
	if (!itsIsDragSourceFlag && !itsSelection.IsEmpty())
		{
		JTEUndoTyping16* newUndo = new JTEUndoTyping16(this);
		assert( newUndo != NULL );

		DeleteText(itsSelection);
		Recalc(itsSelection.first, 1, kJTrue, kJFalse);
		SetCaretLocation(itsSelection.first);

		NewUndo(newUndo, kJTrue);
		}
}

/******************************************************************************
 DeleteToStartOfWord

	Delete characters until start-of-word is reached.

 ******************************************************************************/

void
JTextEditor16::DeleteToStartOfWord()
{
	if (itsCaretLoc.charIndex > 1)
		{
		if (itsSelection.IsEmpty())
			{
			const JIndex startIndex = GetWordStart(itsCaretLoc.charIndex-1);
			SetSelection(startIndex, itsCaretLoc.charIndex-1);
			}

		DeleteSelection();
		}
}

/******************************************************************************
 DeleteToEndOfWord

	Delete characters until end-of-word is reached.

 ******************************************************************************/

void
JTextEditor16::DeleteToEndOfWord()
{
	if (itsCaretLoc.charIndex <= itsBuffer->GetLength())
		{
		if (itsSelection.IsEmpty())
			{
			const JIndex endIndex = GetWordEnd(itsCaretLoc.charIndex);
			SetSelection(itsCaretLoc.charIndex, endIndex);
			}

		DeleteSelection();
		}
}

/******************************************************************************
 TabSelectionLeft

	Remove tabs from the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor16::TabSelectionLeft
	(
	const JSize		tabCount,
	const JBoolean	force
	)
{
	if (TEIsDragging())
		{
		return;
		}

JIndex i,j;

	if (!itsSelection.IsEmpty())
		{
		SetSelection(GetParagraphStart(itsSelection.first),			// for undo
					 GetParagraphEnd(itsSelection.last));
		}
	else if (!itsBuffer->IsEmpty())
		{
		SetSelection(GetParagraphStart(itsCaretLoc.charIndex),		// for undo
					 GetParagraphEnd(itsCaretLoc.charIndex));
		}
	else
		{
		return;
		}

	const JIndex firstLine = GetLineForChar(itsSelection.first);
	const JIndex lastLine  = GetLineForChar(itsSelection.last);

	// check that there are enough tabs at the start of every selected line,
	// ignoring lines created by wrapping

	JBoolean sufficientWS      = kJTrue;
	JSize prefixSpaceCount     = 0;		// min # of spaces at start of line
	JBoolean firstNonemptyLine = kJTrue;

	for (i=firstLine; i<=lastLine; i++)
		{
		JIndex firstChar = GetLineStart(i);
		if ((firstChar == 1 || itsBuffer->GetCharacter(firstChar-1) == '\n') &&
			itsBuffer->GetCharacter(firstChar) != '\n')
			{
			for (j=1; j<=tabCount; j++)
				{
				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (firstChar <= itsBuffer->GetLength() &&
					   itsBuffer->GetCharacter(firstChar) == ' ' &&
					   spaceCount < itsCRMTabCharCount)
					{
					spaceCount++;
					firstChar++;
					}
				if (firstChar > itsBuffer->GetLength())
					{
					break;	// last line contains only whitespace
					}
				if (j == 1 && (spaceCount < prefixSpaceCount || firstNonemptyLine))
					{
					prefixSpaceCount  = spaceCount;
					firstNonemptyLine = kJFalse;
					}
				if (spaceCount >= itsCRMTabCharCount)
					{
					continue;
					}

				const JCharacter16 c = itsBuffer->GetCharacter(firstChar);
				if (c != '\t' && c != '\n')
					{
					sufficientWS = kJFalse;
					break;
					}
				if (c == '\n')
					{
					break;	// line is blank or contains only whitespace
					}
				}
			}
		}

	// If same number of spaces at front of every line, remove those even if
	// not enough to shift by one tab.

	JSize tabCharCount = itsCRMTabCharCount;
	if (!sufficientWS && prefixSpaceCount > 0 && tabCount == 1)
		{
		tabCharCount = prefixSpaceCount;
		}
	else if (force)
		{
		sufficientWS = kJTrue;
		}
	else if (!sufficientWS)
		{
		return;
		}

	JBoolean isNew;
	JTEUndoTabShift16* undo = GetTabShiftUndo(&isNew);

	itsSelection.SetToNothing();
	JSize deleteCount = 0;
	for (i=firstLine; i<=lastLine; i++)
		{
		const JIndex charIndex = GetLineStart(i) - deleteCount;
		if ((charIndex == 1 || itsBuffer->GetCharacter(charIndex-1) == '\n') &&
			itsBuffer->GetCharacter(charIndex) != '\n')
			{
			for (j=1; j<=tabCount; j++)
				{
				// The deletion point stays in same place (charIndex) and
				// simply eats characters.

				// accept itsCRMTabCharCount spaces instead of tab
				// accept fewer spaces in front of tab

				JSize spaceCount = 0;
				while (charIndex <= itsBuffer->GetLength() &&
					   itsBuffer->GetCharacter(charIndex) == ' ' &&
					   spaceCount < tabCharCount)
					{
					spaceCount++;
					DeleteText(charIndex, charIndex);
					deleteCount++;
					}
				if (charIndex > itsBuffer->GetLength())
					{
					break;	// last line contains only whitespace
					}
				if (!sufficientWS)
					{
					break;		// only remove tabCharCount spaces at front of every line
					}
				if (spaceCount >= tabCharCount)
					{
					continue;	// removed equivalent of one tab
					}

				if (itsBuffer->GetCharacter(charIndex) != '\t')
					{
					break;
					}
				DeleteText(charIndex, charIndex);
				deleteCount++;
				}
			}
		}

	const JIndex startIndex = GetLineStart(firstLine);
	const JIndex endIndex   = GetLineEnd(lastLine);
	Recalc(startIndex, endIndex - startIndex + 1, kJTrue, kJFalse);

	SetSelection(startIndex, endIndex - deleteCount);
	undo->UpdateEndChar();

	NewUndo(undo, isNew);
}

/******************************************************************************
 TabSelectionRight

	Insert a tab at the beginning of every selected line.  If nothing
	is selected, we select the line that the caret is on.

 ******************************************************************************/

void
JTextEditor16::TabSelectionRight
	(
	const JSize tabCount
	)
{
	if (TEIsDragging())
		{
		return;
		}

JIndex i;

	if (!itsSelection.IsEmpty())
		{
		SetSelection(GetParagraphStart(itsSelection.first),
					 GetParagraphEnd(itsSelection.last));
		}
	else if (!itsBuffer->IsEmpty())
		{
		SetSelection(GetParagraphStart(itsCaretLoc.charIndex),
					 GetParagraphEnd(itsCaretLoc.charIndex));
		}
	else
		{
		return;
		}

	const JIndex firstLine = GetLineForChar(itsSelection.first);
	const JIndex lastLine  = GetLineForChar(itsSelection.last);

	JBoolean isNew;
	JTEUndoTabShift16* undo = GetTabShiftUndo(&isNew);

	JString16 tabs;
	if (itsTabToSpacesFlag)
		{
		const JSize spaceCount = tabCount*itsCRMTabCharCount;
		for (i=1; i<=spaceCount; i++)
			{
			tabs.AppendCharacter(' ');
			}
		}
	else
		{
		for (i=1; i<=tabCount; i++)
			{
			tabs.AppendCharacter('\t');
			}
		}

	itsSelection.SetToNothing();
	JSize insertCount = 0;
	for (i=firstLine; i<=lastLine; i++)
		{
		const JIndex charIndex = GetLineStart(i) + insertCount;
		if ((charIndex == 1 || itsBuffer->GetCharacter(charIndex-1) == '\n') &&
			itsBuffer->GetCharacter(charIndex) != '\n')
			{
			insertCount += InsertText(charIndex, tabs);
			}
		}

	const JIndex startIndex = GetLineStart(firstLine);
	const JIndex endIndex   = GetLineEnd(lastLine) + insertCount;
	Recalc(startIndex, endIndex - startIndex + 1, kJFalse, kJFalse);

	SetSelection(startIndex, endIndex);
	undo->UpdateEndChar();

	NewUndo(undo, isNew);
}

/******************************************************************************
 CleanRightMargin

	If there is a selection, cleans up the right margin of each paragraph
	touched by the selection.  Otherwise, cleans up the right margin of the
	paragraph containing the caret and maintains the caret position.

	If coerce, paragraphs are detected by looking only for blank lines.
	Otherwise, they are detected by blank lines or a change in the line prefix.

	If anything is changed, returns kJTrue and sets *reformatRange to the
	range of reformatted text, excluding the last newline.

	The work done by this function can be changed by calling SetCRMRuleList()
	and overriding the virtual CRM*() functions.

 ******************************************************************************/

JBoolean
JTextEditor16::CleanRightMargin
	(
	const JBoolean	coerce,
	JIndexRange*	reformatRange
	)
{
	if (TEIsDragging())
		{
		return kJFalse;
		}

	JBoolean changed = kJFalse;
	JIndexRange origTextRange;
	JString16 newText;
	JRunArray<Font> newStyles;
	JIndex newCaretIndex;
	if (itsSelection.IsEmpty())
		{
		changed = PrivateCleanRightMargin(coerce, &origTextRange, &newText,
										  &newStyles, &newCaretIndex);
		}
	else
		{
		const JIndex endChar = GetParagraphEnd(itsSelection.last) - 1;

		JIndexRange range;
		JString16 text;
		JRunArray<Font> styles;
		JIndex caretIndex;
		JBoolean first = kJTrue;
		while (1)
			{
			if (PrivateCleanRightMargin(coerce, &range, &text, &styles, &caretIndex))
				{
				origTextRange += range;
				newText       += text;
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												styles, 1, styles.GetElementCount());
				if (range.last < endChar)
					{
					(range.last)++;
					(origTextRange.last)++;
					newText.AppendCharacter('\n');
					newStyles.AppendElement(newStyles.GetLastElement());
					}
				}
			else
				{
				caretIndex = GetInsertionIndex();
				range.Set(GetParagraphStart(caretIndex),
						  GetParagraphEnd(caretIndex));

				assert( newText.IsEmpty() ||
						range.first == origTextRange.last + 1 );

				origTextRange += range;
				newText       += itsBuffer->GetSubstring(range);
				newStyles.InsertElementsAtIndex(newStyles.GetElementCount()+1,
												*itsStyles, range.first, range.GetLength());
				}

			if (first)
				{
				newCaretIndex = caretIndex;
				first         = kJFalse;
				}

			if (range.last < endChar && IndexValid(range.last + 1))
				{
				SetCaretLocation(range.last + 1);
				}
			else
				{
				break;
				}
			}

		changed = kJTrue;
		}

	if (changed)
		{
		SetSelection(origTextRange, kJFalse);
		Paste(newText, &newStyles);
		SetCaretLocation(newCaretIndex);

		reformatRange->SetFirstAndLength(origTextRange.first, newText.GetLength());
		return kJTrue;
		}
	else
		{
		reformatRange->SetToNothing();
		return kJFalse;
		}
}

/******************************************************************************
 PrivateCleanRightMargin (private)

	*newText is the cleaned up version of the paragraph containing the caret
	or start of selection.

	*origTextRange contains the range that was cleaned up, excluding the last
	newline.

	*newCaretIndex is the index required to maintain the position of the caret.

	Returns kJFalse if the caret was not in a paragraph.

 ******************************************************************************/

JBoolean
JTextEditor16::PrivateCleanRightMargin
	(
	const JBoolean		coerce,
	JIndexRange*		origTextRange,
	JString16*			newText,
	JRunArray<Font>*	newStyles,
	JIndex*				newCaretIndex
	)
	const
{
	origTextRange->SetToNothing();
	newText->Clear();
	newStyles->RemoveAll();
	*newCaretIndex = 0;

	if (itsBuffer->IsEmpty())
		{
		return kJFalse;
		}

	const JIndex caretChar = GetInsertionIndex();
	if (caretChar == itsBuffer->GetLength()+1 &&		// ends with newline
		EndsWithNewline())
		{
		return kJFalse;
		}

	JIndex charIndex, ruleIndex;
	JString16 firstLinePrefix, restLinePrefix;
	JSize firstPrefixLength, restPrefixLength;
	if (!CRMGetRange(caretChar, coerce, origTextRange, &charIndex,
					 &firstLinePrefix, &firstPrefixLength,
					 &restLinePrefix, &restPrefixLength, &ruleIndex))
		{
		return kJFalse;
		}

	if (caretChar <= charIndex)
		{
		*newCaretIndex = caretChar;
		}

	// read in each word, convert it, write it out

	JSize currentLineWidth = 0;
	JBoolean requireSpace  = kJFalse;

	JString16 wordBuffer, spaceBuffer;
	JRunArray<Font> wordStyles;
	while (charIndex <= origTextRange->last)
		{
		JSize spaceCount;
		JIndex rnwCaretIndex = 0;
		const CRMStatus status =
			CRMReadNextWord(&charIndex, origTextRange->last,
							&spaceBuffer, &spaceCount, &wordBuffer, &wordStyles,
							currentLineWidth, caretChar, &rnwCaretIndex,
							*newText, requireSpace);
		requireSpace = kJTrue;

		if (status == kFinished)
			{
			assert( charIndex == origTextRange->last+1 );
			break;
			}
		else if (status == kFoundWord)
			{
			if (newText->IsEmpty())
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  firstLinePrefix, firstPrefixLength);
				}
			else
				{
				CRMAppendWord(newText, newStyles, &currentLineWidth, &rnwCaretIndex,
							  spaceBuffer, spaceCount, wordBuffer, wordStyles,
							  restLinePrefix, restPrefixLength);
				}

			if (rnwCaretIndex > 0)
				{
				*newCaretIndex = origTextRange->first + rnwCaretIndex - 1;
				}
			}
		else
			{
			assert( status == kFoundNewLine );

			CRMTossLinePrefix(&charIndex, origTextRange->last, ruleIndex);

			// CRMGetRange() insures this is strictly *inside* the text,
			// so caretChar == charIndex will be caught elsewhere.

			if (*newCaretIndex == 0 && caretChar < charIndex)
				{
				*newCaretIndex = origTextRange->first + newText->GetLength();
				}
			}
		}

	if (caretChar == origTextRange->last+1)
		{
		*newCaretIndex = origTextRange->first + newText->GetLength();
		}

	assert( *newCaretIndex != 0 );
	assert( newText->GetLength() == newStyles->GetElementCount() );

	return kJTrue;
}

/*******************************************************************************
 CRMGetRange (private)

	Returns the range of characters to reformat.
	Returns kJFalse if the caret is not in a paragraph.

	caretChar is the current location of the caret.

	If coerce, we search forward and backward from the caret location
	for blank lines and include all the text between these blank lines.
	Blank lines are defined as lines that contain nothing but prefix characters.

	Otherwise, we first search backward until we find a blank line or a
	line prefix that can't generate the following one.  Then we search
	forward until we find a blank line or the prefix changes.

	The latter method is safer because it doesn't change the prefix of any of
	the lines.  The former is useful for forcing a change in the prefix.

 ******************************************************************************/

JBoolean
JTextEditor16::CRMGetRange
	(
	const JIndex	caretChar,
	const JBoolean	coerce,
	JIndexRange*	range,
	JIndex*			textStartIndex,
	JString16*		firstLinePrefix,
	JSize*			firstPrefixLength,
	JString16*		restLinePrefix,
	JSize*			restPrefixLength,
	JIndex*			returnRuleIndex
	)
	const
{
	range->Set(GetParagraphStart(caretChar), GetParagraphEnd(caretChar));
	while (range->last > 0 && itsBuffer->GetCharacter(range->last) == '\n')
		{
		range->last--;
		}

	// If the line we are on is empty, quit immediately.

	JIndex tempStart = range->first;
	JString16 origLinePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (range->IsEmpty() ||
		!CRMGetPrefix(&tempStart, range->last,
					  &origLinePrefix, &prefixLength, &ruleIndex) ||
		CRMLineMatchesRest(*range))
		{
		return kJFalse;
		}

	// search backward for a blank line or a change in the prefix (if !coerce)
	// (If range->first==2, the line above us is blank.)

	JString16 currLinePrefix, nextLinePrefix = origLinePrefix;
	while (range->first > 2)
		{
		const JIndex newStart = GetParagraphStart(range->first - 1);
		tempStart             = newStart;
		ruleIndex             = 0;
		if (tempStart >= range->first - 1 ||
			!CRMGetPrefix(&tempStart, range->first - 2,
						  &currLinePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(newStart, range->first - 2)) ||
			(!coerce &&
			 CRMBuildRestPrefix(currLinePrefix, ruleIndex, &prefixLength) != nextLinePrefix))
			{
			break;
			}
		range->first   = newStart;
		nextLinePrefix = currLinePrefix;
		ruleIndex      = 0;
		}

	// search forward for a blank line or a change in the prefix (if !coerce)
	// (If range->last==bufLength-1, the text ends with a newline.)

	*textStartIndex  = range->first;
	*returnRuleIndex = 0;
	const JBoolean hasText =
		CRMGetPrefix(textStartIndex, range->last,
					 firstLinePrefix, firstPrefixLength, returnRuleIndex);
	assert( hasText );

	*restLinePrefix = CRMBuildRestPrefix(*firstLinePrefix, *returnRuleIndex,
										 restPrefixLength);

	while (range->last < itsBuffer->GetLength()-1)
		{
		tempStart     = range->last + 2;
		JIndex newEnd = GetParagraphEnd(tempStart);
		if (itsBuffer->GetCharacter(newEnd) == '\n')	// could hit end of text instead
			{
			newEnd--;
			}

		JIndex tempRuleIndex = *returnRuleIndex;
		if (newEnd < tempStart ||
			!CRMGetPrefix(&tempStart, newEnd,
						  &currLinePrefix, &prefixLength, &tempRuleIndex) ||
			(!coerce && currLinePrefix != *restLinePrefix))
			{
			break;
			}
		range->last = newEnd;
		}

	return kJTrue;
}

/*******************************************************************************
 CRMGetPrefix (private)

	Returns the prefix to be used for each line and updates *startChar to point
	to the first character after the prefix.

	*prefixLength is set to the length of *linePrefix in characters.  This
	can be greater than linePrefix->GetLength() because of tabs.

	Returns kJFalse if the entire range qualifies as a prefix.

 ******************************************************************************/

JBoolean
JTextEditor16::CRMGetPrefix
	(
	JIndex*			startChar,
	const JIndex	endChar,
	JString16*		linePrefix,
	JSize*			prefixLength,
	JIndex*			ruleIndex
	)
	const
{
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*startChar, endChar), ruleIndex);

	*startChar    = prefixRange.last + 1;
	*linePrefix   = itsBuffer->GetSubstring(prefixRange);
	*prefixLength = CRMCalcPrefixLength(linePrefix);
	return JConvertToBoolean(*startChar <= endChar);
}

/*******************************************************************************
 CRMMatchPrefix (private)

	Returns the range of characters that qualifies as a prefix.

	To match a prefix, all CRMRule::first's and [ \t]* are tried.
	The longest match is used.

	If *ruleIndex > 0, CRMRule::rest for the specified rule is tried *first*,
	and if this matches, it preempts everything else.

	When the function returns, *ruleIndex is the CRMRule::first that matched.
	(It remains 0 if the default rule is used, since this doesn't have "rest".)

 ******************************************************************************/

static const JRegex defaultCRMPrefixRegex = "[ \t]*";

JIndexRange
JTextEditor16::CRMMatchPrefix
	(
	const JIndexRange&	textRange,
	JIndex*				ruleIndex
	)
	const
{
	JIndexRange matchRange;

#ifdef _TODO
	if (itsCRMRuleList != NULL && *ruleIndex > 0)
		{
		const CRMRule rule = itsCRMRuleList->GetElement(*ruleIndex);
		if ((rule.rest)->MatchWithin(*itsBuffer, textRange, &matchRange) &&
			matchRange.first == textRange.first)
			{
			return matchRange;
			}
		}

	JIndexRange range;
	if (itsCRMRuleList != NULL)
		{
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.first)->MatchWithin(*itsBuffer, textRange, &range) &&
				range.first == textRange.first &&
				range.last  >  matchRange.last)
				{
				matchRange = range;
				*ruleIndex = i;
				}
			}
		}

	// check equality of range::last in case prefix is empty

	const JBoolean defMatch =
		defaultCRMPrefixRegex.MatchWithin(*itsBuffer, textRange, &range);
	assert( defMatch && range.first == textRange.first );
	if (range.last >= matchRange.last || itsCRMRuleList == NULL)
		{
		matchRange = range;
		*ruleIndex = 0;
		}
#endif

	return matchRange;
}

/*******************************************************************************
 CRMLineMatchesRest (private)

	Returns kJTrue if the given range is matched by any "rest" pattern.
	Used at beginning of CRMGetRange as part of check if line is empty.

 ******************************************************************************/

JBoolean
JTextEditor16::CRMLineMatchesRest
	(
	const JIndexRange& range
	)
	const
{
#ifdef _TODO
	if (itsCRMRuleList != NULL)
		{
		JIndexRange matchRange;
		const JSize count = itsCRMRuleList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const CRMRule rule = itsCRMRuleList->GetElement(i);
			if ((rule.rest)->MatchWithin(*itsBuffer, range, &matchRange) &&
				range == matchRange)
				{
				return kJTrue;
				}
			}
		}
#endif

	return kJFalse;
}

/*******************************************************************************
 CRMCalcPrefixLength (private)

	Returns the length of *linePrefix in characters.  This can be greater
	than linePrefix->GetLength() because of tabs.  *linePrefix may be modified.

 ******************************************************************************/

JSize
JTextEditor16::CRMCalcPrefixLength
	(
	JString16* linePrefix
	)
	const
{
	if (linePrefix->IsEmpty())
		{
		return 0;
		}

	const JString16 s    = *linePrefix;
	JSize prefixLength = 0;
	linePrefix->Clear();

	const JSize length = s.GetLength();
	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter16 c = s.GetCharacter(i);
		if (c == '\t')
			{
			CRMConvertTab(linePrefix, &prefixLength, 0);
			}
		else
			{
			linePrefix->AppendCharacter(c);
			prefixLength++;
			}
		}

	return prefixLength;
}

/*******************************************************************************
 CRMBuildRestPrefix (private)

	Returns the line prefix to be used for all lines after the first one.

 ******************************************************************************/

JString16
JTextEditor16::CRMBuildRestPrefix
	(
	const JString16&	firstLinePrefix,
	const JIndex	ruleIndex,
	JSize*			prefixLength
	)
	const
{
	JString16 s = firstLinePrefix;
#ifdef _TODO
	if (itsCRMRuleList != NULL && ruleIndex > 0)
		{
		JArray<JIndexRange> matchList;
		const CRMRule rule   = itsCRMRuleList->GetElement(ruleIndex);
		const JBoolean match = (rule.first)->Match(s, &matchList);
		assert( match &&
				(matchList.GetFirstElement()).first == 1 &&
				(matchList.GetFirstElement()).last  == s.GetLength() );

		JIndexRange newRange;
		(rule.first)->Replace(&s, matchList, &newRange);
		}

	*prefixLength = CRMCalcPrefixLength(&s);
#endif
	return s;
}

/*******************************************************************************
 CRMTossLinePrefix (private)

	Increments *charIndex past whatever qualifies as a line prefix.

	The default is to toss all character approved by CRMIsPrefixChar().

 ******************************************************************************/

void
JTextEditor16::CRMTossLinePrefix
	(
	JIndex*			charIndex,
	const JIndex	endChar,
	const JIndex	origRuleIndex
	)
	const
{
	JIndex ruleIndex = origRuleIndex;
	const JIndexRange prefixRange =
		CRMMatchPrefix(JIndexRange(*charIndex, endChar), &ruleIndex);
	*charIndex = prefixRange.last + 1;
}

/*******************************************************************************
 CRMConvertTab (virtual protected)

	Appends the tab to *charBuffer and increments *charCount appropriately.
	The default is to physically append the tab character and use
	CRMGetTabWidth() to increment *charCount.  Note that *charCount may be
	longer than charBuffer->GetLength().

	currentLineWidth is the number of characters on the line -excluding-
	what is in *charBuffer.

	Derived classes can override this to, for example, append some spaces
	instead of a tab character.  (How's that for splitting an infinitive?)

 ******************************************************************************/

void
JTextEditor16::CRMConvertTab
	(
	JString16*	charBuffer,
	JSize*		charCount,
	const JSize	currentLineWidth
	)
	const
{
	charBuffer->AppendCharacter('\t');
	*charCount += CRMGetTabWidth(currentLineWidth + *charCount);
}

/*******************************************************************************
 CRMGetTabWidth (virtual protected)

	Returns the number of spaces to which the tab is equivalent.
	The default is to round up to the nearest multiple of GetCRMTabCharCount().
	The default value for this is 8 since this is what all UNIX programs use.

	textColumn starts at zero at the left margin.

 ******************************************************************************/

JSize
JTextEditor16::CRMGetTabWidth
	(
	const JIndex textColumn
	)
	const
{
	return itsCRMTabCharCount - (textColumn % itsCRMTabCharCount);
}

/*******************************************************************************
 CRMIsEOS (private)

 ******************************************************************************/

inline int
JTextEditor16::CRMIsEOS
	(
	const JCharacter16 c
	)
	const
{
	return (c == '.' || c == '?' || c == '!');
}

/*******************************************************************************
 CRMReadNextWord (private)

	Read one block of { spaces + word (non-spaces) } from itsBuffer, starting
	at *charIndex, stopping if we get as far as endIndex.  *spaceBuffer
	contains the whitespace (spaces + tabs) that was found.  *spaceCount is
	the equivalent number of spaces.  *wordBuffer contains the word.
	*charIndex is incremented to point to the next character to read.

	Newlines are treated separately.  If a newline is encountered while reading
	spaces, we throw out the spaces and return kFoundNewLine.  If a newline
	is encountered while reading a word, we leave it for the next time, when
	we immediately return kFoundNewLine.

	When we pass the position origCaretIndex, we set *newCaretIndex to be
	the index into *spaceBuffer+*wordBuffer.  Otherwise, we do not change
	*newCaretIndex.

 ******************************************************************************/

JTextEditor16::CRMStatus
JTextEditor16::CRMReadNextWord
	(
	JIndex*				charIndex,
	const JIndex		endIndex,
	JString16*			spaceBuffer,
	JSize*				spaceCount,
	JString16*			wordBuffer,
	JRunArray<Font>*	wordStyles,
	const JSize			currentLineWidth,
	const JIndex		origCaretIndex,
	JIndex*				newCaretIndex,
	const JString16&		newText,
	const JBoolean		requireSpace
	)
	const
{
#ifdef _TODO
	// read the whitespace

	spaceBuffer->Clear();
	*spaceCount = 0;

	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + 1;
			}

		const JCharacter16 c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ')
			{
			(*charIndex)++;
			spaceBuffer->AppendCharacter(c);
			(*spaceCount)++;
			}
		else if (c == '\t')
			{
			(*charIndex)++;
			CRMConvertTab(spaceBuffer, spaceCount, currentLineWidth);
			}
		else if (c == '\n')			// we can ignore the spaces
			{
			(*charIndex)++;
			spaceBuffer->Clear();
			*spaceCount = 0;
			return kFoundNewLine;
			}
		else						// found beginning of word
			{
			break;
			}
		}

	if (*charIndex == endIndex+1)	// we can ignore the spaces
		{
		return kFinished;
		}

	// read the word

	wordBuffer->Clear();

	const JIndex wordStart = *charIndex;
	while (*charIndex <= endIndex)
		{
		if (*charIndex == origCaretIndex)
			{
			*newCaretIndex = spaceBuffer->GetLength() + wordBuffer->GetLength() + 1;
			}

		const JCharacter16 c = itsBuffer->GetCharacter(*charIndex);
		if (c == ' ' || c == '\t' || c == '\n')
			{
			break;
			}

		wordBuffer->AppendCharacter(c);
		(*charIndex)++;
		}

	wordStyles->RemoveAll();
	wordStyles->InsertElementsAtIndex(1, *itsStyles, wordStart, wordBuffer->GetLength());

	// After a newline, the whitespace may have been tossed
	// as belonging to the prefix, but we still need some space.
	//
	// Punctuation is assumed to be the end of a sentence if the
	// following word does not start with a lower case letter.

	if (*spaceCount == 0 && requireSpace)
		{
		JCharacter16 c1 = newText.GetLastCharacter();
		JCharacter16 c2 = '\0';
		if (newText.GetLength() > 1)
			{
			c2 = newText.GetCharacter(newText.GetLength()-1);
			}
		if ((CRMIsEOS(c1) || (CRMIsEOS(c2) && c1 =='\"')) &&
			!JIsLower(wordBuffer->GetFirstCharacter()))
			{
			*spaceBuffer = "  ";
			}
		else
			{
			*spaceBuffer = " ";
			}

		*spaceCount = spaceBuffer->GetLength();
		if (*newCaretIndex > 0)
			{
			*newCaretIndex += *spaceCount;
			}
		}
#endif

	return kFoundWord;
}

/*******************************************************************************
 CRMAppendWord (private)

	Add the spaces and word to new text, maintaining the required line width.

	If *newCaretIndex>0, we convert it from an index in spaceBuffer+wordBuffer
	to an index in newText.

 ******************************************************************************/

void
JTextEditor16::CRMAppendWord
	(
	JString16*				newText,
	JRunArray<Font>*		newStyles,
	JSize*					currentLineWidth,
	JIndex*					newCaretIndex,
	const JString16&			spaceBuffer,
	const JSize				spaceCount,
	const JString16&			wordBuffer,
	const JRunArray<Font>&	wordStyles,
	const JString16&			linePrefix,
	const JSize				prefixLength
	)
	const
{
	const JSize newLineWidth = *currentLineWidth + spaceCount + wordBuffer.GetLength();
	if (*currentLineWidth == 0 || newLineWidth > itsCRMLineWidth)
		{
		// calculate prefix font

		Font prefixFont  = wordStyles.GetFirstElement();
		prefixFont.style = JFontStyle();
		prefixFont.id    =
			itsFontMgr->UpdateFontID(prefixFont.id, prefixFont.size, prefixFont.style);

		// terminate previous line

		if (!newText->IsEmpty())
			{
			newText->AppendCharacter('\n');
			newStyles->AppendElement(prefixFont);
			}
		if (!linePrefix.IsEmpty())
			{
			*newText += linePrefix;
			newStyles->AppendElements(prefixFont, linePrefix.GetLength());
			}

		// write word

		if (*newCaretIndex > 0)
			{
			if (*newCaretIndex <= spaceCount)
				{
				*newCaretIndex = 1;				// in spaces that we toss
				}
			else
				{
				*newCaretIndex -= spaceCount;	// in word
				}

			*newCaretIndex += newText->GetLength();
			}

		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());
		*currentLineWidth = prefixLength + wordBuffer.GetLength();
		}
	else	// newLineWidth <= itsCRMLineWidth
		{
		// append spaces + word at end of line

		if (*newCaretIndex > 0)
			{
			*newCaretIndex += newText->GetLength();
			}

		*newText += spaceBuffer;
		newStyles->AppendElements(CalcWSFont(newStyles->GetLastElement(),
											 wordStyles.GetFirstElement()),
								  spaceBuffer.GetLength());
		*newText += wordBuffer;
		newStyles->InsertElementsAtIndex(newStyles->GetElementCount()+1,
										 wordStyles, 1, wordStyles.GetElementCount());

		if (newLineWidth < itsCRMLineWidth)
			{
			*currentLineWidth = newLineWidth;
			}
		else	// newLineWidth == itsCRMLineWidth
			{
			*currentLineWidth = 0;
			}
		}
}

/******************************************************************************
 SetCRMRuleList

	To find the first line of a paragraph, one searches backward for a blank
	line:  one that matches any CRMRule::first.  The following line is the
	beginning of the paragraph.

	Each following line is part of the paragraph if it is not blank:
	no CRMRule::first matches the entire line, and neither does the
	CRMRule::rest corresponding to the CRMRule::first that matched the first
	line of the paragraph.

	The prefix of the first line is unchanged.  The prefix of the following
	lines is calculated from calling (CRMRule::first)->Replace() on the
	prefix of the first line.

 ******************************************************************************/

void
JTextEditor16::SetCRMRuleList
	(
	CRMRuleList*	ruleList,
	const JBoolean	teOwnsRuleList
	)
{
	ClearCRMRuleList();

	itsCRMRuleList      = ruleList;
	itsOwnsCRMRulesFlag = teOwnsRuleList;
}

/******************************************************************************
 ClearCRMRuleList

	With no other rules, the default is to match [ \t]*

 ******************************************************************************/

void
JTextEditor16::ClearCRMRuleList()
{
	if (itsOwnsCRMRulesFlag && itsCRMRuleList != NULL)
		{
		itsCRMRuleList->DeleteAll();
		delete itsCRMRuleList;
		}

	itsCRMRuleList      = NULL;
	itsOwnsCRMRulesFlag = kJFalse;
}

/******************************************************************************
 CRMRule functions

 ******************************************************************************/

JRegex*
JTextEditor16::CRMRule::CreateFirst
	(
	const JCharacter16* pattern,
	const JCharacter16* replacePattern
	)
{
#ifdef _TODO
	JRegex* r = new JRegex(pattern);
	assert( r != NULL );
	const JError err = r->SetReplacePattern(replacePattern);
	assert_ok( err );
	return r;
#else
	return new JRegex("");
#endif
}

JRegex*
JTextEditor16::CRMRule::CreateRest
	(
	const JCharacter16* pattern
	)
{
#ifdef _TODO
	JRegex* r = new JRegex(pattern);
	assert( r != NULL );
	return r;
#else
	return new JRegex("");
#endif
}

/******************************************************************************
 CRMRuleList functions

 ******************************************************************************/

JTextEditor16::CRMRuleList::CRMRuleList
	(
	const CRMRuleList& source
	)
	:
	JArray<CRMRule>()
{
#ifdef _TODO
	const JSize count = source.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CRMRule r = source.GetElement(i);
		AppendElement(
			CRMRule((r.first)->GetPattern(), (r.rest)->GetPattern(),
					(r.first)->GetReplacePattern()));
		}
#endif
}

void
JTextEditor16::CRMRuleList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		CRMRule r = GetElement(i);
		delete r.first;
		delete r.rest;
		}

	RemoveAll();
}

/******************************************************************************
 SetCRMLineWidth

 ******************************************************************************/

void
JTextEditor16::SetCRMLineWidth
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMLineWidth = charCount;
}

/******************************************************************************
 SetCRMTabCharCount

 ******************************************************************************/

void
JTextEditor16::SetCRMTabCharCount
	(
	const JSize charCount
	)
{
	assert( charCount > 0 );
	itsCRMTabCharCount = charCount;
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JTextEditor16::Undo()
{
	assert( itsUndoState == kIdle );

	if (TEIsDragging())
		{
		return;
		}

	JTEUndoBase16* undo;
	const JBoolean hasUndo = GetCurrentUndo(&undo);
	if (hasUndo && itsType == kFullEditor)
		{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
	else if (hasUndo)
		{
		ClearUndo();
		}
}

/******************************************************************************
 Redo

 ******************************************************************************/

void
JTextEditor16::Redo()
{
	assert( itsUndoState == kIdle );

	if (TEIsDragging())
		{
		return;
		}

	JTEUndoBase16* undo;
	const JBoolean hasUndo = GetCurrentRedo(&undo);
	if (hasUndo && itsType == kFullEditor)
		{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
	else if (hasUndo)
		{
		ClearUndo();
		}
}

/******************************************************************************
 DeactivateCurrentUndo

 ******************************************************************************/

void
JTextEditor16::DeactivateCurrentUndo()
{
	JTEUndoBase16* undo = NULL;
	if (GetCurrentUndo(&undo))
		{
		undo->Deactivate();
		}
}

/******************************************************************************
 ClearUndo

	Avoid calling this whenever possible since it throws away -all-
	undo information.

 ******************************************************************************/

void
JTextEditor16::ClearUndo()
{
	delete itsUndo;
	itsUndo = NULL;

	if (itsUndoList != NULL)
		{
		itsUndoList->CleanOut();
		}
	itsFirstRedoIndex = 1;
	ClearLastSaveLocation();
}

/******************************************************************************
 UseMultipleUndo

	You probably never need to turn off multiple undo unless you
	are running out of memory.

	If you call this in your constructor, you should call
	SetLastSaveLocation() afterwards.

 ******************************************************************************/

void
JTextEditor16::UseMultipleUndo
	(
	const JBoolean useMultiple
	)
{
	if (useMultiple && itsUndoList == NULL)
		{
		ClearUndo();

		itsUndoList = new JPtrArray<JTEUndoBase16>(JPtrArrayT::kDeleteAll,
												 itsMaxUndoCount+1);
		assert( itsUndoList != NULL );
		}
	else if (!useMultiple && itsUndoList != NULL)
		{
		ClearUndo();

		delete itsUndoList;
		itsUndoList = NULL;
		}
}

/******************************************************************************
 SetUndoDepth

 ******************************************************************************/

void
JTextEditor16::SetUndoDepth
	(
	const JSize maxUndoCount
	)
{
	assert( maxUndoCount > 0 );

	itsMaxUndoCount = maxUndoCount;
	ClearOutdatedUndo();
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

JBoolean
JTextEditor16::GetCurrentUndo
	(
	JTEUndoBase16** undo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex > 1)
		{
		*undo = itsUndoList->NthElement(itsFirstRedoIndex - 1);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*undo = itsUndo;
		return JConvertToBoolean( *undo != NULL );
		}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

JBoolean
JTextEditor16::GetCurrentRedo
	(
	JTEUndoBase16** redo
	)
	const
{
	if (itsUndoList != NULL && itsFirstRedoIndex <= itsUndoList->GetElementCount())
		{
		*redo = itsUndoList->NthElement(itsFirstRedoIndex);
		return kJTrue;
		}
	else if (itsUndoList != NULL)
		{
		return kJFalse;
		}
	else
		{
		*redo = itsUndo;
		return JConvertToBoolean( *redo != NULL );
		}
}

/******************************************************************************
 NewUndo (private)

	Register a new Undo object.

	itsFirstRedoIndex points to the first redo object in itsUndoList.
	1 <= itsFirstRedoIndex <= itsUndoList->GetElementCount()+1
	Minimum => everything is redo
	Maximum => everything is undo

 ******************************************************************************/

void
JTextEditor16::NewUndo
	(
	JTEUndoBase16*	undo,
	const JBoolean	isNew
	)
{
	if (!isNew)
		{
		if (itsBcastAllTextChangedFlag)
			{
			Broadcast(TextChanged());
			}
		return;
		}

	if (itsUndoList != NULL && itsUndoState == kIdle)
		{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
			{
			itsUndoList->DeleteElement(i);
			}

		if (itsLastSaveRedoIndex > 0 &&
			itsFirstRedoIndex < JIndex(itsLastSaveRedoIndex))
			{
			ClearLastSaveLocation();
			}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		ClearOutdatedUndo();
		assert( !itsUndoList->IsEmpty() );
		}

	else if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(kJTrue);
		undo->Deactivate();
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		undo->Deactivate();
		}

	else
		{
		delete itsUndo;
		itsUndo = undo;
		}

	Broadcast(TextChanged());
}

/******************************************************************************
 SameUndo (private)

	Called by Undo objects to notify us that there was actually nothing
	to do, so the same object can be re-used.

 ******************************************************************************/

void
JTextEditor16::SameUndo
	(
	JTEUndoBase16* undo
	)
{
	assert( itsUndoState != kIdle );

	if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		assert( undo == itsUndoList->NthElement(itsFirstRedoIndex) );

		undo->SetRedo(kJTrue);
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		assert( undo == itsUndoList->NthElement(itsFirstRedoIndex) );
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		}

	else if (itsUndoList == NULL)
		{
		assert( undo == itsUndo );
		}

	undo->Deactivate();

	// nothing changed, so we don't broadcast TextChanged
}

/******************************************************************************
 ReplaceUndo (private)

 ******************************************************************************/

void
JTextEditor16::ReplaceUndo
	(
	JTEUndoBase16* oldUndo,
	JTEUndoBase16* newUndo
	)
{
#ifndef NDEBUG

	assert( itsUndoState != kIdle );

	if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex - 1) );
		}
	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() &&
				oldUndo == itsUndoList->GetElement(itsFirstRedoIndex) );
		}
	else
		{
		assert( oldUndo == itsUndo );
		}

#endif

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 ClearOutdatedUndo (private)

 ******************************************************************************/

void
JTextEditor16::ClearOutdatedUndo()
{
	if (itsUndoList != NULL)
		{
		JSize undoCount = itsUndoList->GetElementCount();
		while (undoCount > itsMaxUndoCount)
			{
			itsUndoList->DeleteElement(1);
			undoCount--;
			itsFirstRedoIndex--;
			itsLastSaveRedoIndex--;

			// If we have to throw out redo's, we have to toss everything.

			if (itsFirstRedoIndex == 0)
				{
				ClearUndo();
				break;
				}
			}
		}
}

/******************************************************************************
 GetTypingUndo (private)

	Return the active JTEUndoTyping16 object.  If the current undo object is
	not an active JTEUndoTyping16 object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTyping16*
JTextEditor16::GetTypingUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoTyping16* typingUndo = NULL;

	JTEUndoBase16* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(typingUndo = undo->CastToJTEUndoTyping16()) != NULL &&
		typingUndo->IsActive())
		{
		assert( itsSelection.IsEmpty() );
		*isNew = kJFalse;
		return typingUndo;
		}
	else
		{
		typingUndo = new JTEUndoTyping16(this);
		assert( typingUndo != NULL );

		*isNew = kJTrue;
		return typingUndo;
		}
}

/******************************************************************************
 GetStyleUndo (private)

	Return the active JTEUndoStyle16 object.  If the current undo object is
	not an active JTEUndoStyle16 object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoStyle16*
JTextEditor16::GetStyleUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoStyle16* styleUndo = NULL;

	JTEUndoBase16* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(styleUndo = undo->CastToJTEUndoStyle16()) != NULL &&
		styleUndo->IsActive())
		{
		assert( !itsSelection.IsEmpty() );
		*isNew = kJFalse;
		return styleUndo;
		}
	else
		{
		styleUndo = new JTEUndoStyle16(this);
		assert( styleUndo != NULL );

		*isNew = kJTrue;
		return styleUndo;
		}
}

JTEUndoStyle16*
JTextEditor16::GetStyleUndo
	(
	JBoolean* isNew,
	const JIndex range_start,
	const JIndex range_end
	)
{
	JTEUndoStyle16* styleUndo = NULL;

	JTEUndoBase16* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(styleUndo = undo->CastToJTEUndoStyle16()) != NULL &&
		styleUndo->IsActive())
		{
		*isNew = kJFalse;
		return styleUndo;
		}
	else
		{
		styleUndo = new JTEUndoStyle16(this, range_start, range_end);
		assert( styleUndo != NULL );

		*isNew = kJTrue;
		return styleUndo;
		}
}

/******************************************************************************
 GetTabShiftUndo (private)

	Return the active JTEUndoTabShift16 object.  If the current undo object is
	not an active JTEUndoTabShift16 object, we create a new one that is active.

	If we create a new object, *isNew = kJTrue, and the caller is required
	to call NewUndo() after changing the text.

 ******************************************************************************/

JTEUndoTabShift16*
JTextEditor16::GetTabShiftUndo
	(
	JBoolean* isNew
	)
{
	JTEUndoTabShift16* tabShiftUndo = NULL;

	JTEUndoBase16* undo = NULL;
	if (GetCurrentUndo(&undo) &&
		(tabShiftUndo = undo->CastToJTEUndoTabShift16()) != NULL &&
		tabShiftUndo->IsActive())
		{
		*isNew = kJFalse;
		return tabShiftUndo;
		}
	else
		{
		tabShiftUndo = new JTEUndoTabShift16(this);
		assert( tabShiftUndo != NULL );

		*isNew = kJTrue;
		return tabShiftUndo;
		}
}

/******************************************************************************
 InsertText (private)

	*** Caller must call Recalc().  Nothing can be selected.

	Returns number of characters that were actually inserted.

	style can be NULL.

 ******************************************************************************/

JSize
JTextEditor16::InsertText
	(
	const JIndex			charIndex,
	const JCharacter16*		text,
	const JRunArray<Font>*	style	// can be NULL
	)
{
	const JSize textLen =
		(style != NULL ? style->GetElementCount() : strlen16(text));

	JSize insertedLength = 0;
	if (textLen > 0)
		{
		assert( style == NULL || textLen == style->GetElementCount() );
		assert( itsSelection.IsEmpty() );

		JString16* newText          = NULL;
		JRunArray<Font>* newStyle = NULL;

		// remove illegal characters

		if (ContainsIllegalChars(text, textLen))
			{
			if (newText == NULL)
				{
				newText = new JString16(text, textLen);
				assert( newText != NULL );

				if (style != NULL)
					{
					newStyle = new JRunArray<Font>(*style);
					assert( newStyle != NULL );
					}
				}

			RemoveIllegalChars(newText, newStyle);
			}

		// convert from Macintosh or DOS format

		if (strchr(newText != NULL ? (const char*)newText->GetCString() : (const char*)text, '\r') != NULL)
			{
			if (newText == NULL)
				{
				newText = new JString16(text, textLen);
				assert( newText != NULL );

				if (style != NULL)
					{
					newStyle = new JRunArray<Font>(*style);
					assert( newStyle != NULL );
					}
				}

			JIndex i = 1;
			JString16 temp;
			temp.FromASCII("\r");
			while (newText->LocateNextSubstring(temp, &i))
				{
				if (i < newText->GetLength() &&
					newText->GetCharacter(i+1) == '\n')
					{
					newText->RemoveSubstring(i,i);
					if (newStyle != NULL)
						{
						newStyle->RemoveElement(i);
						}
					}
				else
					{
					newText->SetCharacter(i, '\n');
					}
				}
			}

		// allow derived classes to make additional changes
		// (last so we don't pass anything illegal to FilterText())

		JBoolean okToInsert = kJTrue;
		if (NeedsToFilterText(newText != NULL ? newText->GetCString() : text))
			{
			if (newText == NULL)
				{
				newText = new JString16(text, textLen);
				assert( newText != NULL );

				if (style != NULL)
					{
					newStyle = new JRunArray<Font>(*style);
					assert( newStyle != NULL );
					}
				}

			okToInsert = FilterText(newText, newStyle);
			}

		// insert the text

		if (okToInsert)
			{
			itsBuffer->InsertSubstring(newText != NULL ? newText->GetCString() : text, charIndex);
			if (newStyle != NULL)
				{
				itsStyles->InsertElementsAtIndex(charIndex, *newStyle, 1, newStyle->GetElementCount());
				}
			else if (style != NULL)
				{
				itsStyles->InsertElementsAtIndex(charIndex, *style, 1, style->GetElementCount());
				}
			else
				{
				itsStyles->InsertElementsAtIndex(charIndex, itsInsertionFont,
												 newText != NULL ?
												 	newText->GetLength() : textLen);
				}

			insertedLength = (newText != NULL ? newText->GetLength() : textLen);
			}

		delete newText;
		delete newStyle;
		}

	return insertedLength;
}

/******************************************************************************
 NeedsToFilterText (virtual protected)

	Derived classes should return kJTrue if FilterText() needs to be called.
	This is an optimization, to avoid copying the data if nothing needs to
	be done to it.

 ******************************************************************************/

JBoolean
JTextEditor16::NeedsToFilterText
	(
	const JCharacter16* text
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 FilterText (virtual protected)

	Derived classes can override this to enforce restrictions on the text.
	Return kJFalse if the text cannot be used at all.

	*** Note that style may be NULL or empty if the data was plain text.

 ******************************************************************************/

JBoolean
JTextEditor16::FilterText
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
{
	return kJTrue;
}

/******************************************************************************
 DeleteText (private)

	*** Caller must call Recalc().

 ******************************************************************************/

void
JTextEditor16::DeleteText
	(
	const JIndexRange& range
	)
{
	itsBuffer->RemoveSubstring(range);
	itsStyles->RemoveNextElements(range.first, range.GetLength());
}

void
JTextEditor16::DeleteText
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
{
	itsBuffer->RemoveSubstring(startIndex, endIndex);
	itsStyles->RemoveNextElements(startIndex, endIndex - startIndex + 1);
}

/******************************************************************************
 Paginate (protected)

	Returns breakpoints for cutting text into pages.  The first breakpoint
	is always zero, and the last breakpoint is the height of the text.
	Thus, it is easy to calculate the width of what is printed on each page
	from (breakpt->GetElement(i+1) - breakpt->GetElement(i) + 1).

 ******************************************************************************/

void
JTextEditor16::Paginate
	(
	const JCoordinate		pageHeight,
	JArray<JCoordinate>*	breakpts
	)
	const
{
	assert( pageHeight > 0 );

	breakpts->RemoveAll();
	breakpts->AppendElement(0);

	const JSize count = GetLineCount();

	JRunArrayIterator<LineGeometry> iter(itsLineGeom);
	LineGeometry geom;

	JIndex prev = 1, i = 0;
	JCoordinate h = 0;
	do
		{
		// find the number of strips that will fit on this page

		while (i < count && h <= pageHeight)
			{
			i++;
			const JBoolean ok = iter.Next(&geom);
			assert( ok );
			h += geom.height;
			}

		JCoordinate pageH = h;
		if (h > pageHeight && i > prev)
			{
			// The last line didn't fit on the page,
			// so leave it for the next page.

			pageH -= geom.height;
			i--;
			const JBoolean ok = iter.Prev(&geom);
			assert( ok );
			h = 0;
			}
		else if (h > pageHeight)
			{
			// The line won't fit on any page.  Put
			// as much as possible on this page and leave
			// the rest for the next page.

			pageH = pageHeight;
			h    -= pageHeight;
			}
		else
			{
			// Everything fits on the page, so there is no residual.

			h = 0;
			}

		breakpts->AppendElement(breakpts->GetLastElement() + pageH);
		prev = i;
		}
		while (i < count || h > 0);
}

/******************************************************************************
 Print

	This function could be optimized to save the necessary state so
	TESetBoundsWidth() doesn't have to be called again at the end.

 ******************************************************************************/

void
JTextEditor16::Print
	(
	JPagePrinter& p
	)
{
	// avoid notifing GUI from within Recalc()
	// (We put it here to allow OpenDocument() to adjust things.)

	itsIsPrintingFlag = kJTrue;

	if (!p.OpenDocument())
		{
		itsIsPrintingFlag = kJFalse;
		return;
		}

	const JCoordinate headerHeight = GetPrintHeaderHeight(p);
	const JCoordinate footerHeight = GetPrintFooterHeight(p);

	// We deactivate before printing so the selection or the
	// caret won't be printed.

	const JBoolean savedActive = TEIsActive();
	TEDeactivate();

	// adjust to the width of the page

	const JCoordinate savedWidth = TEGetBoundsWidth();
	const JCoordinate pageWidth  = p.GetPageWidth();
	TESetBoundsWidth(pageWidth);

	// paginate

	JArray<JCoordinate> breakpts;
	Paginate(p.GetPageHeight()-headerHeight-footerHeight, &breakpts);

	// print each page

	const JSize pageCount = breakpts.GetElementCount() - 1;

	JBoolean cancelled = kJFalse;
	for (JIndex i=1; i<=pageCount; i++)
		{
		if (!p.NewPage())
			{
			cancelled = kJTrue;
			break;
			}

		if (headerHeight > 0)
			{
			DrawPrintHeader(p, headerHeight);
			p.LockHeader(headerHeight);
			}
		if (footerHeight > 0)
			{
			DrawPrintFooter(p, footerHeight);
			p.LockFooter(footerHeight);
			}

		JPoint topLeft(0, breakpts.GetElement(i));
		JPoint bottomRight(pageWidth, breakpts.GetElement(i+1));
		JRect clipRect(JPoint(0,0), bottomRight - topLeft);
		p.SetClipRect(clipRect);

		p.ShiftOrigin(-topLeft);
		TEDraw(p, JRect(topLeft,bottomRight));
		}

	if (!cancelled)
		{
		p.CloseDocument();
		}

	itsIsPrintingFlag = kJFalse;

	// restore the original state and width

	if (savedActive)
		{
		TEActivate();
		}

	TESetBoundsWidth(savedWidth);
}

/******************************************************************************
 Print header and footer (virtual protected)

	Derived classes can override these functions if they want to
	print a header or a footer.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetPrintHeaderHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

JCoordinate
JTextEditor16::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return 0;
}

void
JTextEditor16::DrawPrintHeader
	(
	JPagePrinter&		p,
	const JCoordinate	headerHeight
	)
{
}

void
JTextEditor16::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
}

/******************************************************************************
 GetCmdStatus (protected)

	Returns an array indicating which commands can be performed in the
	current state.

 ******************************************************************************/

JArray<JBoolean>
JTextEditor16::GetCmdStatus
	(
	JString*	crmActionText,
	JString*	crm2ActionText,
	JBoolean*	isReadOnly
	)
	const
{
	JArray<JBoolean> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
		{
		flags.AppendElement(kJFalse);
		}

	*isReadOnly = kJTrue;

	if (itsType == kStaticText || !itsActiveFlag)
		{
		return flags;
		}

	// Edit menu

	if (itsSelection.IsEmpty())
		{
		*crmActionText  = kCRMCaretActionText;
		*crm2ActionText = kCRM2CaretActionText;
		}
	else
		{
		flags.SetElement(kCopyCmd, kJTrue);
		*crmActionText  = kCRMSelectionActionText;
		*crm2ActionText = kCRM2SelectionActionText;
		}

	flags.SetElement(kSelectAllCmd,      kJTrue);
	flags.SetElement(kToggleReadOnlyCmd, kJTrue);

	if (itsType == kFullEditor)
		{
		*isReadOnly = kJFalse;

		flags.SetElement(kPasteCmd, kJTrue);
		if (!itsSelection.IsEmpty())
			{
			flags.SetElement(kCutCmd,       kJTrue);
			flags.SetElement(kDeleteSelCmd, kJTrue);
			}

		if (!itsBuffer->IsEmpty())
			{
			flags.SetElement(kCleanRightMarginCmd,  kJTrue);
			flags.SetElement(kCoerceRightMarginCmd, kJTrue);
			flags.SetElement(kShiftSelLeftCmd,      kJTrue);
			flags.SetElement(kShiftSelRightCmd,     kJTrue);
			flags.SetElement(kForceShiftSelLeftCmd, kJTrue);
			}

		if (itsUndoList != NULL)
			{
			flags.SetElement(kUndoCmd, JConvertToBoolean( itsFirstRedoIndex > 1 ));
			flags.SetElement(kRedoCmd,
				JConvertToBoolean( itsFirstRedoIndex <= itsUndoList->GetElementCount() ));
			}
		else if (itsUndo != NULL)
			{
			const JBoolean isRedo = itsUndo->IsRedo();
			flags.SetElement(kUndoCmd, JNegate(isRedo));
			flags.SetElement(kRedoCmd, isRedo);
			}
		}

	// Search & Replace menu

	flags.SetElement(kFindDialogCmd, kJTrue);

	if (!itsBuffer->IsEmpty())
		{
		flags.SetElement(kFindClipboardBackwardCmd, kJTrue);
		flags.SetElement(kFindClipboardForwardCmd,  kJTrue);

		if (TEHasSearchText())
			{
			flags.SetElement(kFindNextCmd,     kJTrue);
			flags.SetElement(kFindPreviousCmd, kJTrue);

			if (itsType == kFullEditor)
				{
				flags.SetElement(kReplaceAllBackwardCmd, kJTrue);
				flags.SetElement(kReplaceAllForwardCmd,  kJTrue);

				if (HasSelection())
					{
					flags.SetElement(kReplaceSelectionCmd,      kJTrue);
					flags.SetElement(kReplaceFindNextCmd,       kJTrue);
					flags.SetElement(kReplaceFindPrevCmd,       kJTrue);
					flags.SetElement(kReplaceAllInSelectionCmd, kJTrue);
					}
				}
			}
		}

	if (HasSelection())
		{
		flags.SetElement(kEnterSearchTextCmd,       kJTrue);
		flags.SetElement(kEnterReplaceTextCmd,      kJTrue);
		flags.SetElement(kFindSelectionBackwardCmd, kJTrue);
		flags.SetElement(kFindSelectionForwardCmd,  kJTrue);
		}

	return flags;
}

/******************************************************************************
 TEDraw (protected)

	Draw everything that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor16::TEDraw
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	// shade the dead-zone
/*
	if (itsLeftMarginWidth > kMinLeftMarginWidth)
		{
		const JColorIndex savedColor = p.GetPenColor();
		const JBoolean savedFill     = p.IsFilling();
		p.SetPenColor(itsColormap->GetGray90Color());
		p.SetFilling(kJTrue);
		const JRect& clipRect = p.GetClipRect();
		p.Rect(0, clipRect.top, itsLeftMarginWidth-1, clipRect.height());
		p.SetPenColor(savedColor);
		p.SetFilling(savedFill);
		}
*/
	p.ShiftOrigin(itsLeftMarginWidth, 0);

	// draw the text

	TEDrawText(p, rect);

	// if DND, draw drop location and object being dragged

	if ((itsDragType == kDragAndDrop || itsDragType == kLocalDragAndDrop) &&
		itsDropLoc.charIndex > 0)
		{
		TEDrawCaret(p, itsDropLoc);

		if (itsDragType == kLocalDragAndDrop)
			{
			JRect r = CalcLocalDNDRect(itsPrevPt);
			r.Shift(-itsLeftMarginWidth, 0);
			p.SetPenColor(itsDragColor);
			p.Rect(r);
			}
		}

	// otherwise, draw insertion caret

	else if (itsActiveFlag && itsCaretVisibleFlag && itsSelection.IsEmpty() &&
			 itsType == kFullEditor)
		{
		TEDrawCaret(p, itsCaretLoc);
		}

	// clean up

	p.ShiftOrigin(-itsLeftMarginWidth, 0);
}

/******************************************************************************
 TEDrawText (private)

	Draw the text that is visible in the given rectangle.

 ******************************************************************************/

void
JTextEditor16::TEDrawText
	(
	JPainter&		p,
	const JRect&	rect
	)
{
	if (IsEmpty())
		{
		return;
		}

	const JSize lineCount = GetLineCount();

	JCoordinate h;
	const JIndex startLine = CalcLineIndex(rect.top, &h);

	// draw selection region

	if (itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText)
		{
		TEDrawSelection(p, rect, startLine, h);
		}

	// draw text, one line at a time

	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(GetLineStart(startLine), &runIndex, &firstInRun);
	assert( found );

	JRunArrayIterator<LineGeometry> iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=lineCount; i++)
		{
		const JBoolean ok = iter.Next(&geom);
		assert( ok );

		TEDrawLine(p, h, geom, i, &runIndex, &firstInRun);

		h += geom.height;
		if (h >= rect.bottom)
			{
			break;
			}
		}
}

/******************************************************************************
 TEDrawLine (private)

	Draw the text on the given line.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

inline void
teDrawSpaces
	(
	const JCharacter16*					buffer,
	const JRunArray<JTextEditor16::Font>&	styles,
	const JIndex						startChar,
	const JInteger						direction,		// +1/-1
	const JIndex						trueRunEnd,
	JPainter&							p,
	const JCoordinate					left,
	const JCoordinate					ycenter,
	const JTextEditor16::Font&			f,
	const JColorIndex					wsColor
	)
{
	JCoordinate l = left;
	JSize w       = (p.GetFontManager())->GetCharWidth(f.id, f.size, f.style, ' ');

	p.SetLineWidth(1);
	p.SetPenColor(wsColor);

	JIndex i = startChar;
	JPoint pt;
	while (buffer[i-1] == ' ')
		{
		if ((direction == +1 && i > trueRunEnd) ||
			(direction == -1 && i < trueRunEnd))
			{
			JTextEditor16::Font f = styles.GetElement(i);
			w = (p.GetFontManager())->GetCharWidth(f.id, f.size, f.style, ' ');
			}

		if (direction == -1)
			{
			l -= w;
			}
		pt.Set(l + w/2 - 1, ycenter);
		p.Point(pt);
		if (direction == +1)
			{
			l += w;
			}
		i += direction;
		}
}

void
JTextEditor16::TEDrawLine
	(
	JPainter&			p,
	const JCoordinate	top,
	const LineGeometry&	geom,
	const JIndex		lineIndex,
	JIndex*				runIndex,
	JIndex*				firstInRun
	)
{
	TEDrawInMargin(p, JPoint(-itsLeftMarginWidth, top), geom, lineIndex);

	const JSize lineLength = GetLineLength(lineIndex);
	assert( lineLength > 0 );

	JBoolean lineEndsWithNewline = kJFalse;

	JIndex startChar = GetLineStart(lineIndex);
	JIndex endChar   = startChar + lineLength - 1;
	if (itsBuffer->GetCharacter(endChar) == '\n')	// some fonts draw stuff for \n
		{
		endChar--;
		lineEndsWithNewline = kJTrue;
		}

	// Handle alignment
	JCoordinate dx = 0;
	const Font& af = itsStyles->GetRunDataRef(*runIndex);
	if (af.align != kAlignLeft)
	{
		JCoordinate drawwidth = TEGetBoundsWidth();
		JCoordinate linewidth = GetLineWidth(lineIndex);
		if (af.align == kAlignCenter)
			dx = (drawwidth - linewidth) / 2;
		else if (af.align == kAlignRight)
			dx = drawwidth - linewidth;
	}

	JCoordinate left = dx;

	JBoolean wsInit = kJFalse;
	JRect wsRect;
	JCoordinate wsYCenter = 0;
	if (itsDrawWhitespaceFlag)
		{
		wsRect.Set(top+3, left, top+geom.height-4, left);
		if (wsRect.height() < 3)
			{
			wsRect.top    -= (3 - wsRect.height())/2;
			wsRect.bottom += 3 - wsRect.height();
			}
		wsRect.bottom -= (wsRect.height() % 2);
		wsRect.right  += wsRect.height();

		wsYCenter = wsRect.ycenter();
		}

	JString16 s;
	while (startChar <= endChar)
		{
		JSize runLength        = itsStyles->GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startChar - *firstInRun;
		if (startChar + runLength-1 > endChar)
			{
			runLength = endChar - startChar + 1;
			}

		const Font& f = itsStyles->GetRunDataRef(*runIndex);
		s             = itsBuffer->GetSubstring(startChar, startChar + runLength-1);

		// If the line starts with spaces, we have to draw them.

		if (!wsInit && itsDrawWhitespaceFlag)
			{
			teDrawSpaces(*itsBuffer, *itsStyles, startChar, +1, trueRunEnd,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}
		wsInit = kJTrue;

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabIndex;
		JCharacter16 temp[] = {'\t', 0};
		if (s.LocateSubstring(temp, &tabIndex))
			{
			runLength = tabIndex - 1;
			if (runLength > 0)
				{
				s = itsBuffer->GetSubstring(startChar, startChar + runLength-1);
				}
			}
		else
			{
			tabIndex = 0;
			}

		if (runLength > 0)
			{
			p.SetFont(f.id, f.size, f.style);
			JCoordinate ascent,descent;
			p.GetLineHeight(&ascent, &descent);
			JSize str_width = p.GetStringWidth16(s);
			
			// Use background color if text is NOT selected
			if ((f.style.backcolor != 0) &&
				(itsSelection.IsEmpty() ||
				 (!itsSelection.Contains(startChar) &&
				  !itsSelection.Contains(startChar + runLength-1))))
			{
				JColorIndex old_pen_color = p.GetPenColor();
				p.SetPenColor(f.style.backcolor);
				p.SetFilling(kJTrue);
				p.Rect(left, top + geom.ascent - ascent, str_width, ascent + descent);
				p.SetFilling(kJFalse);
				p.SetPenColor(old_pen_color);
			}

			p.String16(left, top + geom.ascent - ascent, s);

			// we only care if there is more text on the line

			if (startChar + runLength <= endChar || itsDrawWhitespaceFlag)
				{
				left += p.GetStringWidth16(s);
				}
			}

		if (tabIndex > 0)
			{
			if (itsDrawWhitespaceFlag)
				{
				p.SetLineWidth(1);
				p.SetPenColor(itsWhitespaceColor);

				wsRect.left  = left;
				wsRect.right = left + wsRect.height();

				const JCoordinate xc = wsRect.xcenter();
				const JCoordinate yc = wsRect.ycenter();
				const JPoint top   (xc, wsRect.top);
				const JPoint left  (wsRect.left, yc);
				const JPoint bottom(xc, wsRect.bottom);
				const JPoint right (wsRect.right, yc);

				p.Line(left, right);
				p.Line(top, right);
				p.LineTo(bottom);
				}

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(*itsBuffer, *itsStyles,
							 startChar + runLength - 1, -1, *firstInRun,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}

			left += GetTabWidth(startChar + runLength, left);
			runLength++;

			if (itsDrawWhitespaceFlag)
				{
				teDrawSpaces(*itsBuffer, *itsStyles,
							 startChar + runLength, +1, trueRunEnd,
							 p, left, wsYCenter, f, itsWhitespaceColor);
				}
			}

		startChar += runLength;
		if (startChar > trueRunEnd)		// move to next style run
			{
			*firstInRun = startChar;
			(*runIndex)++;
			}
		}

	// account for newline

	if (lineEndsWithNewline)
		{
		if (itsDrawWhitespaceFlag)
			{
			p.SetLineWidth(1);
			p.SetPenColor(itsWhitespaceColor);

			JRect rect(top+3, left, top+geom.height-2, left);
			rect.right += rect.height();
			const JCoordinate xc = rect.xcenter();
			const JCoordinate yc = rect.ycenter();

			JCoordinate delta = rect.height()/4;
			if (delta < 1)
				{
				delta = 1;
				}

			const JPoint pt1(xc, rect.top);
			const JPoint pt2(xc, rect.bottom);
			const JPoint pt3(xc - delta, rect.bottom - delta);
			const JPoint pt4(xc, rect.bottom - 2*delta);

			p.Line(pt1, pt2);
			p.LineTo(pt3);
			p.LineTo(pt4);

			const Font& f = itsStyles->GetRunDataRef(*runIndex);
			teDrawSpaces(*itsBuffer, *itsStyles, endChar, -1, *firstInRun,
						 p, left, wsYCenter, f, itsWhitespaceColor);
			}

		const JSize runLength = itsStyles->GetRunLength(*runIndex);
		if (startChar >= *firstInRun + runLength-1)
			{
			*firstInRun += runLength;
			(*runIndex)++;
			}
		}
}

/******************************************************************************
 TEDrawInMargin (virtual protected)

	Scribble in the margins, e.g., bookmarks, line numbers, etc.

 ******************************************************************************/

void
JTextEditor16::TEDrawInMargin
	(
	JPainter&			p,
	const JPoint&		topLeft,
	const LineGeometry&	geom,
	const JIndex		lineIndex
	)
{
}

/******************************************************************************
 TEDrawSelection (private)

	Draw the selection region.

 ******************************************************************************/

void
JTextEditor16::TEDrawSelection
	(
	JPainter&			p,
	const JRect&		rect,
	const JIndex		startVisLine,
	const JCoordinate	startVisLineTop
	)
{
	assert( itsActiveFlag && !itsSelection.IsEmpty() && itsType != kStaticText );

	// calculate intersection of selection region and drawing region

	JIndex startLine = GetLineForChar(itsSelection.first);
	JIndex endLine   = GetLineForChar(itsSelection.last);

	const JIndex origStartLine = startLine;
	const JIndex origEndLine   = endLine;

	JIndex startChar;
	JCoordinate x, y;

	JCoordinate endVisLineTop;
	const JIndex endVisLine = CalcLineIndex(rect.bottom, &endVisLineTop);
	if (startVisLine > endLine || endVisLine < startLine)
		{
		return;
		}
	else if (startVisLine > startLine)
		{
		startLine = startVisLine;
		startChar = GetLineStart(startVisLine);
		x         = 0;
		y         = startVisLineTop;
		}
	else
		{
		startChar = itsSelection.first;
		x         = GetCharLeft(CaretLocation(startChar, startLine));
		y         = GetLineTop(startLine);
		}

	if (endLine > endVisLine)
		{
		endLine = endVisLine;
		}

	// draw the selection region

	const JColorIndex savedColor = p.GetPenColor();
	const JBoolean savedFill     = p.IsFilling();
	if (itsSelActiveFlag)
		{
		p.SetPenColor(GetSelectionColor());
		p.SetFilling(kJTrue);
		}
	else
		{
		p.SetPenColor(GetSelectionOutlineColor());
		p.SetFilling(kJFalse);
		}

	const JCoordinate xmax = JMax(itsGUIWidth, itsWidth);

	JRunArrayIterator<LineGeometry> iter(itsLineGeom, kJIteratorStartBefore, startLine);
	LineGeometry geom;
	for (JIndex i=startLine; i<=endLine; i++)
		{
		const JBoolean ok = iter.Next(&geom);
		assert( ok );

		JCoordinate w;
		JIndex endChar = GetLineEnd(i);
		if (endChar > itsSelection.last)
			{
			endChar = itsSelection.last;
			w = GetStringWidth(startChar, endChar);
			
			// If selecting from the start of a line we must account
			// for alignment offset in the width
			if (x == 0 && w > 0)
				w += GetCharLeft(CaretLocation(startChar, i));
			}
		else
			{
			w = xmax - x;
			}

		const JCoordinate bottom = y + geom.height - 1;
		if (itsSelActiveFlag)
			{
			// solid rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (origStartLine == origEndLine)
			{
			// empty rectangle

			p.Rect(x, y, w, geom.height);
			}
		else if (i == origStartLine)
			{
			// top of selection region

			p.Line(0,bottom, x,bottom);
			p.LineTo(x,y);
			p.LineTo(x+w,y);
			p.LineTo(x+w,bottom);
			}
		else if (i == origEndLine)
			{
			// bottom of selection region

			p.Line(0,y, 0,bottom);
			p.LineTo(w,bottom);
			p.LineTo(w,y);
			p.LineTo(xmax,y);
			}
		else
			{
			// vertical sides of selection region

			p.Line(0,y, 0,bottom);
			p.Line(xmax,y, xmax,bottom);
			}

		startChar = endChar+1;

		x  = 0;		// all lines after the first start at the far left
		y += geom.height;
		}

	// clean up

	p.SetPenColor(savedColor);
	p.SetFilling(savedFill);
}

/******************************************************************************
 TEDrawCaret (private)

 ******************************************************************************/

void
JTextEditor16::TEDrawCaret
	(
	JPainter&				p,
	const CaretLocation&	caretLoc
	)
{
	assert( itsType == kFullEditor );

	JCoordinate x, y1, y2;

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())									// ends with newline
		{
		x  = -1;
		y1 = GetLineBottom(caretLoc.lineIndex) + 1;
		y2 = y1 + GetEWNHeight()-1;
		}
	else
		{
		x = GetCharLeft(caretLoc) - 1;
		if (x >= itsWidth)
			{
			x = itsWidth;		// keep inside bounds
			}

		y1 = GetLineTop(caretLoc.lineIndex);
		y2 = y1 + GetLineHeight(caretLoc.lineIndex)-1;
		}

	const JColorIndex savedColor = p.GetPenColor();
	p.SetPenColor(itsCaretColor);
	p.Line(x,y1, x,y2);
	p.SetPenColor(savedColor);
}

/******************************************************************************
 TEWillDragAndDrop (protected)

 ******************************************************************************/

JBoolean
JTextEditor16::TEWillDragAndDrop
	(
	const JPoint&	pt,
	const JBoolean	extendSelection,
	const JBoolean	dropCopy
	)
	const
{
	if (itsActiveFlag && itsType != kStaticText && itsPerformDNDFlag &&
		!itsSelection.IsEmpty() && !extendSelection)
		{
		return PointInSelection(pt);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PointInSelection (protected)

 ******************************************************************************/

JBoolean
JTextEditor16::PointInSelection
	(
	const JPoint& origPt
	)
	const
{
	if (HasSelection())
		{
		const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
		const CaretLocation caretLoc = CalcCaretLocation(pt);

		return JI2B(itsSelection.first <= caretLoc.charIndex &&
					caretLoc.charIndex <= itsSelection.last+1);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 TEHandleMouseDown (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleMouseDown
	(
	const JPoint&	origPt,
	const JSize		clickCount,
	const JBoolean	extendSelection,
	const JBoolean	partialWord
	)
{
	assert( itsActiveFlag );

	itsDragType         = kInvalidDrag;
	itsIsDragSourceFlag = kJFalse;
	if (itsType == kStaticText)
		{
		return;
		}

	DeactivateCurrentUndo();

	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	const CaretLocation caretLoc = CalcCaretLocation(pt);

	itsStartPt = itsPrevPt = pt;

	StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);

	if (!itsSelection.IsEmpty() && extendSelection &&
		caretLoc.charIndex <= itsSelection.first)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.last+1;
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (!itsSelection.IsEmpty() && extendSelection)
		{
		itsDragType =
			(itsPrevDragType == kInvalidDrag ? kSelectDrag : itsPrevDragType);
		itsSelectionPivot = itsSelection.first;
		itsPrevPt.x--;
		TEHandleMouseDrag(origPt);
		}
	else if (itsPerformDNDFlag && clickCount == 1 && !itsSelection.IsEmpty() &&
			 itsSelection.first <= caretLoc.charIndex &&
			 caretLoc.charIndex <= itsSelection.last+1)
		{
		itsDragType = kDragAndDrop;
		itsDropLoc  = CaretLocation(0,0);
		}
	else if (extendSelection)
		{
		itsDragType       = kSelectDrag;
		itsSelectionPivot = itsCaretLoc.charIndex;	// save this before SetSelection()

		if (caretLoc.charIndex < itsCaretLoc.charIndex)
			{
			SetSelection(caretLoc.charIndex, itsCaretLoc.charIndex-1);
			}
		else if (itsCaretLoc.charIndex < caretLoc.charIndex)
			{
			SetSelection(itsCaretLoc.charIndex, caretLoc.charIndex-1);
			}
		}
	else if (clickCount == 1)
		{
		if (caretLoc != itsCaretLoc)
			{
			SetCaretLocation(caretLoc);
			}
		itsDragType       = kSelectDrag;
		itsSelectionPivot = caretLoc.charIndex;
		}
	else if (clickCount == 2)
		{
		itsDragType = (partialWord ? kSelectPartialWordDrag : kSelectWordDrag);
		TEGetDoubleClickSelection(caretLoc.charIndex, partialWord, kJFalse,
								  &itsWordSelPivot);
		SetSelection(itsWordSelPivot);
		}
	else if (clickCount == 3)
		{
		itsDragType     = kSelectLineDrag;
		itsLineSelPivot = caretLoc.lineIndex;
		SetSelection(GetLineStart(caretLoc.lineIndex), GetLineEnd(caretLoc.lineIndex));
		}

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseDrag (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleMouseDrag
	(
	const JPoint& origPt
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType == kInvalidDrag || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDrag(origPt);

	StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	if (itsDragType == kSelectDrag && caretLoc.charIndex < itsSelectionPivot)
		{
		SetSelection(caretLoc.charIndex, itsSelectionPivot-1, kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectDrag && caretLoc.charIndex == itsSelectionPivot)
		{
		SetCaretLocation(caretLoc);
		}
	else if (itsDragType == kSelectDrag && caretLoc.charIndex > itsSelectionPivot)
		{
		SetSelection(itsSelectionPivot, caretLoc.charIndex-1, kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectWordDrag || itsDragType == kSelectPartialWordDrag)
		{
		JIndexRange range;
		TEGetDoubleClickSelection(caretLoc.charIndex,
								  JI2B(itsDragType == kSelectPartialWordDrag),
								  kJTrue, &range);
		SetSelection(JCovering(itsWordSelPivot, range), kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kSelectLineDrag)
		{
		SetSelection(GetLineStart( JMin(itsLineSelPivot, caretLoc.lineIndex) ),
					 GetLineEnd(   JMax(itsLineSelPivot, caretLoc.lineIndex) ),
					 kJFalse);
		BroadcastCaretMessages(caretLoc, kJTrue);
		}
	else if (itsDragType == kLocalDragAndDrop)
		{
		TERefreshCaret(itsDropLoc);
		CaretLocation dropLoc = CaretLocation(0,0);
		if (caretLoc.charIndex  <= itsSelection.first ||
			itsSelection.last+1 <= caretLoc.charIndex)
			{
			dropLoc = caretLoc;	// only drop outside selection
			}
		itsDropLoc = dropLoc;
		TERefreshCaret(itsDropLoc);
		TERefreshRect( CalcLocalDNDRect(itsPrevPt) );
		TERefreshRect( CalcLocalDNDRect(pt) );
		}
	else if (itsDragType == kDragAndDrop && JMouseMoved(itsStartPt, pt))
		{
		itsDragType = kInvalidDrag;		// wait for TEHandleDNDEnter()
		if (TEBeginDND())
			{
			itsIsDragSourceFlag = kJTrue;
			// switches to TEHandleDND*()
			}
		else if (itsType == kFullEditor)
			{
			itsDragType = kLocalDragAndDrop;
			}
		}

	itsPrevPt = pt;

	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleMouseUp (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleMouseUp
	(
	const JBoolean dropCopy
	)
{
	// handle local DND

	if (itsDragType == kLocalDragAndDrop && itsDropLoc.charIndex != 0)
		{
		const JIndex charIndex = itsDropLoc.charIndex;
		itsDropLoc.charIndex   = 0;		// no longer valid, so don't let TEDraw() draw it
		itsDropLoc.lineIndex   = 0;
		DropSelection(charIndex, dropCopy);
		TERefreshRect( CalcLocalDNDRect(itsPrevPt) );	// get rid of rectangle
		}
	else if (itsDragType == kLocalDragAndDrop || itsDragType == kDragAndDrop)
		{
		const CaretLocation startLoc = CalcCaretLocation(itsStartPt);
		if (CalcCaretLocation(itsPrevPt) == startLoc)
			{
			SetCaretLocation(startLoc);
			}
		}

	// set itsSelectionPivot for "shift-arrow" selection

	else if (!itsSelection.IsEmpty() &&
			 (itsDragType == kSelectDrag ||
			  itsDragType == kSelectPartialWordDrag ||
			  itsDragType == kSelectWordDrag ||
			  itsDragType == kSelectLineDrag))
		{
		if ((itsDragType == kSelectWordDrag ||
			 itsDragType == kSelectPartialWordDrag) &&
			itsWordSelPivot.last  == itsSelection.last &&
			itsWordSelPivot.first != itsSelection.first)
			{
			itsSelectionPivot = itsSelection.last+1;
			}
		else if (itsDragType == kSelectLineDrag &&
				 GetLineEnd(itsLineSelPivot)   == itsSelection.last &&
				 GetLineStart(itsLineSelPivot) != itsSelection.first)
			{
			itsSelectionPivot = itsSelection.last+1;
			}

		if (itsSelectionPivot == itsSelection.last+1)
			{
			itsCaretX = GetCharLeft(CalcCaretLocation(itsSelection.first));
			}
		else
			{
			itsSelectionPivot = itsSelection.first;
			itsCaretX = GetCharRight(CalcCaretLocation(itsSelection.last));
			}
		}

	// save drag type for "extend" drag

	if (itsDragType == kSelectDrag ||
		itsDragType == kSelectPartialWordDrag ||
		itsDragType == kSelectWordDrag ||
		itsDragType == kSelectLineDrag)
		{
		itsPrevDragType = itsDragType;
		}
	else
		{
		itsPrevDragType = kInvalidDrag;
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDEnter (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleDNDEnter()
{
	itsDragType = kDragAndDrop;
	itsDropLoc  = CaretLocation(0,0);
	TERefreshCaret(itsCaretLoc);		// hide the typing caret
}

/******************************************************************************
 TEHandleDNDHere (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleDNDHere
	(
	const JPoint&	origPt,
	const JBoolean	dropOnSelf
	)
{
	const JPoint pt(origPt.x - itsLeftMarginWidth, origPt.y);
	if (itsDragType != kDragAndDrop || pt == itsPrevPt)
		{
		return;
		}

	TEScrollForDrag(origPt);
	TERefreshCaret(itsDropLoc);

	const CaretLocation caretLoc = CalcCaretLocation(pt);

	CaretLocation dropLoc = CaretLocation(0,0);
	if (!dropOnSelf ||
		caretLoc.charIndex <= itsSelection.first ||
		itsSelection.last+1 <= caretLoc.charIndex)
		{
		dropLoc = caretLoc;		// only drop outside selection
		}
	itsDropLoc = dropLoc;
	TERefreshCaret(itsDropLoc);

	itsPrevPt = pt;
	TEUpdateDisplay();
}

/******************************************************************************
 TEHandleDNDLeave (protected)

 ******************************************************************************/

void
JTextEditor16::TEHandleDNDLeave()
{
	assert( itsDragType == kDragAndDrop );

	TERefreshCaret(itsDropLoc);
	itsDragType = kInvalidDrag;
}

/******************************************************************************
 TEHandleDNDDrop (protected)

	If we are not the source, the derived class has to figure out how to get
	the data from the target.  Once it has the data, it should call
	Paste(text,style).

 ******************************************************************************/

void
JTextEditor16::TEHandleDNDDrop
	(
	const JPoint&	pt,
	const JBoolean	dropOnSelf,
	const JBoolean	dropCopy
	)
{
	assert( itsDragType == kDragAndDrop );

	TEHandleDNDHere(pt, dropOnSelf);
	TERefreshCaret(itsDropLoc);

	if (dropOnSelf)
		{
		itsDragType = kLocalDragAndDrop;
		TEHandleMouseUp(dropCopy);
		}
	else
		{
		SetCaretLocation(itsDropLoc);
		TEPasteDropData();

		const JIndex index = GetInsertionIndex();
		if (index > itsDropLoc.charIndex)
			{
			StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
			SetSelection(itsDropLoc.charIndex, index-1);
			}
		}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 CalcLocalDNDRect (private)

	Returns the rectangle to draw when performing local DND.
	The point should be relative to the left margin.
	The rectangle is relative to the frame.

 ******************************************************************************/

JRect
JTextEditor16::CalcLocalDNDRect
	(
	const JPoint& pt
	)
	const
{
	JRect r(pt, pt);
	r.Shift(itsLeftMarginWidth, 0);
	r.Shrink(-kDraggedOutlineRadius, -kDraggedOutlineRadius);
	return r;
}

/******************************************************************************
 DropSelection (private)

	Called by TEHandleMouseUp() and JTEUndoDrop16.  We modify the undo
	information even though we are private because we implement a single
	user command.

 ******************************************************************************/

void
JTextEditor16::DropSelection
	(
	const JIndex	origDropLoc,
	const JBoolean	dropCopy
	)
{
	assert( !itsSelection.IsEmpty() );

	itsIsDragSourceFlag = kJFalse;		// allow text to be modified

	if (!dropCopy &&
		(origDropLoc == itsSelection.first ||
		 origDropLoc == itsSelection.last + 1))
		{
		return;
		}

	const JSize textLen = itsSelection.GetLength();

	JString16 dropText;
	JRunArray<Font> dropStyles;
	const JBoolean ok = Copy(&dropText, &dropStyles);
	assert( ok );

	JIndex dropLoc       = origDropLoc;
	JTEUndoBase16* newUndo = NULL;
	if (dropCopy)
		{
		SetCaretLocation(dropLoc);
		newUndo = new JTEUndoPaste16(this, textLen);
		}
	else
		{
		JIndex origIndex = itsSelection.first;
		if (dropLoc > itsSelection.first)
			{
			dropLoc -= textLen;
			}
		else
			{
			assert( dropLoc < itsSelection.first );
			origIndex += textLen;
			}

		newUndo = new JTEUndoDrop16(this, origIndex, dropLoc, textLen);

		DeleteText(itsSelection);
		Recalc(itsSelection.first, 1, kJTrue, kJFalse);
		}
	assert( newUndo != NULL );

	itsSelection.SetToNothing();
	const JSize pasteLength = InsertText(dropLoc, dropText, &dropStyles);
	newUndo->SetPasteLength(pasteLength);

	Recalc(dropLoc, textLen, kJFalse, kJFalse);
	StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);
	SetSelection(dropLoc, dropLoc + textLen-1);
	TEScrollToSelection(kJFalse);

	NewUndo(newUndo, kJTrue);
}

/******************************************************************************
 TEGetDoubleClickSelection (private)

	Select the word that was clicked on.  By computing the end of the word
	found by GetWordStart(), we avoid selecting two words when the user
	double clicks on the space between them.

 ******************************************************************************/

void
JTextEditor16::TEGetDoubleClickSelection
	(
	const JIndex	charIndex,
	const JBoolean	partialWord,
	const JBoolean	dragging,
	JIndexRange*	range
	)
{
	JIndex startIndex, endIndex;
	if (partialWord)
		{
		startIndex = GetPartialWordStart(charIndex);
		endIndex   = GetPartialWordEnd(startIndex);
		}
	else
		{
		startIndex = GetWordStart(charIndex);
		endIndex   = GetWordEnd(startIndex);
		}

	// To select the word, the caret location should be inside the word
	// or on the character following the word.

	const JSize bufLength = itsBuffer->GetLength();

	if ((startIndex <= charIndex && charIndex <= endIndex) ||
		(!dragging &&
		 ((charIndex == endIndex+1  && IndexValid(endIndex+1)) ||
		  (charIndex == bufLength+1 && endIndex == bufLength))) )
		{
		range->Set(startIndex, endIndex);
		}

	// Otherwise, we select the character that was clicked on

	else
		{
		range->first = range->last = JMin(charIndex, bufLength);
		if (range->first > 1 &&
			itsBuffer->GetCharacter(range->first)   == '\n' &&
			itsBuffer->GetCharacter(range->first-1) != '\n')
			{
			(range->first)--;
			(range->last)--;
			}
		}
}

/******************************************************************************
 TEHitSamePart

 ******************************************************************************/

JBoolean
JTextEditor16::TEHitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	const CaretLocation loc1 = CalcCaretLocation(pt1);
	const CaretLocation loc2 = CalcCaretLocation(pt2);
	return JConvertToBoolean(loc1 == loc2);
}

/******************************************************************************
 TEHandleKeyPress (protected)

	Returns kJTrue if the key was processed.

 ******************************************************************************/

JBoolean
JTextEditor16::TEHandleKeyPress
	(
	const JCharacter16	origKey,
	const JBoolean		selectText,
	const CaretMotion	motion
	)
{
	assert( itsActiveFlag );
	assert( (!itsSelection.IsEmpty() && itsCaretLoc.charIndex == 0) ||
			( itsSelection.IsEmpty() && itsCaretLoc.charIndex >  0) );

	StValueChanger<JBoolean> _change(itsDoCopySelectFlag, kJTrue);

	if (itsType == kStaticText)
		{
		return kJFalse;
		}
	else if (origKey == kJEscapeKey && itsDragType == kLocalDragAndDrop)
		{
		itsDragType = kInvalidDrag;
		TERefresh();
		return kJTrue;
		}
	else if (TEIsDragging())
		{
		return kJTrue;
		}

	// pre-processing

	JCharacter16 key = origKey;
	if (key == '\r')
		{
		key = '\n';
		}

	const JSize bufLength = itsBuffer->GetLength();

	// We select text by selecting to where the caret ends up.

	const JBoolean isArrowKey = JI2B(
		key == kJLeftArrow || key == kJRightArrow ||
		key == kJUpArrow   || key == kJDownArrow);
	const JBoolean willSelectText = JI2B( selectText && isArrowKey );

	if (willSelectText)
		{
		JBoolean restoreCaretX        = kJTrue;
		const JCoordinate savedCaretX = itsCaretX;

		if (!itsSelection.IsEmpty() && itsSelectionPivot == itsSelection.last+1)
			{
			SetCaretLocation(itsSelection.first);
			}
		else if (!itsSelection.IsEmpty() && itsSelectionPivot == itsSelection.first)
			{
			SetCaretLocation(itsSelection.last+1);
			}
		else if (!itsSelection.IsEmpty())	// SetSelection() was called by outsider
			{
			itsSelectionPivot = itsSelection.first;
			restoreCaretX     = kJFalse;
			SetCaretLocation(itsSelection.last+1);
			}
		else
			{
			itsSelectionPivot = itsCaretLoc.charIndex;
			}

		if (restoreCaretX && (key == kJUpArrow || key == kJDownArrow))
			{
			itsCaretX = savedCaretX;
			}
		}
	else if (itsType == kSelectableText && !isArrowKey)
		{
		return kJFalse;
		}

	// left arrow

	if (key == kJLeftArrow && motion == kMoveByLine)
		{
		GoToBeginningOfLine();
		}

	else if (key == kJLeftArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetWordStart(itsSelection.first));				// works for zero
		}
	else if (key == kJLeftArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetWordStart(itsCaretLoc.charIndex-1));		// works for zero
		}

	else if (key == kJLeftArrow && motion == kMoveByPartialWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetPartialWordStart(itsSelection.first));		// works for zero
		}
	else if (key == kJLeftArrow && motion == kMoveByPartialWord)
		{
		SetCaretLocation(GetPartialWordStart(itsCaretLoc.charIndex-1));	// works for zero
		}

	else if (key == kJLeftArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.first);
		}
	else if (key == kJLeftArrow)
		{
		if (itsCaretLoc.charIndex > 1)
			{
			SetCaretLocation(itsCaretLoc.charIndex-1);
			}
		else
			{
			SetCaretLocation(1);	// scroll to it
			}
		}

	// right arrow

	else if (key == kJRightArrow && motion == kMoveByLine)
		{
		GoToEndOfLine();
		}

	else if (key == kJRightArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetWordEnd(itsSelection.last)+1);
		}
	else if (key == kJRightArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetWordEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJRightArrow && motion == kMoveByPartialWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetPartialWordEnd(itsSelection.last)+1);
		}
	else if (key == kJRightArrow && motion == kMoveByPartialWord)
		{
		SetCaretLocation(GetPartialWordEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJRightArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.last+1);
		}
	else if (key == kJRightArrow)
		{
		if (itsCaretLoc.charIndex <= bufLength)
			{
			SetCaretLocation(itsCaretLoc.charIndex+1);
			}
		else
			{
			SetCaretLocation(bufLength+1);	// scroll to it
			}
		}

	// up arrow

	else if (key == kJUpArrow && motion == kMoveByLine)
		{
		SetCaretLocation(1);
		}

	else if (key == kJUpArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetParagraphStart(itsSelection.first-1));
		}
	else if (key == kJUpArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetParagraphStart(itsCaretLoc.charIndex-1));
		}

	else if (key == kJUpArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.first);
		}
	else if (key == kJUpArrow && itsCaretLoc.charIndex == bufLength+1 &&
			 EndsWithNewline())									// ends with newline
		{
		SetCaretLocation(GetLineStart(itsCaretLoc.lineIndex));
		}
	else if (key == kJUpArrow && itsCaretLoc.lineIndex > 1)
		{
		MoveCaretVert(-1);
		}
	else if (key == kJUpArrow)
		{
		SetCaretLocation(1);
		}

	// down arrow

	else if (key == kJDownArrow && motion == kMoveByLine)
		{
		SetCaretLocation(bufLength+1);
		}

	else if (key == kJDownArrow && motion == kMoveByWord && !itsSelection.IsEmpty())
		{
		SetCaretLocation(GetParagraphEnd(itsSelection.last+1)+1);
		}
	else if (key == kJDownArrow && motion == kMoveByWord)
		{
		SetCaretLocation(GetParagraphEnd(itsCaretLoc.charIndex)+1);
		}

	else if (key == kJDownArrow && !itsSelection.IsEmpty())
		{
		SetCaretLocation(itsSelection.last+1);
		}
	else if (key == kJDownArrow && itsCaretLoc.lineIndex < GetLineCount())
		{
		MoveCaretVert(+1);
		}
	else if (key == kJDownArrow)
		{
		SetCaretLocation(bufLength+1);
		}

	// delete

	else if (key == kJDeleteKey && !itsSelection.IsEmpty())
		{
		DeleteSelection();
		}
	else if (key == kJDeleteKey && itsCaretLoc.charIndex > 1)
		{
		JBoolean isNew;
		JTEUndoTyping16* typingUndo = GetTypingUndo(&isNew);
		typingUndo->HandleDelete();

		const Font f = itsStyles->GetElement(itsCaretLoc.charIndex-1);	// preserve font
		DeleteText(itsCaretLoc.charIndex-1, itsCaretLoc.charIndex-1);
		Recalc(itsCaretLoc.charIndex-1, 1, kJTrue, kJFalse);
		SetCaretLocation(itsCaretLoc.charIndex-1);
		if (itsPasteStyledTextFlag)
			{
			itsInsertionFont = f;
			}

		typingUndo->Activate();		// cancel SetCaretLocation()
		NewUndo(typingUndo, isNew);
		}

	// forward delete

	else if (key == kJForwardDeleteKey && !itsSelection.IsEmpty())
		{
		DeleteSelection();
		}
	else if (key == kJForwardDeleteKey && itsCaretLoc.charIndex <= bufLength)
		{
		JBoolean isNew;
		JTEUndoTyping16* typingUndo = GetTypingUndo(&isNew);
		typingUndo->HandleFwdDelete();

		DeleteText(itsCaretLoc.charIndex, itsCaretLoc.charIndex);
		Recalc(itsCaretLoc, 1, kJTrue, kJFalse);
		SetCaretLocation(itsCaretLoc.charIndex);

		typingUndo->Activate();		// cancel SetCaretLocation()
		NewUndo(typingUndo, isNew);
		}

	// insert character

	else if (itsTabToSpacesFlag && key == '\t')
		{
		InsertSpacesForTab();
		}

	else if (JIsPrint(key) || key == '\n' || key == '\t' || key >= 0x80)
		{
		JBoolean isNew;
		JTEUndoTyping16* typingUndo = GetTypingUndo(&isNew);
		typingUndo->HandleCharacter();

		const JBoolean hadSelection = JNegate( itsSelection.IsEmpty() );
		if (hadSelection)
			{
			itsInsertionFont = itsStyles->GetElement(itsSelection.first);
			DeleteText(itsSelection);
			itsCaretLoc = CalcCaretLocation(itsSelection.first);
			itsSelection.SetToNothing();
			}
		const JCharacter16 s[2]   = { key, '\0' };
		const JSize pasteLength = InsertText(itsCaretLoc.charIndex, s);
		assert( pasteLength == 1 );
		Recalc(itsCaretLoc, 1, hadSelection, kJFalse);
		SetCaretLocation(itsCaretLoc.charIndex+1);

		typingUndo->Activate();		// cancel SetCaretLocation()

		if (key == '\n' && itsAutoIndentFlag)
			{
			AutoIndent(typingUndo);
			}

		NewUndo(typingUndo, isNew);
		}

	// finish selection process

	if (willSelectText)
		{
		const CaretLocation savedCaretLoc = itsCaretLoc;
		if (itsCaretLoc.charIndex < itsSelectionPivot)
			{
			SetSelection(itsCaretLoc.charIndex, itsSelectionPivot-1, kJFalse);
			}
		else if (itsCaretLoc.charIndex > itsSelectionPivot)
			{
			SetSelection(itsSelectionPivot, itsCaretLoc.charIndex-1, kJFalse);
			}

		itsPrevDragType = kSelectDrag;

		// show moving end of selection

		BroadcastCaretMessages(savedCaretLoc, kJTrue);
		}

	// We redraw the display immediately because it is very disconcerting
	// when the display does not instantly show the changes.

	TEUpdateDisplay();
	return kJTrue;
}

/******************************************************************************
 AutoIndent (private)

	Insert the "rest" prefix based on the previous line.  If the previous
	line is nothing but whitespace, clear it.

 ******************************************************************************/

void
JTextEditor16::AutoIndent
	(
	JTEUndoTyping16* typingUndo
	)
{
	assert( itsSelection.IsEmpty() );

	// Move up one line if we are not at the very end of the text.

	JIndex lineIndex = itsCaretLoc.lineIndex;
	if (itsCaretLoc.charIndex <= itsBuffer->GetLength())	// ends with newline
		{
		lineIndex--;
		}

	// Skip past lines that were created by wrapping the text.
	// (This is faster than using GetParagraphStart() because then we would
	//  have to call GetLineForChar(), which is a search.  GetLineStart()
	//  is an array lookup.)

	JIndex firstChar = GetLineStart(lineIndex);
	while (lineIndex > 1 && itsBuffer->GetCharacter(firstChar-1) != '\n')
		{
		lineIndex--;
		firstChar = GetLineStart(lineIndex);
		}

	// Calculate the prefix range for the original line.

	const JIndex lastChar = GetParagraphEnd(firstChar) - 1;
	assert( itsBuffer->GetCharacter(lastChar+1) == '\n' );

	JIndex firstTextChar = firstChar;
	JString16 linePrefix;
	JSize prefixLength;
	JIndex ruleIndex = 0;
	if (lastChar < firstChar ||
		!CRMGetPrefix(&firstTextChar, lastChar,
					  &linePrefix, &prefixLength, &ruleIndex))
		{
		firstTextChar = lastChar+1;
		if (firstTextChar > firstChar)
			{
			linePrefix = itsBuffer->GetSubstring(firstChar, firstTextChar-1);
			}
		}
	else if (CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
		{
		// CRMBuildRestPrefix() will complain if we pass in the entire
		// line, so we give it only the whitespace.

		CRMRuleList* savedRuleList = itsCRMRuleList;
		itsCRMRuleList             = NULL;
		firstTextChar              = firstChar;
		ruleIndex                  = 0;
		CRMGetPrefix(&firstTextChar, lastChar,
					 &linePrefix, &prefixLength, &ruleIndex);
		itsCRMRuleList = savedRuleList;
		}

	// Generate the prefix and include it in the undo object.

	if (firstTextChar > firstChar)
		{
		linePrefix   = CRMBuildRestPrefix(linePrefix, ruleIndex, &prefixLength);
		prefixLength = linePrefix.GetLength();

		typingUndo->HandleCharacters(prefixLength);

		const JSize pasteLength = InsertText(itsCaretLoc.charIndex, linePrefix);
		assert( pasteLength == prefixLength );
		Recalc(itsCaretLoc, prefixLength, kJFalse, kJFalse);

		JIndex newCaretChar = itsCaretLoc.charIndex + prefixLength;

		// check the initial whitespace range

		JIndex lastWSChar = firstChar;
		while (1)
			{
			const JCharacter16 c = itsBuffer->GetCharacter(lastWSChar);
			if ((c != ' ' && c != '\t') || lastWSChar > lastChar)		// can't use isspace() because '\n' stops us
				{
				lastWSChar--;
				break;
				}
			lastWSChar++;
			}

		// if the line is blank, clear it

		if (lastWSChar >= firstChar && itsBuffer->GetCharacter(lastWSChar+1) == '\n')
			{
			typingUndo->HandleAutoIndentDelete(firstChar, lastWSChar);

			DeleteText(firstChar, lastWSChar);
			Recalc(CaretLocation(firstChar, lineIndex), 1, kJTrue, kJFalse);

			newCaretChar -= lastWSChar - firstChar + 1;
			}

		SetCaretLocation(newCaretChar);

		// re-activate undo after SetCaretLocation()

		typingUndo->Activate();
		}
}

/******************************************************************************
 InsertSpacesForTab (private)

	Insert spaces to use up the same amount of space that a tab would use.

 ******************************************************************************/

void
JTextEditor16::InsertSpacesForTab()
{
	const JSize charIndex  = GetInsertionIndex();
	JSize lineIndex        = GetLineForChar(charIndex);
	JSize column           = charIndex - GetLineStart(lineIndex);

	if (charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())									// ends with newline
		{
		lineIndex++;
		column = 0;
		}

	const JSize spaceCount = CRMGetTabWidth(column);

	JString16 space;
	for (JIndex i=1; i<=spaceCount; i++)
		{
		space.AppendCharacter(' ');
		}
	Paste(space);
}

/******************************************************************************
 ContainsIllegalChars (static)

	Returns kJTrue if the given text contains characters that we will not
	accept:  00-08, 0B, 0E-1F, 7F

	We accept form feed (0C) because PrintPlainText() converts it to space.

	We accept all characters above 0x7F because they provide useful
	(though hopelessly system dependent) extensions to the character set.

 ******************************************************************************/

#ifdef _TODO
static const JRegex illegalCharRegex("[\x00-\x08\x0B\x0E-\x1F\x7F]+", 11, kJFalse);
#endif

JBoolean
JTextEditor16::ContainsIllegalChars
	(
	const JCharacter16*	text,
	const JSize			length
	)
{
#ifdef _TODO
	return illegalCharRegex.MatchWithin(text, JIndexRange(1,length));
#else
	return kJFalse;
#endif
}

/******************************************************************************
 RemoveIllegalChars (static)

	Returns kJTrue if we had to remove any characters that
	ContainsIllegalChars() would flag.

	If the user cancels, we toss the entire string and return kJTrue.

	style can be NULL or empty.

 ******************************************************************************/

JBoolean
JTextEditor16::RemoveIllegalChars
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
{
#ifdef _TODO
	assert( style == NULL || style->IsEmpty() ||
			style->GetElementCount() == text->GetLength() );

	const JSize origTextLength = text->GetLength();
	const JSize saveBlockSize  = text->GetBlockSize();
	text->SetBlockSize(origTextLength);			// avoid realloc

	JLatentPG pg(100);
	pg.FixedLengthProcessBeginning(origTextLength,
				"Removing illegal characters...", kJTrue, kJFalse);

	JIndexRange remainder(1, text->GetLength());
	JIndexRange illegal;
	while (illegalCharRegex.MatchWithin(*text, remainder, &illegal))
		{
		text->RemoveSubstring(illegal);
		if (style != NULL && !style->IsEmpty())
			{
			style->RemoveNextElements(illegal.first, illegal.GetLength());
			}

		const JSize pgDelta = (illegal.first - remainder.first) + illegal.GetLength();
		remainder.Set(illegal.first, text->GetLength());

		if (!pg.IncrementProgress(pgDelta))
			{
			text->RemoveSubstring(remainder);
			if (style != NULL)
				{
				style->RemoveNextElements(remainder.first, remainder.GetLength());
				}
			break;
			}
		}

	pg.ProcessFinished();

	text->SetBlockSize(saveBlockSize);
	return JConvertToBoolean( text->GetLength() < origTextLength );
#else
	return kJFalse;
#endif
}

/******************************************************************************
 ReplaceIllegalChars (static)

	Returns kJTrue if we had to replace any characters that
	ContainsIllegalChars() would flag.

	If the user cancels, we toss the entire string and return kJTrue.

	style can be NULL or empty.

 ******************************************************************************/

JBoolean
JTextEditor16::ReplaceIllegalChars
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
{
#ifdef _TODO
	assert( style == NULL || style->IsEmpty() ||
			style->GetElementCount() == text->GetLength() );

	const JSize origTextLength = text->GetLength();
	const JSize saveBlockSize  = text->GetBlockSize();
	text->SetBlockSize(origTextLength);			// avoid realloc

	JLatentPG pg(100);
	pg.FixedLengthProcessBeginning(origTextLength,
				"Replacing illegal characters...", kJTrue, kJFalse);

	JIndexRange remainder(1, text->GetLength());
	JIndexRange illegal;
	while (illegalCharRegex.MatchWithin(*text, remainder, &illegal))
		{
		JCharacter16 temp[] = {'?', 0};
		for(JIndex idx = illegal.first; idx <= illegal.last; idx++)
			text->ReplaceSubstring(idx, idx, temp);

		const JSize pgDelta = (illegal.first - remainder.first) + illegal.GetLength();
		remainder.Set(illegal.last + 1, text->GetLength());

		if (!pg.IncrementProgress(pgDelta))
			{
			text->RemoveSubstring(remainder);
			if (style != NULL)
				{
				style->RemoveNextElements(remainder.first, remainder.GetLength());
				}
			break;
			}
		}

	pg.ProcessFinished();

	text->SetBlockSize(saveBlockSize);
	return JConvertToBoolean( text->GetLength() < origTextLength );
#else
	return kJFalse;
#endif
}

/******************************************************************************
 GetWordStart

	Return the index of the first character of the word at the given location.
	This function is required to work for charIndex == 0.

 ******************************************************************************/

JIndex
JTextEditor16::GetWordStart
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty() || charIndex <= 1)
		{
		return 1;
		}

	JIndex i = JMin(charIndex, itsBuffer->GetLength());
	if (!IsCharacterInWord(*itsBuffer, i))
		{
		i--;
		while (i >= 1 && !IsCharacterInWord(*itsBuffer, i))
			{
			i--;
			}
		}

	while (i >= 1 && IsCharacterInWord(*itsBuffer, i))
		{
		i--;
		}
	return i+1;
}

/******************************************************************************
 GetWordEnd

	Return the index of the last character of the word at the given location.
	This function is required to work for charIndex > buffer length.

 ******************************************************************************/

JIndex
JTextEditor16::GetWordEnd
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty())
		{
		return 1;
		}

	const JSize bufLen = itsBuffer->GetLength();
	if (charIndex >= bufLen)
		{
		return bufLen;
		}

	JIndex i = charIndex;
	if (!IsCharacterInWord(*itsBuffer, i))
		{
		i++;
		while (i <= bufLen && !IsCharacterInWord(*itsBuffer, i))
			{
			i++;
			}
		}

	while (i <= bufLen && IsCharacterInWord(GetText16(), i))
		{
		i++;
		}
	return i-1;
}

/******************************************************************************
 GetCharacterInWordFunction

 ******************************************************************************/

JBoolean (*
JTextEditor16::GetCharacterInWordFunction()
)(const JString16&, const JIndex)
{
	return itsCharInWordFn;
}

/******************************************************************************
 SetCharacterInWordFunction

	Set the function that determines if a character is part of a word.
	The default is JIsAlnum(), which uses [A-Za-z0-9].

 ******************************************************************************/

void
JTextEditor16::SetCharacterInWordFunction
	(
	JBoolean (*f)(const JString16&, const JIndex)
	)
{
	assert( f != NULL );
	itsCharInWordFn = f;
}

/******************************************************************************
 GetI18NCharacterInWordFunction (static)

 ******************************************************************************/

JBoolean (*
JTextEditor16::GetI18NCharacterInWordFunction()
)(const JCharacter16)
{
	return itsI18NCharInWordFn;
}

/******************************************************************************
 SetI18NCharacterInWordFunction (static)

	Set the function that determines if a character is part of a word.

 ******************************************************************************/

void
JTextEditor16::SetI18NCharacterInWordFunction
	(
	JBoolean (*f)(const JCharacter16)
	)
{
	itsI18NCharInWordFn = f;
}

/******************************************************************************
 IsCharacterInWord (protected)

	Returns kJTrue if the given character should be considered part of a word.

 ******************************************************************************/

JBoolean
JTextEditor16::IsCharacterInWord
	(
	const JString16&	text,
	const JIndex	charIndex
	)
	const
{
	return JI2B(VIsCharacterInWord(text, charIndex) ||
				(itsI18NCharInWordFn != NULL &&
				 itsI18NCharInWordFn(text.GetCharacter(charIndex))));
}

/******************************************************************************
 VIsCharacterInWord (virtual protected)

	Allows derived classes to safely override the IsCharacterInWord
	definition so they can use their instance data.

 ******************************************************************************/

JBoolean
JTextEditor16::VIsCharacterInWord
	(
	const JString16&	text,
	const JIndex	charIndex
	)
	const
{
	return itsCharInWordFn(text, charIndex);
}

/******************************************************************************
 DefaultIsCharacterInWord (static private)

	Returns kJTrue if the given character should be considered part of a word.
	The definition is alphanumeric or apostrophe or underscore.

	This is not a virtual function because one needs to be able to set it
	for simple objects like input fields.

 ******************************************************************************/

JBoolean
JTextEditor16::DefaultIsCharacterInWord
	(
	const JString16&	text,
	const JIndex	charIndex
	)
{
	const JCharacter16 c = text.GetCharacter(charIndex);
	return JI2B( JIsAlnum(c) || c == '\'' || c == '_' );
}

/******************************************************************************
 GetPartialWordStart

	Return the index of the first character of the partial word at the given
	location.  This function is required to work for charIndex == 0.

	Example:  get_word Get142TheWordABCGood ABCDe
	          ^   ^    ^  ^  ^  ^   ^  ^    ^  ^
 ******************************************************************************/

JIndex
JTextEditor16::GetPartialWordStart
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty() || charIndex <= 1)
		{
		return 1;
		}

	JIndex i = JMin(charIndex, itsBuffer->GetLength());
	if (!JIsAlnum(itsBuffer->GetCharacter(i)))
		{
		i--;
		while (i >= 1 && !JIsAlnum(itsBuffer->GetCharacter(i)))
			{
			i--;
			}
		}

	if (i < 2)
		{
		return 1;
		}

	JCharacter16 prev     = itsBuffer->GetCharacter(i);
	JBoolean foundLower = JI2B(JIsLower(prev));
	while (i > 1)
		{
		const JCharacter16 c = itsBuffer->GetCharacter(i-1);
		foundLower         = JI2B(foundLower || JIsLower(c));
		if (!JIsAlnum(c) ||
			(JIsUpper(prev) && JIsLower(c)) ||
			(JIsUpper(prev) && JIsUpper(c) && foundLower) ||
			(JIsAlpha(prev) && isdigit(c)) ||
			(isdigit(prev)  && JIsAlpha(c)))
			{
			break;
			}

		prev = c;
		i--;
		}

	return i;
}

/******************************************************************************
 GetPartialWordEnd

	Return the index of the last character of the partial word at the given
	location.  This function is required to work for charIndex > buffer length.

	Example:  get_word Get142TheWordABCGood
	            ^    ^   ^  ^  ^   ^  ^   ^
 ******************************************************************************/

JIndex
JTextEditor16::GetPartialWordEnd
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty())
		{
		return 1;
		}

	const JSize bufLen = itsBuffer->GetLength();
	if (charIndex >= bufLen)
		{
		return bufLen;
		}

	JIndex i = charIndex;
	if (!JIsAlnum(itsBuffer->GetCharacter(i)))
		{
		i++;
		while (i <= bufLen && !JIsAlnum(itsBuffer->GetCharacter(i)))
			{
			i++;
			}
		}

	if (i >= bufLen)
		{
		return bufLen;
		}

	JCharacter16 prev = itsBuffer->GetCharacter(i);
	while (i < bufLen)
		{
		const JCharacter16 c = itsBuffer->GetCharacter(i+1);
		if (!JIsAlnum(c) ||
			(JIsLower(prev) && JIsUpper(c)) ||
			(JIsAlpha(prev) && isdigit(c)) ||
			(isdigit(prev)  && JIsAlpha(c)) ||
			(i < bufLen-1 && JIsUpper(prev) && JIsUpper(c) &&
			 JIsLower(itsBuffer->GetCharacter(i+2))))
			{
			break;
			}

		prev = c;
		i++;
		}

	return i;
}

/******************************************************************************
 GetParagraphStart

	Return the index of the first character in the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex == 0.

 ******************************************************************************/

JIndex
JTextEditor16::GetParagraphStart
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty() || charIndex <= 1)
		{
		return 1;
		}

	JIndex i = JMin(charIndex, itsBuffer->GetLength());
	while (i > 1 && itsBuffer->GetCharacter(i-1) != '\n')
		{
		i--;
		}

	return i;
}

/******************************************************************************
 GetParagraphEnd

	Return the index of the newline that ends the paragraph that contains
	the character at the given location.  This function is required to work
	for charIndex > buffer length.

 ******************************************************************************/

JIndex
JTextEditor16::GetParagraphEnd
	(
	const JIndex charIndex
	)
	const
{
	if (itsBuffer->IsEmpty())
		{
		return 1;
		}

	const JSize bufLen = itsBuffer->GetLength();
	if (charIndex >= bufLen)
		{
		return bufLen;
		}

	JIndex i = charIndex;
	while (i < bufLen && itsBuffer->GetCharacter(i) != '\n')
		{
		i++;
		}

	return i;
}

/******************************************************************************
 SetCaretLocation

	Move the caret to the specified point.

 ******************************************************************************/

void
JTextEditor16::SetCaretLocation
	(
	const JIndex origCharIndex
	)
{
	JIndex charIndex = JMax(origCharIndex, JIndex(1));
	charIndex        = JMin(charIndex, itsBuffer->GetLength()+1);

	SetCaretLocation( CalcCaretLocation(charIndex) );
}

// private

void
JTextEditor16::SetCaretLocation
	(
	const CaretLocation& caretLoc
	)
{
	if (itsIsDragSourceFlag)
		{
		return;
		}

	DeactivateCurrentUndo();

	const JBoolean hadSelection      = JNegate( itsSelection.IsEmpty() );
	const CaretLocation origCaretLoc = itsCaretLoc;

	if (hadSelection)
		{
		TERefreshLines(GetLineForChar(itsSelection.first),
					   GetLineForChar(itsSelection.last));
		}

	itsSelection.SetToNothing();
	itsCaretLoc = caretLoc;
	itsCaretX   = GetCharLeft(itsCaretLoc);

	if (hadSelection || origCaretLoc != itsCaretLoc || !itsPasteStyledTextFlag)
		{
		itsInsertionFont = CalcInsertionFont(itsCaretLoc.charIndex);
		}

	if (!TEScrollTo(itsCaretLoc))
		{
		TERefreshCaret(origCaretLoc);
		TERefreshCaret(itsCaretLoc);
		}

	BroadcastCaretMessages(itsCaretLoc,
		JI2B(hadSelection || origCaretLoc.lineIndex != itsCaretLoc.lineIndex));

	TECaretShouldBlink(kJTrue);
}

/******************************************************************************
 BroadcastCaretMessages (private)

 ******************************************************************************/

void
JTextEditor16::BroadcastCaretMessages
	(
	const CaretLocation&	caretLoc,
	const JBoolean			lineChanged
	)
{
	if (!BroadcastCaretPosChanged(caretLoc) && lineChanged)
		{
		JIndex line = caretLoc.lineIndex;
		if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
			EndsWithNewline())									// ends with newline
			{
			line++;
			}
		Broadcast(CaretLineChanged(line));
		}
}

JBoolean
JTextEditor16::BroadcastCaretPosChanged
	(
	const CaretLocation& caretLoc
	)
{
	if (itsBcastLocChangedFlag)
		{
		JIndex line = caretLoc.lineIndex;
		JIndex col  = GetColumnForChar(caretLoc);
		if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
			EndsWithNewline())									// ends with newline
			{
			line++;
			col = 1;
			}
		Broadcast(CaretLocationChanged(line, col));
		}
	return itsBcastLocChangedFlag;
}

/******************************************************************************
 GetColumnForChar

	Returns the column that the specified character is in.  If the caret is
	at the far left, it is column 1.

	Given that this is only useful with monospace fonts, the CRM tab width
	is used to calculate the column when tabs are encountered, by calling
	CRMGetTabWidth().

 ******************************************************************************/

JIndex
JTextEditor16::GetColumnForChar
	(
	const JIndex charIndex
	)
	const
{
	return GetColumnForChar(CalcCaretLocation(charIndex));
}

// protected

JIndex
JTextEditor16::GetColumnForChar
	(
	const CaretLocation& caretLoc
	)
	const
{
	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())									// ends with newline
		{
		return 1;
		}
	else
		{
		JIndex charIndex = GetLineStart(caretLoc.lineIndex);
		JIndex col       = 1;
		while (charIndex < caretLoc.charIndex && charIndex <= itsBuffer->GetLength())
			{
			if (itsBuffer->GetCharacter(charIndex) == '\t')
				{
				col += CRMGetTabWidth(col-1);
				}
			else
				{
				col++;
				}
			charIndex++;
			}

		return col;
		}
}

/******************************************************************************
 GoToColumn

 ******************************************************************************/

void
JTextEditor16::GoToColumn
	(
	const JIndex lineIndex,
	const JIndex columnIndex
	)
{
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc(0, lineIndex);
	if (lineIndex > lineCount && EndsWithNewline())		// ends with newline
		{
		caretLoc.charIndex = itsBuffer->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		caretLoc.charIndex    = GetLineStart(lineIndex);
		const JIndex endIndex = GetLineEnd(lineIndex);
		JIndex col            = 1;
		while (col < columnIndex && caretLoc.charIndex < endIndex)
			{
			if (itsBuffer->GetCharacter(caretLoc.charIndex) == '\t')
				{
				col += CRMGetTabWidth(col-1);
				}
			else
				{
				col++;
				}
			caretLoc.charIndex++;
			}
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToLine

 ******************************************************************************/

void
JTextEditor16::GoToLine
	(
	const JIndex lineIndex
	)
{
	JIndex trueIndex      = lineIndex;
	const JSize lineCount = GetLineCount();

	CaretLocation caretLoc;
	if (trueIndex > lineCount && EndsWithNewline())		// ends with newline
		{
		caretLoc.charIndex = itsBuffer->GetLength() + 1;
		caretLoc.lineIndex = lineCount;
		}
	else
		{
		if (trueIndex == 0)
			{
			trueIndex = 1;
			}
		else if (trueIndex > lineCount)
			{
			trueIndex = lineCount;
			}
		caretLoc.charIndex = GetLineStart(trueIndex);
		caretLoc.lineIndex = trueIndex;
		}

	TEScrollToRect(CalcCaretRect(caretLoc), kJTrue);
	if (IsReadOnly())
		{
		SetSelection(GetLineStart(caretLoc.lineIndex),
					 GetLineEnd(caretLoc.lineIndex));
		}
	else
		{
		SetCaretLocation(caretLoc);
		}
}

/******************************************************************************
 SelectLine

 ******************************************************************************/

void
JTextEditor16::SelectLine
	(
	const JIndex lineIndex
	)
{
	GoToLine(lineIndex);
	if (itsSelection.IsEmpty())
		{
		SetSelection(GetLineStart(itsCaretLoc.lineIndex),
					 GetLineEnd(itsCaretLoc.lineIndex));
		}
}

/******************************************************************************
 GoToBeginningOfLine

 ******************************************************************************/

void
JTextEditor16::GoToBeginningOfLine()
{
	CaretLocation caretLoc;
	if (!itsSelection.IsEmpty())
		{
		caretLoc = CalcCaretLocation(itsSelection.first);
		}
	else
		{
		caretLoc = itsCaretLoc;
		}

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())										// ends with newline
		{
		// set current value so we scroll to it
		}
	else if (itsMoveToFrontOfTextFlag && !itsBuffer->IsEmpty())
		{
		const JIndex firstChar = GetLineStart(caretLoc.lineIndex);
		JIndex lastChar        = GetLineEnd(caretLoc.lineIndex);
		if (itsBuffer->GetCharacter(lastChar) == '\n')
			{
			lastChar--;
			}

		JIndex firstTextChar = firstChar;
		JString16 linePrefix;
		JSize prefixLength;
		JIndex ruleIndex = 0;
		if (lastChar < firstChar ||
			!CRMGetPrefix(&firstTextChar, lastChar,
						  &linePrefix, &prefixLength, &ruleIndex) ||
			CRMLineMatchesRest(JIndexRange(firstChar, lastChar)))
			{
			firstTextChar = lastChar+1;
			}

		caretLoc.charIndex =
			caretLoc.charIndex <= firstTextChar ? firstChar : firstTextChar;
		}
	else
		{
		caretLoc.charIndex = GetLineStart(caretLoc.lineIndex);
		}

	SetCaretLocation(caretLoc);
}

/******************************************************************************
 GoToEndOfLine

 ******************************************************************************/

void
JTextEditor16::GoToEndOfLine()
{
	if (!itsSelection.IsEmpty())
		{
		const JIndex lineIndex = GetLineForChar(itsSelection.last);
		const JIndex charIndex = GetLineEnd(lineIndex);
		if (isspace(itsBuffer->GetCharacter(charIndex)))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
	else if (itsCaretLoc.charIndex == itsBuffer->GetLength()+1 &&
			 EndsWithNewline())										// ends with newline
		{
		SetCaretLocation(itsCaretLoc);	// scroll to it
		}
	else if (!itsBuffer->IsEmpty())
		{
		const JIndex charIndex = GetLineEnd(itsCaretLoc.lineIndex);
		const JCharacter16 c     = itsBuffer->GetCharacter(charIndex);
		if (isspace(c) && (charIndex < itsBuffer->GetLength() || c == '\n'))
			{
			SetCaretLocation(charIndex);
			}
		else
			{
			SetCaretLocation(charIndex+1);
			}
		}
}

/******************************************************************************
 CRLineIndexToVisualLineIndex

	Returns the index that the user sees that corresponds to the given
	index that would be seen without word wrap.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor16::CRLineIndexToVisualLineIndex
	(
	const JIndex crLineIndex
	)
	const
{
	if (itsBreakCROnlyFlag)
		{
		return JMin(crLineIndex, GetLineCount());
		}
	else
		{
		JIndex charIndex = 0;
		for (JIndex i=1; i<=crLineIndex; i++)
			{
			const JIndex newCharIndex = GetParagraphEnd(charIndex+1);
			if (newCharIndex == charIndex)	// end of text
				{
				break;
				}
			charIndex = newCharIndex;
			}
		return GetLineForChar(GetParagraphStart(charIndex));
		}
}

/******************************************************************************
 VisualLineIndexToCRLineIndex

	Returns the index that would be seen without word wrap that corresponds
	to the given index that the user sees.

	This function is required to work for arbitrarily large, invalid line
	indices.

 ******************************************************************************/

JIndex
JTextEditor16::VisualLineIndexToCRLineIndex
	(
	const JIndex origVisualLineIndex
	)
	const
{
	const JIndex visualLineIndex = JMin(origVisualLineIndex, GetLineCount());
	if (itsBreakCROnlyFlag)
		{
		return visualLineIndex;
		}
	else
		{
		JSize crLineIndex         = 1;
		const JIndex endCharIndex = GetLineStart(visualLineIndex);
		for (JIndex i=1; i<endCharIndex; i++)
			{
			if (itsBuffer->GetCharacter(i) == '\n')
				{
				crLineIndex++;
				}
			}
		return crLineIndex;
		}
}

/******************************************************************************
 TEScrollTo (private)

	Scroll to the specified insertion point.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor16::TEScrollTo
	(
	const CaretLocation& caretLoc
	)
{
	return TEScrollToRect(CalcCaretRect(caretLoc), kJFalse);
}

/******************************************************************************
 CalcCaretRect (private)

	Calculate the rectangle enclosing the caret when it is at the given
	location.  We set the width of the rectangle to be the left margin
	width so that the entire left margin will scroll into view when the
	caret is at the far left.

 ******************************************************************************/

JRect
JTextEditor16::CalcCaretRect
	(
	const CaretLocation& caretLoc
	)
	const
{
	JRect r;

	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())								// ends with newline
		{
		r.top    = GetLineBottom(caretLoc.lineIndex) + 1;
		r.bottom = r.top + GetEWNHeight();
		r.left   = 0;
		r.right  = itsLeftMarginWidth + kRightMarginWidth;
		}
	else
		{
		r.top    = GetLineTop(caretLoc.lineIndex);
		r.bottom = r.top + GetLineHeight(caretLoc.lineIndex);
		r.left   = GetCharLeft(caretLoc);
		r.right  = r.left + itsLeftMarginWidth + kRightMarginWidth;
		}

	return r;
}

/******************************************************************************
 TEScrollToSelection (private)

	Scroll to make the selection or caret visible.  Returns kJTrue if scrolling
	was actually necessary.

 ******************************************************************************/

JBoolean
JTextEditor16::TEScrollToSelection
	(
	const JBoolean centerInDisplay
	)
{
	if (!itsSelection.IsEmpty())
		{
		const CaretLocation start = CalcCaretLocation(itsSelection.first);
		const CaretLocation end   = CalcCaretLocation(itsSelection.last);

		JRect r;
		r.top    = GetLineTop(start.lineIndex);
		r.bottom = GetLineBottom(end.lineIndex);
		if (start.lineIndex == end.lineIndex)
			{
			r.left  = GetCharLeft(start) + itsLeftMarginWidth;
			r.right = GetCharRight(end) + itsLeftMarginWidth;
			}
		else
			{
			r.left  = 0;
			r.right = itsLeftMarginWidth + itsWidth + kRightMarginWidth;
			}

		return TEScrollToRect(r, centerInDisplay);
		}
	else
		{
		return TEScrollToRect(CalcCaretRect(itsCaretLoc), centerInDisplay);
		}
}

/******************************************************************************
 TEUpdateDisplay (virtual protected)

	Called to update the window after mouse and keyboard events.
	Not pure virtual because some systems do this automatically.

 ******************************************************************************/

void
JTextEditor16::TEUpdateDisplay()
{
}

/******************************************************************************
 TERefreshLines (protected)

	Redraw the specified range of lines.

 ******************************************************************************/

void
JTextEditor16::TERefreshLines
	(
	const JIndex firstLine,
	const JIndex lastLine
	)
{
	JRect r(GetLineTop(firstLine), 0, GetLineBottom(lastLine)+1,
			itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) + kRightMarginWidth);
	if (lastLine == GetLineCount() && EndsWithNewline())	// ends with newline
		{
		r.bottom += GetEWNHeight();
		}
	TERefreshRect(r);
}

/******************************************************************************
 TERefreshCaret (private)

 ******************************************************************************/

void
JTextEditor16::TERefreshCaret
	(
	const CaretLocation& caretLoc
	)
{
	if (caretLoc.charIndex == itsBuffer->GetLength()+1 &&
		EndsWithNewline())								// ends with newline
		{
		const JRect r(itsHeight - GetEWNHeight(), 0, itsHeight,
					  itsLeftMarginWidth + JMax(itsGUIWidth, itsWidth) +
					  kRightMarginWidth);
		TERefreshRect(r);
		}
	else if (itsLineStarts->IndexValid(caretLoc.lineIndex))
		{
		TERefreshLines(caretLoc.lineIndex, caretLoc.lineIndex);
		}
}

/******************************************************************************
 MoveCaretVert (protected)

	Moves the caret up or down the specified number of lines.

 ******************************************************************************/

void
JTextEditor16::MoveCaretVert
	(
	const JInteger deltaLines
	)
{
	assert( itsSelection.IsEmpty() );

	const JSize lineCount  = GetLineCount();
	const JIndex lineIndex = itsCaretLoc.lineIndex;

	JIndex newLineIndex = 0;
	if (deltaLines > 0 && lineIndex + deltaLines <= lineCount)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines > 0)
		{
		newLineIndex = lineCount;
		}
	else if (deltaLines < 0 && lineIndex > (JSize) -deltaLines)
		{
		newLineIndex = lineIndex + deltaLines;
		}
	else if (deltaLines < 0)
		{
		newLineIndex = 1;
		}

	if (newLineIndex > 0)
		{
		const JCoordinate saveX = itsCaretX;

		const JPoint pt(itsCaretX, GetLineTop(newLineIndex)+1);
		SetCaretLocation(CalcCaretLocation(pt));

		itsCaretX = saveX;
		}
}

/******************************************************************************
 GetLineForChar

	Returns the line that the specified character is on.  Since the
	array is sorted, we can use a binary search.  We can use kAnyMatch
	because we know the values are unique.

 ******************************************************************************/

JIndex
JTextEditor16::GetLineForChar
	(
	const JIndex charIndex
	)
	const
{
	if (charIndex == 0)
		{
		return 1;
		}

	JBoolean found;
	JIndex lineIndex =
		itsLineStarts->SearchSorted1(charIndex, JOrderedSetT::kAnyMatch, &found);
	if (!found)
		{
		lineIndex--;	// wants to insert -after- the value
		}
	return lineIndex;
}

/******************************************************************************
 CalcInsertionFont (private)

	Returns the font to use when inserting at the specified point.

 ******************************************************************************/

JTextEditor16::Font
JTextEditor16::CalcInsertionFont
	(
	const JIndex charIndex
	)
	const
{
	if (1 < charIndex && charIndex <= itsBuffer->GetLength() &&
		itsBuffer->GetCharacter(charIndex-1) == '\n')
		{
		return itsStyles->GetElement(charIndex);
		}
	else if (charIndex > 1)
		{
		return itsStyles->GetElement(charIndex-1);
		}
	else if (!itsStyles->IsEmpty())
		{
		return itsStyles->GetElement(1);
		}
	else
		{
		return itsDefFont;
		}
}

/******************************************************************************
 GetLineEnd

	Returns the last character on the specified line.

 ******************************************************************************/

JIndex
JTextEditor16::GetLineEnd
	(
	const JIndex lineIndex
	)
	const
{
	if (lineIndex < GetLineCount())
		{
		return GetLineStart(lineIndex+1) - 1;
		}
	else
		{
		return itsPrevBufLength;	// consistent with rest of output from Recalc()
		}
}

/******************************************************************************
 GetLineTop

	Returns the top of the specified line.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetLineTop
	(
	const JIndex endLineIndex
	)
	const
{
	return (endLineIndex == 1 ? 0 :
			itsLineGeom->SumElements(1, endLineIndex-1, GetLineHeight));
}

/******************************************************************************
 GetCharLeft (private)

	Returns the starting x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetCharLeft
	(
	const CaretLocation& charLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(charLoc.lineIndex);

	JCoordinate x = 0;
	if (charLoc.charIndex > firstChar)
		{
		x = GetStringWidth(firstChar, charLoc.charIndex-1);
		}

	// Handle alignment
	if (!itsStyles->IsEmpty())
	{
		const Font af = itsStyles->GetElement(firstChar);
		if (af.align != kAlignLeft)
		{
			JCoordinate drawwidth = TEGetBoundsWidth();
			JCoordinate linewidth = GetLineWidth(charLoc.lineIndex);
			if (af.align == kAlignCenter)
				x += (drawwidth - linewidth) / 2;
			else if (af.align == kAlignRight)
				x += drawwidth - linewidth;
		}
	}

	return x;
}

/******************************************************************************
 GetCharWidth (private)

	Returns the width of the specified character.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetCharWidth
	(
	const CaretLocation& charLoc
	)
	const
{
	const JCharacter16 c = itsBuffer->GetCharacter(charLoc.charIndex);
	if (c != '\t')
		{
		const Font f = itsStyles->GetElement(charLoc.charIndex);
		return itsFontMgr->GetCharWidth16(f.id, f.size, f.style, c);
		}
	else
		{
		return GetTabWidth(charLoc.charIndex, GetCharLeft(charLoc));
		}
}

/******************************************************************************
 GetStringWidth (private)

	Returns the width of the specified character range.

	The second version updates *runIndex,*firstInRun so that they
	are correct for endIndex+1.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetStringWidth
	(
	const JIndex startIndex,
	const JIndex endIndex
	)
	const
{
	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(startIndex, &runIndex, &firstInRun);
	assert( found );

	return GetStringWidth(startIndex, endIndex, &runIndex, &firstInRun);
}

JCoordinate
JTextEditor16::GetStringWidth
	(
	const JIndex	origStartIndex,
	const JIndex	endIndex,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	assert( IndexValid(origStartIndex) );
	assert( IndexValid(endIndex) );

	// preWidth stores the width of the characters preceding origStartIndex
	// on the line containing origStartIndex.  We calculate this -once- when
	// it is first needed.  (i.e. when we hit the first tab character)

	JCoordinate width    = 0;
	JCoordinate preWidth = -1;

	JIndex startIndex = origStartIndex;
	while (startIndex <= endIndex)
		{
		JSize runLength        = itsStyles->GetRunLength(*runIndex);
		const JSize trueRunEnd = *firstInRun + runLength-1;

		runLength -= startIndex - *firstInRun;
		if (startIndex + runLength-1 > endIndex)
			{
			runLength = endIndex - startIndex + 1;
			}

		const Font& f = itsStyles->GetRunDataRef(*runIndex);

		// If there is a tab in the string, we step up to it and take care of
		// the rest in the next iteration.

		JIndex tabIndex;
		if (LocateTab(startIndex, startIndex + runLength-1, &tabIndex))
			{
			runLength = tabIndex - startIndex;
			}
		else
			{
			tabIndex = 0;
			}

		if (runLength > 0)
			{
			width += itsFontMgr->GetStringWidth16(f.id, f.size, f.style,
												itsBuffer->GetCString() + startIndex-1,
												runLength);
			}
		if (tabIndex > 0)
			{
			if (preWidth < 0)
				{
				// recursion: max depth 1
				preWidth = GetCharLeft(CalcCaretLocation(origStartIndex));
				assert( preWidth >= 0 );
				}
			width += GetTabWidth(startIndex + runLength, preWidth + width);
			runLength++;
			}

		startIndex += runLength;
		if (startIndex > trueRunEnd)	// move to next style run
			{
			*firstInRun = startIndex;
			(*runIndex)++;
			}
		}

	return width;
}

/******************************************************************************
 LocateTab (private)

	Returns the index of the first tab character, starting from startIndex.
	If no tab is found, returns kJFalse, and *tabIndex = 0.

 ******************************************************************************/

JBoolean
JTextEditor16::LocateTab
	(
	const JIndex	startIndex,
	const JIndex	endIndex,
	JIndex*			tabIndex
	)
	const
{
	for (JIndex i=startIndex; i<=endIndex; i++)
		{
		if (itsBuffer->GetCharacter(i) == '\t')
			{
			*tabIndex = i;
			return kJTrue;
			}
		}

	*tabIndex = 0;
	return kJFalse;
}

/******************************************************************************
 GetTabWidth (virtual protected)

	Given the index of the tab character (charIndex) and the horizontal
	position on the line (in pixels) where the tab character starts (x),
	return the width of the tab character.

	This default implementation rounds the location up to the nearest
	multiple of itsDefTabWidth.

 ******************************************************************************/

JCoordinate
JTextEditor16::GetTabWidth
	(
	const JIndex		charIndex,
	const JCoordinate	x
	)
	const
{
	const JCoordinate xnew = ((x / itsDefTabWidth) + 1) * itsDefTabWidth;
	return (xnew - x);
}

/******************************************************************************
 RecalcAll (protected)

 ******************************************************************************/

void
JTextEditor16::RecalcAll
	(
	const JBoolean needAdjustStyles
	)
{
	if (itsBreakCROnlyFlag)
		{
		itsWidth = 0;
		}
	itsMaxWordWidth = 0;

	itsLineStarts->RemoveAll();
	itsLineStarts->AppendElement(1);

	itsLineWidths->RemoveAll();
	itsLineWidths->AppendElement(0);

	itsLineGeom->RemoveAll();

	Recalc(CaretLocation(1,1), itsBuffer->GetLength(), kJFalse,
		   kJTrue, needAdjustStyles);
}

/******************************************************************************
 Recalc (private)

	Recalculate layout, starting from caretLoc and continuing at least
	through charCount characters.

	*** We assume that nothing above caretLoc.lineIndex needs to be recalculated.
		We also assume that the first line to recalculate already exists.

 ******************************************************************************/

void
JTextEditor16::Recalc
	(
	const CaretLocation&	origCaretLoc,
	const JSize				origMinCharCount,
	const JBoolean			deletion,
	const JBoolean			needCaretBcast,
	const JBoolean			needAdjustStyles
	)
{
	JCoordinate maxLineWidth = 0;
	if (itsBreakCROnlyFlag && GetLineCount() == 1)
		{
		itsWidth = 0;
		}

	JCoordinate origY = -1;
	if (!itsLineGeom->IsEmpty())
		{
		origY = GetLineTop(origCaretLoc.lineIndex);
		}

	const JSize bufLength  = itsBuffer->GetLength();

	CaretLocation caretLoc = origCaretLoc;
	JSize minCharCount     = JMax((JSize) 1, origMinCharCount);
	JIndexRange redrawRange;
	if (bufLength > 0 && needAdjustStyles)
		{
		JIndexRange recalcRange(caretLoc.charIndex, caretLoc.charIndex + minCharCount - 1);
		redrawRange = recalcRange;
		AdjustStylesBeforeRecalc(*itsBuffer, itsStyles, &recalcRange, &redrawRange, deletion);
		caretLoc.charIndex = recalcRange.first;
		minCharCount       = recalcRange.GetLength();

		if (recalcRange.Contains(redrawRange))
			{
			redrawRange.SetToNothing();
			}

		assert( caretLoc.charIndex <= origCaretLoc.charIndex );
		assert( caretLoc.charIndex + minCharCount - 1 >=
				origCaretLoc.charIndex + origMinCharCount - 1 );
		assert( itsBuffer->GetLength() == itsStyles->GetElementCount() );

		if (caretLoc.charIndex < origCaretLoc.charIndex)
			{
			while (caretLoc.charIndex < itsLineStarts->GetElement(caretLoc.lineIndex))
				{
				(caretLoc.lineIndex)--;
				}
			}
		}

	if (caretLoc.charIndex == bufLength+1)
		{
		(caretLoc.charIndex)--;
		caretLoc.lineIndex = GetLineForChar(caretLoc.charIndex);
		}

	JIndex firstLineIndex, lastLineIndex;
	if (bufLength > 0)
		{
		Recalc1(bufLength, caretLoc, minCharCount, &maxLineWidth,
				&firstLineIndex, &lastLineIndex);

		if (!redrawRange.IsEmpty())
			{
			firstLineIndex = JMin(firstLineIndex, GetLineForChar(redrawRange.first));
			lastLineIndex  = JMax(lastLineIndex,  GetLineForChar(redrawRange.last));
			}
		}
	else
		{
		itsLineStarts->RemoveAll();
		itsLineStarts->AppendElement(1);

		itsLineWidths->RemoveAll();
		itsLineWidths->AppendElement(0);

		itsLineGeom->RemoveAll();

		const Font f = CalcInsertionFont(1);
		JCoordinate ascent,descent;
		const JCoordinate h =
			itsFontMgr->GetLineHeight(f.id, f.size, f.style, &ascent, &descent);
		itsLineGeom->AppendElement(LineGeometry(h, ascent));

		firstLineIndex = lastLineIndex = 1;
		}

	// If the caret is visible, recalculate the line index.

	JBoolean lineChanged = kJFalse;
	if (itsSelection.IsEmpty())
		{
		const JIndex origLine = itsCaretLoc.lineIndex;
		itsCaretLoc.lineIndex = GetLineForChar(itsCaretLoc.charIndex);
		lineChanged           = JI2B(itsCaretLoc.lineIndex != origLine);
		}

	// If we only break at newlines, we control our width.

	if (itsBreakCROnlyFlag && maxLineWidth > itsWidth)
		{
		itsWidth = maxLineWidth;
		}

	// If all the lines are the same height, set the scrolling step size to that.

	if (itsLineGeom->GetRunCount() == 1 && !itsIsPrintingFlag)
		{
		TESetVertScrollStep(GetLineHeight(1));
		}
	else if (!itsIsPrintingFlag)
		{
		TESetVertScrollStep(
			itsFontMgr->GetLineHeight(itsDefFont.id, itsDefFont.size, itsDefFont.style));
		}

	// recalculate the height

	const JSize lineCount = GetLineCount();
	JCoordinate newHeight = GetLineTop(lineCount) + GetLineHeight(lineCount);
	if (EndsWithNewline())								// ends with newline
		{
		newHeight += GetEWNHeight();
		}

	JBoolean needRefreshLines = kJTrue;
	if (!itsIsPrintingFlag && newHeight != itsHeight)
		{
		TERefresh();	// redraw everything if the line heights changed
		needRefreshLines = kJFalse;
		}

	itsHeight = newHeight;

	// notify the derived class of our new size

	if (!itsIsPrintingFlag)
		{
		TESetGUIBounds(itsLeftMarginWidth + itsWidth + kRightMarginWidth, itsHeight, origY);
		}

	// save buffer length for next time

	itsPrevBufLength = bufLength;

	// show the changes

	if (!itsIsPrintingFlag && needRefreshLines)
		{
		TERefreshLines(firstLineIndex, lastLineIndex);
		}

	// This has to be last so everything is correctly set.

	if (needCaretBcast && itsSelection.IsEmpty())
		{
		BroadcastCaretMessages(itsCaretLoc, lineChanged);
		}
	else if (needCaretBcast)
		{
		BroadcastCaretMessages(CalcCaretLocation(itsSelection.first), kJTrue);
		}

	// avoid ludicrous width

	if (TEWidthIsBeyondDisplayCapacity(itsWidth))
		{
		SetBreakCROnly(kJFalse);
		}
}

/******************************************************************************
 Recalc1 (private)

	Subroutine called by Recalc().

	(firstLineIndex, lastLineIndex) gives the range of lines that had
	to be recalculated.

 ******************************************************************************/

void
JTextEditor16::Recalc1
	(
	const JSize				bufLength,
	const CaretLocation&	caretLoc,
	const JSize				origMinCharCount,
	JCoordinate*			maxLineWidth,
	JIndex*					firstLineIndex,
	JIndex*					lastLineIndex
	)
{
	JIndex lineIndex = caretLoc.lineIndex;
	if (!itsBreakCROnlyFlag && lineIndex > 1 &&
		caretLoc.charIndex <= bufLength &&
		NoPrevWhitespaceOnLine(*itsBuffer, caretLoc))
		{
		// If we start in the first word on the line, it
		// might actually belong on the previous line.

		lineIndex--;
		}

	JIndex firstChar   = GetLineStart(lineIndex);
	JSize minCharCount = origMinCharCount + (caretLoc.charIndex - firstChar);
	*firstLineIndex    = lineIndex;

	JIndex runIndex, firstInRun;
	const JBoolean found = itsStyles->FindRun(firstChar, &runIndex, &firstInRun);
	assert( found );

	JSize totalCharCount = 0;
	*maxLineWidth        = itsWidth;
	while (1)
		{
		JCoordinate lineWidth;
		const JSize charCount = RecalcLine(bufLength, firstChar, lineIndex, &lineWidth,
										   &runIndex, &firstInRun);
		itsLineWidths->SetElement(lineIndex, lineWidth);

		totalCharCount += charCount;
		if (*maxLineWidth < lineWidth)
			{
			*maxLineWidth = lineWidth;
			}
		const JIndex endChar = firstChar + charCount-1;
		assert( endChar <= bufLength );

		// remove line starts that are further from the end than the new one
		// (we use (bufLength - endChar) so subtraction won't produce negative numbers)

		while (lineIndex < GetLineCount() &&
			   (itsPrevBufLength+1) - GetLineStart(lineIndex+1) >
					bufLength - endChar)
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineWidths->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}

		// check if we are done

		if (endChar >= bufLength)
			{
			// We reached the end of the text.

			break;
			}
		else if (totalCharCount >= minCharCount &&
				 lineIndex < GetLineCount() &&
				 itsPrevBufLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			// The rest of the line starts merely shift.

			const JSize lineCount = itsLineStarts->GetElementCount();
			assert( lineCount > lineIndex );
			const long delta = endChar+1 - GetLineStart(lineIndex+1);
			if (delta != 0)
				{
				const JIndex* lineStart = itsLineStarts->GetCArray();
				for (JIndex i=lineIndex+1; i<=lineCount; i++)
					{
					itsLineStarts->SetElement(i, lineStart[i-1] + delta);
					}
				}
			break;
			}

		// insert the new line start

		lineIndex++;
		firstChar += charCount;

		itsLineStarts->InsertElementAtIndex(lineIndex, firstChar);
		itsLineWidths->InsertElementAtIndex(lineIndex, 0);
		itsLineGeom->InsertElementAtIndex(lineIndex, LineGeometry());

		// This catches the case when the new and old line starts
		// are equally far from the end, but we haven't recalculated
		// far enough yet, so the above breakout code didn't trigger.

		if (lineIndex < GetLineCount() &&
			   itsPrevBufLength - GetLineStart(lineIndex+1) ==
					bufLength - (endChar+1))
			{
			itsLineStarts->RemoveElement(lineIndex+1);
			itsLineWidths->RemoveElement(lineIndex+1);
			itsLineGeom->RemoveElement(lineIndex+1);
			}
		}

	*lastLineIndex = lineIndex;
}

/******************************************************************************
 RecalcLine (private)

	Recalculate the line starting with firstChar.  Returns the number
	of characters on the line.  Sets the appropriate values in itsLineGeom.
	Sets *lineWidth to the width of the line in pixels.

	If insertLine is kJTrue, then this line is new, so we insert a new
	element into itsLineGeom.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the line.

 ******************************************************************************/

JSize
JTextEditor16::RecalcLine
	(
	const JSize		bufLength,
	const JIndex	firstCharIndex,
	const JIndex	lineIndex,
	JCoordinate*	lineWidth,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
{
	JSize charCount = 0;
	*lineWidth      = 0;

	JIndex gswRunIndex   = *runIndex;
	JIndex gswFirstInRun = *firstInRun;

	if (itsBreakCROnlyFlag)
		{
		JIndex endIndex = firstCharIndex;
		JCharacter16 temp[] = {'\n', 0};
		if (!itsBuffer->LocateNextSubstring(temp, &endIndex))
			{
			endIndex = itsBuffer->GetLength();
			}
		charCount  = endIndex - firstCharIndex + 1;
		*lineWidth = GetStringWidth(firstCharIndex, endIndex,
									&gswRunIndex, &gswFirstInRun);
		}

	else
		{
		// include leading whitespace

		JBoolean endOfLine;
		charCount = IncludeWhitespaceOnLine(bufLength, firstCharIndex,
											lineWidth, &endOfLine,
											&gswRunIndex, &gswFirstInRun);
		JIndex charIndex = firstCharIndex + charCount;

		// Add words until we hit the right margin, a newline,
		// or the end of the buffer.

		while (charIndex <= bufLength && !endOfLine)
			{
			// get the next word

			JIndex prevIndex = charIndex;
			if (!LocateNextWhitespace(bufLength, &charIndex))
				{
				charIndex = bufLength+1;
				}

			// check if the word fits on this line

			JCoordinate dw =
				GetStringWidth(prevIndex, charIndex-1, &gswRunIndex, &gswFirstInRun);
			if (itsMaxWordWidth < dw)
				{
				itsMaxWordWidth = dw;
				}
			if (*lineWidth + dw > itsWidth)
				{
				if (prevIndex != firstCharIndex)
					{
					// this word goes on the next line

					charIndex = prevIndex;
					}
				else
					{
					// put as much of this word as possible on the line

					assert( *lineWidth == 0 && charCount == 0 );
					charCount = GetSubwordForLine(bufLength, lineIndex,
												  firstCharIndex, lineWidth);
					}
				break;
				}

			// put the word on this line

			*lineWidth += dw;
			charCount  += charIndex - prevIndex;

			// include the whitespace after the word

			JSize wsCount =
				IncludeWhitespaceOnLine(bufLength, charIndex, lineWidth, &endOfLine,
										&gswRunIndex, &gswFirstInRun);
			charIndex += wsCount;
			charCount += wsCount;
			}
		}

	// update geometry for this line

	const JSize runCount = itsStyles->GetRunCount();
	const JSize lastChar = firstCharIndex + charCount-1;

	JCoordinate maxAscent=0, maxDescent=0;
	while (*runIndex <= runCount)
		{
		const Font& f = itsStyles->GetRunDataRef(*runIndex);
		JCoordinate ascent, descent;
		itsFontMgr->GetLineHeight(f.id, f.size, f.style, &ascent, &descent);

		if (ascent > maxAscent)
			{
			maxAscent = ascent;
			}
		if (descent > maxDescent)
			{
			maxDescent = descent;
			}

		const JIndex firstInNextRun = *firstInRun + itsStyles->GetRunLength(*runIndex);
		if (firstInNextRun <= lastChar+1)
			{
			(*runIndex)++;
			*firstInRun = firstInNextRun;
			}
		if (firstInNextRun > lastChar)
			{
			break;
			}
		}

	const LineGeometry geom(maxAscent+maxDescent, maxAscent);
	if (lineIndex <= itsLineGeom->GetElementCount())
		{
		itsLineGeom->SetElement(lineIndex, geom);
		}
	else
		{
		itsLineGeom->AppendElement(geom);
		}

	// return number of characters on line

	return charCount;
}

/******************************************************************************
 IncludeWhitespaceOnLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of consecutive whitespace
	characters encountered.  Increments *lineWidth with the width of this
	whitespace.  If we encounter a newline, we stop beyond it and set
	*endOfLine to kJTrue.

	Updates *runIndex,*firstInRun so that they are correct for the character
	beyond the end of the whitespace.

 ******************************************************************************/

JSize
JTextEditor16::IncludeWhitespaceOnLine
	(
	const JSize		bufLength,
	const JIndex	origStartIndex,
	JCoordinate*	lineWidth,
	JBoolean*		endOfLine,
	JIndex*			runIndex,
	JIndex*			firstInRun
	)
	const
{
	*endOfLine = kJFalse;

	JIndex startIndex = origStartIndex;
	JIndex endIndex   = startIndex;
	JSize wsCount     = 0;

	while (endIndex <= bufLength)
		{
		const JCharacter16 c = itsBuffer->GetCharacter(endIndex);
		if (!isspace(c))
			{
			break;
			}

		wsCount++;
		if (c == '\t')
			{
			if (endIndex > startIndex)
				{
				*lineWidth += GetStringWidth(startIndex, endIndex-1,
											 runIndex, firstInRun);
				}
			*lineWidth += GetTabWidth(endIndex, *lineWidth);
			startIndex  = endIndex+1;

			// update *runIndex,*firstInRun after passing tab character

			const JSize runLength = itsStyles->GetRunLength(*runIndex);
			if (startIndex > *firstInRun + runLength-1)
				{
				*firstInRun += runLength;
				(*runIndex)++;
				}

			// tab characters can wrap to the next line

			if (!itsBreakCROnlyFlag && *lineWidth > itsWidth)
				{
				*endOfLine = kJTrue;
				break;
				}
			}

		endIndex++;
		if (c == '\n')
			{
			*endOfLine = kJTrue;
			break;
			}
		}

	if (endIndex > startIndex)
		{
		*lineWidth += GetStringWidth(startIndex, endIndex-1, runIndex, firstInRun);
		}

	return wsCount;
}

/******************************************************************************
 LocateNextWhitespace (private)

	*** Only for use by RecalcLine()

	Find the next whitespace character, starting from the given index.
	Returns kJFalse if it doesn't find any.

	*** If we only break at newlines, only newlines are considered to be
		whitespace.

 ******************************************************************************/

JBoolean
JTextEditor16::LocateNextWhitespace
	(
	const JSize	bufLength,
	JIndex*		startIndex
	)
	const
{
	for (JIndex i=*startIndex; i<=bufLength; i++)
		{
		const JCharacter16 c = itsBuffer->GetCharacter(i);
		if ((!itsBreakCROnlyFlag && isspace(c)) ||
			( itsBreakCROnlyFlag && c == '\n'))
			{
			*startIndex = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetSubwordForLine (private)

	*** Only for use by RecalcLine()

	Starting with the given index, return the number of characters that
	will fit on a line.  Sets *lineWidth to the width of the line in pixels.
	We always put at least one character on the line.

 ******************************************************************************/

JSize
JTextEditor16::GetSubwordForLine
	(
	const JSize		bufLength,
	const JIndex	lineIndex,
	const JIndex	startIndex,
	JCoordinate*	lineWidth
	)
	const
{
	*lineWidth = 0;

	JIndex endIndex = 0;
	CaretLocation caretLoc(0, lineIndex);
	for (JIndex i=startIndex; i<=bufLength; i++)
		{
		caretLoc.charIndex   = i;
		const JCoordinate dw = GetCharWidth(caretLoc);
		if (i > startIndex && *lineWidth + dw > itsWidth)
			{
			endIndex = i-1;
			break;
			}
		else
			{
			*lineWidth += dw;
			}
		}
	if (endIndex == 0)
		{
		endIndex = bufLength;
		}

	return (endIndex - startIndex + 1);
}

/******************************************************************************
 NoPrevWhitespaceOnLine (private)

	Returns kJTrue if there is no whitespace between startIndex-1 and
	the first character on the line.

	Called by Recalc1() to decide whether or not to start on the
	previous line.

 ******************************************************************************/

JBoolean
JTextEditor16::NoPrevWhitespaceOnLine
	(
	const JCharacter16*		str,
	const CaretLocation&	startLoc
	)
	const
{
	const JIndex firstChar = GetLineStart(startLoc.lineIndex);
	for (JIndex i = startLoc.charIndex - 1; i>firstChar; i--)
		{
		if (isspace(str[i-1]))
			{
			return kJFalse;
			}
		}

	return kJTrue;	// we hit the start of the string
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Called before Recalc() begins its work.  Derived classes can override
	this to adjust the style of the affected range of text.  range is an
	in/out variable because the changes usually slop out beyond the initial
	range.

	*** The endpoints of the ranges are only allowed to move outward.

	*** The contents of *styles can change, but the length must remain
		the same.  *range must be expanded to include all the changed
		elements.

 ******************************************************************************/

void
JTextEditor16::AdjustStylesBeforeRecalc
	(
	const JString16&		buffer,
	JRunArray<Font>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
}

/******************************************************************************
 CalcCaretLocation (protected)

	Return the closest insertion point.  If the line ends with a space,
	the last possible insertion point is in front of this space

 ******************************************************************************/

JTextEditor16::CaretLocation
JTextEditor16::CalcCaretLocation
	(
	const JPoint& pt
	)
	const
{
	const JSize bufLength = itsBuffer->GetLength();
	if (bufLength == 0)
		{
		return CaretLocation(1,1);
		}
	else if (pt.y >= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the line that was clicked on

	JCoordinate lineTop;
	const JIndex lineIndex = CalcLineIndex(pt.y, &lineTop);
	if (EndsWithNewline() &&									// ends with newline
		itsHeight - GetEWNHeight() < pt.y && pt.y <= itsHeight)
		{
		return CaretLocation(bufLength+1, GetLineCount());
		}

	// find the closest insertion point

	JIndex charIndex = 0;
	{
	const JIndex lineStart = GetLineStart(lineIndex);

	JIndex lineEnd = GetLineEnd(lineIndex);
	if ((lineEnd < bufLength || EndsWithNewline()) &&	// ends with newline
		isspace(itsBuffer->GetCharacter(lineEnd)))
		{
		lineEnd--;
		}

	JCoordinate prevD = pt.x;
	if (prevD <= 0)
		{
		charIndex = lineStart;
		}
	else
		{
		// Offset to take into account alignment
		JCoordinate x = GetCharLeft(CaretLocation(lineStart, lineIndex));
		prevD = pt.x - x;
		for (JIndex i=lineStart; i<=lineEnd; i++)
			{
			x += GetCharWidth(CaretLocation(i, lineIndex));
			const JCoordinate d = pt.x - x;
			if (d == 0)
				{
				charIndex = i+1;
				break;
				}
			else if (d < 0 && prevD <= -d)
				{
				charIndex = i;
				break;
				}
			else if (d < 0 && prevD > -d)
				{
				charIndex = i+1;
				break;
				}
			prevD = d;
			}

		if (charIndex == 0)
			{
			charIndex = lineEnd+1;
			}
		}
	}

	return CaretLocation(charIndex, lineIndex);
}

/******************************************************************************
 CalcLineIndex (private)

 ******************************************************************************/

JIndex
JTextEditor16::CalcLineIndex
	(
	const JCoordinate	y,
	JCoordinate*		lineTop
	)
	const
{
	if (y < 0)
		{
		*lineTop = 0;
		return 1;
		}
	else
		{
		JIndex lineIndex;
		itsLineGeom->FindPositiveSum(y, 1, &lineIndex, lineTop, GetLineHeight);
		return lineIndex;
		}
}

/******************************************************************************
 GetLineHeight (static private)

	Used by GetLineTop() and CalcLineIndex().

 ******************************************************************************/

JInteger
JTextEditor16::GetLineHeight
	(
	const LineGeometry& data
	)
{
	return data.height;
}

/******************************************************************************
 GetEWNHeight (private)

	Not inline to avoid including JFontManager.h in header file

 ******************************************************************************/

JCoordinate
JTextEditor16::GetEWNHeight()
	const
{
	const Font f = CalcInsertionFont(itsBuffer->GetLength() + 1);
	return itsFontMgr->GetLineHeight(f.id, f.size, f.style);
}
