/******************************************************************************
 JTextEditor16.h

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTextEditor16
#define _H_JTextEditor16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JString16.h>
#include <JBroadcaster.h>
#include <JFontStyle.h>
#include <JColorList.h>
#include <JRect.h>
#include <JRunArray.h>
#include <JPtrArray-JString.h>

class JRegex;
class JPainter;
class JPagePrinter;
class JFontManager;
class JColormap;
class JTEUndoBase16;
class JTEUndoTextBase16;
class JTEUndoTyping16;
class JTEUndoDrop16;
class JTEUndoStyle16;
class JTEUndoTabShift16;

class JTextEditor16 : virtual public JBroadcaster
{
	friend class JTEUndoTextBase16;
	friend class JTEUndoDrop16;
	friend class JTEUndoStyle16;

public:

	enum Type
	{
		kFullEditor,
		kSelectableText,
		kStaticText
	};

	enum PlainTextFormat
	{
		kUNIXText,
		kMacintoshText,
		kDOSText
	};

	enum AlignmentType
	{
		kAlignLeft,
		kAlignCenter,
		kAlignRight
	};

	enum CmdIndex
	{
		kSeparatorCmd = 1,	// always disabled, use if menu separator is separate item

		kUndoCmd,
		kRedoCmd,
		kCutCmd,
		kCopyCmd,
		kPasteCmd,
		kDeleteSelCmd,
		kSelectAllCmd,
		kCleanRightMarginCmd,
		kCoerceRightMarginCmd,
		kShiftSelLeftCmd,
		kShiftSelRightCmd,
		kForceShiftSelLeftCmd,
		kToggleReadOnlyCmd,

		kFindDialogCmd,
		kFindPreviousCmd,
		kFindNextCmd,
		kEnterSearchTextCmd,
		kEnterReplaceTextCmd,
		kFindSelectionBackwardCmd,
		kFindSelectionForwardCmd,
		kFindClipboardBackwardCmd,
		kFindClipboardForwardCmd,
		kReplaceSelectionCmd,
		kReplaceFindPrevCmd,
		kReplaceFindNextCmd,
		kReplaceAllBackwardCmd,
		kReplaceAllForwardCmd,
		kReplaceAllInSelectionCmd,

		kCmdCount = kReplaceAllInSelectionCmd
	};

	struct Font
	{
		JFontID		id;
		JSize		size;
		JFontStyle	style;
		AlignmentType	align;

		Font()
			:
			id(0), size(kJDefaultFontSize), style(), align(kAlignLeft)
		{ };

		Font(const JFontID anID, const JSize aSize,
			 const JFontStyle& aStyle, const AlignmentType align = kAlignLeft)
			:
			id(anID), size(aSize), style(aStyle), align(kAlignLeft)
		{ };
	};

	enum CaretMotion
	{
		kMoveByCharacter,
		kMoveByPartialWord,
		kMoveByWord,
		kMoveByLine
	};

	enum
	{
		kMinLeftMarginWidth = 2
	};

	struct CRMRule
	{
		JRegex*	first;		// match and replace
		JRegex* rest;		// match

		CRMRule()
			:
			first(NULL), rest(NULL)
		{ };

		CRMRule(JRegex* f, JRegex* r)
			:
			first(f), rest(r)
		{ };

		CRMRule(const JCharacter16* firstPattern, const JCharacter16* restPattern,
				const JCharacter16* replacePattern)
			:
			first(CreateFirst(firstPattern, replacePattern)),
			rest(CreateRest(restPattern))
		{ };

		static JRegex*	CreateFirst(const JCharacter16* pattern,
									const JCharacter16* replacePattern);
		static JRegex*	CreateRest(const JCharacter16* pattern);
	};

	class CRMRuleList : public JArray<CRMRule>
	{
	public:

		CRMRuleList(const JSize aBlockSize = 5)
			:
			JArray<CRMRule>(aBlockSize)
		{ };

		CRMRuleList(const CRMRuleList& source);

		void	DeleteAll();
	};

	class FontMatch
	{
	public:

		virtual ~FontMatch();

		virtual JBoolean	Match(const JCharacter*, const JSize, const JFontStyle&) const = 0;
	};

public:

	JTextEditor16(const Type type, const JBoolean breakCROnly,
				const JBoolean pasteStyledText, const JBoolean useInternalClipboard,
				const JFontManager* fontManager, JColormap* colormap,
				const JColorIndex caretColor, const JColorIndex selectionColor,
				const JColorIndex outlineColor, const JColorIndex dragColor,
				const JColorIndex wsColor, const JCoordinate width);
	JTextEditor16(const JTextEditor16& source);

	virtual ~JTextEditor16();

	Type		GetType() const;
	JBoolean	IsReadOnly() const;
	JBoolean	WillPasteStyledText() const;

	JBoolean	WillBreakCROnly() const;
	void		SetBreakCROnly(const JBoolean breakCROnly);

	JBoolean				IsEmpty() const;
	JBoolean				EndsWithNewline() const;
	JSize					GetTextLength() const;
	const JString16&			GetText16() const;
	const JRunArray<Font>&	GetStyles() const;
	JBoolean				SetText16(const JString16& text,
									const JRunArray<Font>* style = NULL);
	JBoolean				SetText16(const JCharacter16* text,
									const JRunArray<Font>* style = NULL);

	JBoolean	ReadPlainText(const JCharacter* fileName, PlainTextFormat* format,
							  const JBoolean acceptBinaryFile = kJTrue);
	void		WritePlainText(const JCharacter* fileName, const PlainTextFormat format) const;
	void		WritePlainText(ostream& output, const PlainTextFormat format) const;

	JBoolean	ReadUNIXManOutput(istream& input, const JBoolean allowCancel = kJFalse);

	JBoolean	ReadPrivateFormat(istream& input);
	void		WritePrivateFormat(ostream& output) const;

	static void	WritePrivateFormat(ostream& output, const JFontManager* fontMgr,
								   const JColormap* colormap, const JFileVersion vers,
								   const JString16& text, const JRunArray<Font>& style,
								   const JIndex startIndex, const JIndex endIndex);

	JBoolean	SearchForward(const JCharacter16* searchStr,
							  const JBoolean caseSensitive, const JBoolean entireWord,
							  const JBoolean wrapSearch, JBoolean* wrapped);
	JBoolean	SearchBackward(const JCharacter16* searchStr,
							   const JBoolean caseSensitive, const JBoolean entireWord,
							   const JBoolean wrapSearch, JBoolean* wrapped);
	JBoolean	SelectionMatches(const JCharacter16* searchStr,
								 const JBoolean caseSensitive, const JBoolean entireWord);

	JBoolean	ReplaceAllForward(const JCharacter16* searchStr,
								  const JBoolean searchIsRegex, const JBoolean caseSensitive,
								  const JBoolean entireWord, const JBoolean wrapSearch,
								  const JCharacter16* replaceStr,
								  const JIndexRange& searchRange = JIndexRange());
	JBoolean	ReplaceAllBackward(const JCharacter16* searchStr,
								   const JBoolean searchIsRegex, const JBoolean caseSensitive,
								   const JBoolean entireWord, const JBoolean wrapSearch,
								   const JCharacter16* replaceStr);

	JBoolean	SearchForward(const FontMatch& match,
							  const JBoolean wrapSearch, JBoolean* wrapped);
	JBoolean	SearchBackward(const FontMatch& match,
							   const JBoolean wrapSearch, JBoolean* wrapped);

	virtual JBoolean	TEHasSearchText() const = 0;

	const JFontManager*	TEGetFontManager() const;
	JColormap*			TEGetColormap() const;

	const JCharacter*	GetFontName(const JIndex charIndex) const;
	JSize				GetFontSize(const JIndex charIndex) const;
	JFontStyle			GetFontStyle(const JIndex charIndex) const;
	void				GetFont(const JIndex charIndex, JString* name,
								JSize* size, JFontStyle* style) const;

	JBoolean	SetFontName(const JIndex startIndex, const JIndex endIndex,
							const JCharacter* name, const JBoolean clearUndo);
	JBoolean	SetFontSize(const JIndex startIndex, const JIndex endIndex,
							const JSize size, const JBoolean clearUndo);
	JBoolean	SetFontBold(const JIndex startIndex, const JIndex endIndex,
							const JBoolean bold, const JBoolean clearUndo);
	JBoolean	SetFontItalic(const JIndex startIndex, const JIndex endIndex,
							  const JBoolean italic, const JBoolean clearUndo);
	JBoolean	SetFontUnderline(const JIndex startIndex, const JIndex endIndex,
								 const JSize count, const JBoolean clearUndo);
	JBoolean	SetFontStrike(const JIndex startIndex, const JIndex endIndex,
							  const JBoolean strike, const JBoolean clearUndo);
	JBoolean	SetFontColor(const JIndex startIndex, const JIndex endIndex,
							 const JColorIndex color, const JBoolean clearUndo);
	JBoolean	SetFontStyle(const JIndex startIndex, const JIndex endIndex,
							 const JFontStyle& style, const JBoolean clearUndo);
	JBoolean	SetFontAlign(const JIndex startIndex, const JIndex endIndex,
							 const AlignmentType align, const JBoolean clearUndo);
	void		SetFont(const JIndex startIndex,const JIndex endIndex,
						const JCharacter* name, const JSize size,
						const JFontStyle& style, const AlignmentType align,
						const JBoolean clearUndo);

	JBoolean	GetCaretLocation(JIndex* charIndex) const;
	void		SetCaretLocation(const JIndex charIndex);

	JBoolean	HasSelection() const;
	JBoolean	GetSelection(JIndex* startIndex, JIndex* endIndex) const;
	JBoolean	GetSelection(JIndexRange* range) const;
	JBoolean	GetSelection(JString16* text) const;
	JBoolean	GetSelection(JString16* text, JRunArray<Font>* style) const;
	void		SetSelection(const JIndex startIndex, const JIndex endIndex,
							 const JBoolean needCaretBcast = kJTrue);
	void		SetSelection(const JIndexRange& range,
							 const JBoolean needCaretBcast = kJTrue);
	JBoolean	TEScrollToSelection(const JBoolean centerInDisplay);
	void		TEGetDoubleClickSelection(const JIndex charIndex,
										  const JBoolean partialWord,
										  const JBoolean dragging, JIndexRange* range);

	JBoolean	GetTextRange(JString16* text, JRunArray<Font>* style, const JIndex range_start, const JIndex range_end) const;

	JIndex		GetInsertionIndex() const;
	JBoolean	IsEntireWord(const JIndex startIndex, const JIndex endIndex) const;
	JBoolean	IsEntireWord(const JIndexRange& range) const;
	JIndex		GetWordStart(const JIndex charIndex) const;
	JIndex		GetWordEnd(const JIndex charIndex) const;
	JIndex		GetPartialWordStart(const JIndex charIndex) const;
	JIndex		GetPartialWordEnd(const JIndex charIndex) const;
	JIndex		GetParagraphStart(const JIndex charIndex) const;
	JIndex		GetParagraphEnd(const JIndex charIndex) const;
	void		GoToBeginningOfLine();
	void		GoToEndOfLine();

	const JCharacter*	GetCurrentFontName() const;
	JSize				GetCurrentFontSize() const;
	JFontStyle			GetCurrentFontStyle() const;
	AlignmentType		GetCurrentFontAlign() const;
	void				GetCurrentFont(JString* name, JSize* size,
									   JFontStyle* style, AlignmentType* align) const;
	void				GetCurrentFont(JFontID* id, JSize* size,
									   JFontStyle* style, AlignmentType* align) const;

	void	SetCurrentFontName(const JCharacter* name);
	void	SetCurrentFontSize(const JSize size);
	void	SetCurrentFontBold(const JBoolean bold);
	void	SetCurrentFontItalic(const JBoolean italic);
	void	SetCurrentFontUnderline(const JSize count);
	void	SetCurrentFontUnderlineType(const JFontStyle::EUnderlineType type);
	void	SetCurrentFontStrike(const JBoolean strike);
	void	SetCurrentFontColor(const JColorIndex color);
	void	SetCurrentFontBackColor(const JColorIndex color);
	void	SetCurrentFontStyle(const JFontStyle& style);
	void	SetCurrentFontAlign(const AlignmentType align);
	void	SetCurrentFont(const JCharacter* name, const JSize size,
						   const JFontStyle& style = JFontStyle(),
						   const AlignmentType align = kAlignLeft);
	void	SetCurrentFont(const JFontID id, const JSize size,
						   const JFontStyle& style = JFontStyle(),
						   const AlignmentType align = kAlignLeft);

	void	SetRangeFontName(const JIndex startIndex, const JIndex endIndex, const JCharacter* name);
	void	SetRangeFontSize(const JIndex startIndex, const JIndex endIndex, const JSize size);
	void	SetRangeFontBold(const JIndex startIndex, const JIndex endIndex, const JBoolean bold);
	void	SetRangeFontItalic(const JIndex startIndex, const JIndex endIndex, const JBoolean italic);
	void	SetRangeFontUnderline(const JIndex startIndex, const JIndex endIndex, const JSize count);
	void	SetRangeFontUnderlineType(const JIndex startIndex, const JIndex endIndex, const JFontStyle::EUnderlineType type);
	void	SetRangeFontStrike(const JIndex startIndex, const JIndex endIndex, const JBoolean strike);
	void	SetRangeFontColor(const JIndex startIndex, const JIndex endIndex, const JColorIndex color);
	void	SetRangeFontColor(const JIndex startIndex, const JIndex endIndex, const JRGB& color);
	void	SetRangeFontBackColor(const JIndex startIndex, const JIndex endIndex, const JColorIndex color);
	void	SetRangeFontBackColor(const JIndex startIndex, const JIndex endIndex, const JRGB& color);
	void	SetRangeFontStyle(const JIndex startIndex, const JIndex endIndex, const JFontStyle& style);
	void	SetRangeFontAlign(const JIndex startIndex, const JIndex endIndex, const AlignmentType align);
	void	SetRangeFont(const JIndex startIndex, const JIndex endIndex, const JCharacter* name,
							const JSize size, const JFontStyle& style, const AlignmentType align = kAlignLeft);
	void	SetRangeFont(const JIndex startIndex, const JIndex endIndex, const JFontID id,
							const JSize size, const JFontStyle& style, const AlignmentType align = kAlignLeft);

	const JCharacter*	GetDefaultFontName() const;
	JSize				GetDefaultFontSize() const;
	const JFontStyle&	GetDefaultFontStyle() const;
	AlignmentType		GetDefaultFontAlign() const;
	void				GetDefaultFont(JString* name, JSize* size,
									   JFontStyle* style, AlignmentType* align) const;
	void				GetDefaultFont(JFontID* id, JSize* size,
									   JFontStyle* style, AlignmentType* align) const;

	void	SetDefaultFontName(const JCharacter* name);
	void	SetDefaultFontSize(const JSize size);
	void	SetDefaultFontStyle(const JFontStyle& style);
	void	SetDefaultFontAlign(const AlignmentType align);
	void	SetDefaultFont(const JCharacter* name, const JSize size,
						   const JFontStyle& style = JFontStyle(),
						   const AlignmentType align = kAlignLeft);
	void	SetDefaultFont(const JFontID id, const JSize size,
						   const JFontStyle& style = JFontStyle(),
						   const AlignmentType align = kAlignLeft);

	JCoordinate	GetDefaultTabWidth() const;
	void		SetDefaultTabWidth(const JCoordinate width);
	void		TabSelectionLeft(const JSize tabCount = 1,
								 const JBoolean force = kJFalse);
	void		TabSelectionRight(const JSize tabCount = 1);

	JBoolean	WillAutoIndent() const;
	void		ShouldAutoIndent(const JBoolean indent);

	JBoolean	TabInsertsSpaces() const;
	void		TabShouldInsertSpaces(const JBoolean spaces);

	JBoolean	WillShowWhitespace() const;
	void		ShouldShowWhitespace(const JBoolean show);

	JBoolean	WillMoveToFrontOfText() const;
	void		ShouldMoveToFrontOfText(const JBoolean moveToFront);

	JSize		GetLineCount() const;
	JIndex		GetLineForChar(const JIndex charIndex) const;
	void		GoToLine(const JIndex lineIndex);
	void		SelectLine(const JIndex lineIndex);
	JIndex		GetLineLength(const JIndex lineIndex) const;
	JIndex		GetLineStart(const JIndex lineIndex) const;
	JIndex		GetLineEnd(const JIndex lineIndex) const;
	JCoordinate	GetLineTop(const JIndex lineIndex) const;
	JCoordinate	GetLineBottom(const JIndex lineIndex) const;
	JSize		GetLineHeight(const JIndex lineIndex) const;
	JCoordinate	GetLineWidth(const JIndex lineIndex) const;
	JIndex		CRLineIndexToVisualLineIndex(const JIndex crLineIndex) const;
	JIndex		VisualLineIndexToCRLineIndex(const JIndex visualLineIndex) const;

	JCoordinate	GetCharLeft(const JIndex charIndex) const;
	JCoordinate	GetCharRight(const JIndex charIndex) const;

	JIndex		GetColumnForChar(const JIndex charIndex) const;
	void		GoToColumn(const JIndex lineIndex, const JIndex columnIndex);

	void	Undo();
	void	Redo();
	void	ClearUndo();
	void	DeactivateCurrentUndo();

	JBoolean	IsUsingMultipleUndo() const;
	void		UseMultipleUndo(const JBoolean useMultiple = kJTrue);

	JSize		GetUndoDepth() const;
	void		SetUndoDepth(const JSize maxUndoCount);

	JBoolean	IsAtLastSaveLocation() const;
	void		SetLastSaveLocation();
	void		ClearLastSaveLocation();

	void	DeleteSelection();
	void	DeleteToStartOfWord();
	void	DeleteToEndOfWord();
	void	EditSelectAll();
	void	SelectAll();

	// internal clipboard

	void	Cut();
	void	Copy();
	void	Paste();

	// other source

	JBoolean	Cut(JString16* text, JRunArray<Font>* style = NULL);
	JBoolean	Copy(JString16* text, JRunArray<Font>* style = NULL) const;
	void		Paste(const JCharacter16* text, const JRunArray<Font>* style = NULL);

	JBoolean	GetClipboard(JString16* text, JRunArray<Font>* style = NULL) const;

	static JBoolean	ContainsIllegalChars(const JString16& text);
	static JBoolean	ContainsIllegalChars(const JCharacter16* text, const JSize length);
	static JBoolean	RemoveIllegalChars(JString16* text, JRunArray<Font>* style = NULL);
	static JBoolean	ReplaceIllegalChars(JString16* text, JRunArray<Font>* style = NULL);

	void	Paginate(const JCoordinate pageHeight,
					 JArray<JCoordinate>* breakpts) const;
	virtual void	Print(JPagePrinter& p);
	void	SetPrinting(JBoolean printing);

	JBoolean	CleanRightMargin(const JBoolean coerce, JIndexRange* reformatRange);

	JSize		GetCRMLineWidth() const;
	void		SetCRMLineWidth(const JSize charCount);

	JSize		GetCRMTabCharCount() const;
	void		SetCRMTabCharCount(const JSize charCount);

	JBoolean	GetCRMRuleList(const CRMRuleList** ruleList) const;
	void		SetCRMRuleList(CRMRuleList* ruleList,
							   const JBoolean teOwnsRuleList);
	void		ClearCRMRuleList();

	JBoolean	(*GetCharacterInWordFunction())(const JString16&, const JIndex);
	void		SetCharacterInWordFunction(JBoolean (*f)(const JString16&, const JIndex));

	static JBoolean	(*GetI18NCharacterInWordFunction())(const JCharacter16);
	static void		SetI18NCharacterInWordFunction(JBoolean (*f)(const JCharacter16));

	JBoolean	AllowsDragAndDrop() const;
	void		ShouldAllowDragAndDrop(const JBoolean allow);

	static JBoolean	WillCopyWhenSelect();
	static void		ShouldCopyWhenSelect(const JBoolean copy);

	static JBoolean	WillReplaceIllegalChars();
	static void		ShouldReplaceIllegalChars(const JBoolean replace);

	JCoordinate	TEGetMinPreferredGUIWidth() const;

	JCoordinate	TEGetLeftMarginWidth() const;
	void		TESetLeftMarginWidth(const JCoordinate width);

	JColorIndex	GetCaretColor() const;
	void		SetCaretColor(const JColorIndex color);
	virtual JColorIndex	GetSelectionColor() const;
	void		SetSelectionColor(const JColorIndex color);
	virtual JColorIndex	GetSelectionOutlineColor() const;
	void		SetSelectionOutlineColor(const JColorIndex color);
	JColorIndex	GetDragColor() const;
	void		SetDragColor(const JColorIndex color);
	JColorIndex	GetWhitespaceColor() const;
	void		SetWhitespaceColor(const JColorIndex color);

	JBoolean	WillBroadcastCaretLocationChanged();
	void		ShouldBroadcastCaretLocationChanged(const JBoolean broadcast);

	JBoolean	WillBroadcastAllTextChanged();
	void		ShouldBroadcastAllTextChanged(const JBoolean broadcast);

	JBoolean	IndexValid(const JIndex charIndex) const;

public:		// ought to be protected

	struct CaretLocation
	{
		JIndex charIndex;	// caret is in front of this character in the buffer
		JIndex lineIndex;	// caret is on this line of text

		CaretLocation()
			:
			charIndex(0),
			lineIndex(0)
		{ };

		CaretLocation(const JIndex ch, const JIndex line)
			:
			charIndex(ch),
			lineIndex(line)
		{ };
	};

public:		// ought to be protected

	struct LineGeometry
	{
		JCoordinate height;		// height of line
		JCoordinate ascent;		// offset to font baseline

		LineGeometry()
			:
			height(0),
			ascent(0)
		{ };

		LineGeometry(const JCoordinate aHeight, const JCoordinate anAscent)
			:
			height(aHeight),
			ascent(anAscent)
		{ };
	};

protected:

	JBoolean	TEIsActive() const;
	void		TEActivate();
	void		TEDeactivate();

	JBoolean	TESelectionIsActive() const;
	void		TEActivateSelection();
	void		TEDeactivateSelection();

	JBoolean	TECaretIsVisible() const;
	void		TEShowCaret();
	void		TEHideCaret();

	void	SetType(const Type type);
	void	ShouldPasteStyledText(const JBoolean pasteStyled);

	void				RecalcAll(const JBoolean needAdjustStyles);
	JArray<JBoolean>	GetCmdStatus(JString* crmActionText, JString* crm2ActionText,
									 JBoolean* isReadOnly) const;

	void			TEDraw(JPainter& p, const JRect& rect);
	virtual void	TEDrawInMargin(JPainter& p, const JPoint& topLeft,
								   const LineGeometry& geom, const JIndex lineIndex);

	JBoolean	TEWillDragAndDrop(const JPoint& pt, const JBoolean extendSelection,
								  const JBoolean dropCopy) const;
	void		TEHandleMouseDown(const JPoint& pt, const JSize clickCount,
								  const JBoolean extendSelection,
								  const JBoolean partialWord);
	void		TEHandleMouseDrag(const JPoint& pt);
	void		TEHandleMouseUp(const JBoolean dropCopy);
	JBoolean	TEHitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual void		TEDisplayBusyCursor() const = 0;

	virtual JBoolean	TEBeginDND() = 0;
	void				TEDNDFinished();
	void				TEHandleDNDEnter();
	void				TEHandleDNDHere(const JPoint& pt, const JBoolean dropOnSelf);
	void				TEHandleDNDLeave();
	void				TEHandleDNDDrop(const JPoint& pt, const JBoolean dropOnSelf,
										const JBoolean dropCopy);
	virtual void		TEPasteDropData() = 0;

	JBoolean	TEIsDragging() const;
	JBoolean	TEHandleKeyPress(const JCharacter16 key, const JBoolean selectText,
								 const CaretMotion motion);

	JCoordinate	TEGetBoundsWidth() const;
	void		TESetBoundsWidth(const JCoordinate width);

	virtual void		TERefresh() = 0;
	virtual void		TERefreshRect(const JRect& rect) = 0;
	void				TERefreshLines(const JIndex first, const JIndex last);
	virtual void		TEUpdateDisplay();
	virtual void		TERedraw() = 0;
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) = 0;
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const = 0;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay) = 0;
	virtual JBoolean	TEScrollForDrag(const JPoint& pt) = 0;
	virtual void		TESetVertScrollStep(const JCoordinate vStep) = 0;
	virtual void		TEClipboardChanged() = 0;
	virtual JBoolean	TEOwnsClipboard() const = 0;
	virtual JBoolean	TEGetExternalClipboard(JString16* text, JRunArray<Font>* style) const = 0;
	virtual void		TECaretShouldBlink(const JBoolean blink) = 0;

	void				TEClearClipboard();

	virtual JCoordinate	GetTabWidth(const JIndex charIndex, const JCoordinate x) const;

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate headerHeight);
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	CRMConvertTab(JString16* charBuffer, JSize* charCount,
								  const JSize currentLineWidth) const;
	virtual JSize	CRMGetTabWidth(const JIndex textColumn) const;

	virtual JBoolean	NeedsToFilterText(const JCharacter16* text) const;
	virtual JBoolean	FilterText(JString16* text, JRunArray<Font>* style);

	virtual void	AdjustStylesBeforeRecalc(const JString16& buffer, JRunArray<Font>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

	JBoolean	GetInternalClipboard(const JString16** text,
									 const JRunArray<Font>** style = NULL) const;

	virtual JBoolean	IsCharacterInWord(const JString16& text,
								  const JIndex charIndex) const;

	// OK to override, but don't call directly
	virtual JBoolean	VIsCharacterInWord(const JString16& text,
										   const JIndex charIndex) const;

	JBoolean		GetCaretLocation(CaretLocation* caretLoc) const;
	CaretLocation	CalcCaretLocation(const JPoint& pt) const;
	JBoolean		PointInSelection(const JPoint& pt) const;
	void			MoveCaretVert(const JInteger deltaLines);
	JIndex			GetColumnForChar(const CaretLocation& caretLoc) const;

	Font		GetFont(const JIndex charIndex) const;
	void		SetFont(const JIndex startIndex,const JIndex endIndex,
						const Font& f, const JBoolean clearUndo);
	void		SetFont(const JIndex startIndex, const JRunArray<Font>& f,
						const JBoolean clearUndo);
	Font		GetCurrentFont() const;
	void		SetCurrentFont(const Font& f);
	void		SetRangeFont(const JIndex startIndex,const JIndex endIndex, const Font& f);
	const Font&	GetDefaultFont() const;
	void		SetDefaultFont(const Font& f);
	void		SetAllFontNameAndSize(const JCharacter* name, const JSize size,
									  const JCoordinate tabWidth,
									  const JBoolean breakCROnly,
									  const JBoolean clearUndo);

	void	ReplaceSelection(const JCharacter16* replaceStr,
							 const JBoolean preserveCase,
							 const JArray<JIndexRange>& submatchList);

	static JBoolean	ReadPrivateFormat(istream& input, const JTextEditor16* te,
									  JString16* text, JRunArray<Font>* style);

	void		WritePrivateFormat(ostream& output, const JFileVersion vers,
								   const JIndex startIndex, const JIndex endIndex) const;
	void		WritePrivateFormat(ostream& output, const JFileVersion vers,
								   const JString16& text, const JRunArray<Font>& style) const;
	JBoolean	WriteClipboardPrivateFormat(ostream& output, const JFileVersion vers) const;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectDrag,
		kSelectPartialWordDrag,
		kSelectWordDrag,
		kSelectLineDrag,
		kDragAndDrop,
		kLocalDragAndDrop
	};

	enum UndoState
	{
		kIdle,
		kUndo,
		kRedo
	};

	enum CRMStatus
	{
		kFoundWord,
		kFoundNewLine,
		kFinished
	};

private:

	Type				itsType;
	JString16*			itsBuffer;
	JRunArray<Font>*	itsStyles;
	JBoolean			itsActiveFlag;
	JBoolean			itsPasteStyledTextFlag;		// kJTrue => paste styled text
	JBoolean			itsPerformDNDFlag;			// kJTrue => drag-and-drop enabled
	JBoolean			itsAutoIndentFlag;			// kJTrue => auto-indent after newline enabled
	JBoolean			itsTabToSpacesFlag;			// kJTrue => 1 tab -> itsCRMTabCharCount spaces
	JBoolean			itsMoveToFrontOfTextFlag;	// kJTrue => left arrow w/ moveEOL puts caret after whitespace
	JBoolean			itsBcastLocChangedFlag;		// kJTrue => broadcast CaretLocationChanged instead of CaretLineChanged
	JBoolean			itsBcastAllTextChangedFlag;	// kJTrue => broadcast TextChanged every time
	JBoolean			itsIsPrintingFlag;			// kJTrue => stack threads through Print()
	JBoolean			itsDrawWhitespaceFlag;		// kJTrue => show tabs, spaces, newlines
	JBoolean			itsDoCopySelectFlag;		// kJTrue => SetSelection() calls Copy()
	static JBoolean		itsCopyWhenSelectFlag;		// kJTrue => SetSelection() calls Copy()
	static JBoolean		itsReplaceIllegalChars;		// kJTrue => default for replace instead of remove illegal chars

	const JFontManager*	itsFontMgr;
	Font				itsDefFont;

	JColormap*			itsColormap;			// not owned
	JColorList			itsCustomColors;		// can be NULL
	JColorIndex			itsCaretColor;
	JColorIndex			itsSelectionColor;
	JColorIndex			itsSelectionOutlineColor;
	JColorIndex			itsDragColor;
	JColorIndex			itsWhitespaceColor;

	JTEUndoBase16*			itsUndo;				// can be NULL
	JPtrArray<JTEUndoBase16>*	itsUndoList;			// NULL if not multiple undo
	JIndex					itsFirstRedoIndex;		// range [1:count+1]
	JInteger				itsLastSaveRedoIndex;	// index where text was saved -- can be outside range of itsUndoList!
	UndoState				itsUndoState;
	JSize					itsMaxUndoCount;		// maximum length of itsUndoList

	JCoordinate					itsWidth;			// pixels -- width of widest line
	JCoordinate					itsHeight;			// pixels
	JCoordinate					itsGUIWidth;		// pixels -- available width of widget aperture
	JCoordinate					itsLeftMarginWidth;	// pixels
	JCoordinate					itsDefTabWidth;		// pixels
	JCoordinate					itsMaxWordWidth;	// pixels -- widest single word; only if word wrap
	JArray<JIndex>*				itsLineStarts;		// index of first character on each line
	JArray<JCoordinate>*		itsLineWidths;		// width of each line
	JRunArray<LineGeometry>*	itsLineGeom;		// geometry of each line

	JBoolean (*itsCharInWordFn)(const JString16&, const JIndex);
	static JBoolean (*itsI18NCharInWordFn)(const JCharacter16);	// can be NULL

	// information for Recalc

	JBoolean	itsBreakCROnlyFlag;			// kJFalse => break line at whitespace
	JSize		itsPrevBufLength;			// buffer length after last Recalc

	// used while active

	JBoolean		itsSelActiveFlag;		// kJTrue => draw solid selection
	JBoolean		itsCaretVisibleFlag;	// kJTrue => draw caret
	CaretLocation	itsCaretLoc;			// insertion point is -at- this character
	JCoordinate		itsCaretX;				// horizontal location used by MoveCaretVert()
	Font			itsInsertionFont;		// style for characters that user types
	JIndexRange		itsSelection;			// caret is visible if this is empty

	// clipboard

	const JBoolean		itsStoreClipFlag;	// kJTrue => use itsClipText and itsClipStyle
	JString16*			itsClipText;		// can be NULL
	JRunArray<Font>*	itsClipStyle;		// can be NULL

	// used during drag

	DragType		itsDragType;
	DragType		itsPrevDragType;	// so extend selection will maintain drag type
	JPoint			itsStartPt;
	JPoint			itsPrevPt;
	JIndex			itsSelectionPivot;	// insertion point about which to pivot selection
	JIndexRange		itsWordSelPivot;	// range of characters to keep selected
	JIndex			itsLineSelPivot;	// line about which to pivot selection
	CaretLocation	itsDropLoc;			// insertion point at which to drop the dragged text
	JBoolean		itsIsDragSourceFlag;// kJTrue => is dragging even if itsDragType == kInvalidDrag

	// information for CleanRightMargin()

	JSize	itsCRMLineWidth;
	JSize	itsCRMTabCharCount;

	CRMRuleList*	itsCRMRuleList;		// can be NULL
	JBoolean		itsOwnsCRMRulesFlag;

private:

	void		Recalc(const JIndex startChar, const JSize minCharCount,
					   const JBoolean deletion,
					   const JBoolean needCaretBcast = kJTrue,
					   const JBoolean needAdjustStyles = kJTrue);
	void		Recalc(const CaretLocation& caretLoc, const JSize minCharCount,
					   const JBoolean deletion,
					   const JBoolean needCaretBcast = kJTrue,
					   const JBoolean needAdjustStyles = kJTrue);
	void		Recalc1(const JSize bufLength, const CaretLocation& caretLoc,
						const JSize minCharCount, JCoordinate* maxLineWidth,
						JIndex* firstLineIndex, JIndex* lastLineIndex);
	JSize		RecalcLine(const JSize bufLength, const JIndex firstCharIndex,
						   const JIndex lineIndex, JCoordinate* lineWidth,
						   JIndex* runIndex, JIndex* firstInRun);
	JBoolean	LocateNextWhitespace(const JSize bufLength, JIndex* startIndex) const;
	JSize		GetSubwordForLine(const JSize bufLength, const JIndex lineIndex,
								  const JIndex startIndex, JCoordinate* lineWidth) const;
	JSize		IncludeWhitespaceOnLine(const JSize bufLength, const JIndex startIndex,
										JCoordinate* lineWidth, JBoolean* endOfLine,
										JIndex* runIndex, JIndex* firstInRun) const;
	JBoolean	NoPrevWhitespaceOnLine(const JCharacter16* str, const CaretLocation& startLoc) const;

	void	TEDrawText(JPainter& p, const JRect& rect);
	void	TEDrawLine(JPainter& p, const JCoordinate top, const LineGeometry& geom,
					   const JIndex lineIndex, JIndex* runIndex, JIndex* firstInRun);
	void	TEDrawSelection(JPainter& p, const JRect& rect, const JIndex startVisLine,
							const JCoordinate startVisLineTop);
	void	TEDrawCaret(JPainter& p, const CaretLocation& caretLoc);

	static JBoolean	DefaultIsCharacterInWord(const JString16& text,
											 const JIndex charIndex);

	void	AdjustRangesForReplace(JArray<JIndexRange>* list);

	Font	CalcInsertionFont(const JIndex charIndex) const;
	void	DropSelection(const JIndex dropLoc, const JBoolean dropCopy);

	JBoolean			GetCurrentUndo(JTEUndoBase16** undo) const;
	JBoolean			GetCurrentRedo(JTEUndoBase16** redo) const;
	void				NewUndo(JTEUndoBase16* undo, const JBoolean isNew);
	void				SameUndo(JTEUndoBase16* undo);
	void				ReplaceUndo(JTEUndoBase16* oldUndo, JTEUndoBase16* newUndo);
	void				ClearOutdatedUndo();
	JTEUndoTyping16*		GetTypingUndo(JBoolean* isNew);
	JTEUndoStyle16*		GetStyleUndo(JBoolean* isNew);
	JTEUndoStyle16*		GetStyleUndo(JBoolean* isNew, const JIndex range_start, const JIndex range_end);
	JTEUndoTabShift16*	GetTabShiftUndo(JBoolean* isNew);

	JSize	PrivatePaste(const JCharacter16* text, const JRunArray<Font>* style);
	JSize	InsertText(const JIndex charIndex, const JCharacter16* text,
					   const JRunArray<Font>* style = NULL);
	void	DeleteText(const JIndex startIndex, const JIndex endIndex);
	void	DeleteText(const JIndexRange& range);

	void	TECreateClipboard();

	void			SetCaretLocation(const CaretLocation& caretLoc);
	CaretLocation	CalcCaretLocation(const JIndex charIndex) const;
	JIndex			CalcLineIndex(const JCoordinate y, JCoordinate* lineTop) const;
	JBoolean		TEScrollTo(const CaretLocation& caretLoc);
	JRect			CalcCaretRect(const CaretLocation& caretLoc) const;
	void			TERefreshCaret(const CaretLocation& caretLoc);

	JCoordinate	GetCharLeft(const CaretLocation& charLoc) const;
	JCoordinate	GetCharRight(const CaretLocation& charLoc) const;
	JCoordinate	GetCharWidth(const CaretLocation& charLoc) const;
	JCoordinate	GetStringWidth(const JIndex startIndex, const JIndex endIndex) const;
	JCoordinate	GetStringWidth(const JIndex startIndex, const JIndex endIndex,
							   JIndex* runIndex, JIndex* firstInRun) const;

	void	PrivateSetBreakCROnly(const JBoolean breakCROnly);
	void	TEGUIWidthChanged();

	JCoordinate	GetEWNHeight() const;

	JBoolean	SetText1(const JRunArray<Font>* style);
	JRect		CalcLocalDNDRect(const JPoint& pt) const;

	void	AutoIndent(JTEUndoTyping16* typingUndo);
	void	InsertSpacesForTab();

	JBoolean	PrivateCleanRightMargin(const JBoolean coerce,
										JIndexRange* textRange,
										JString16* newText, JRunArray<Font>* newStyles,
										JIndex* newCaretIndex) const;
	JBoolean	CRMGetRange(const JIndex caretChar, const JBoolean coerce,
							JIndexRange* range, JIndex* textStartIndex,
							JString16* firstLinePrefix, JSize* firstPrefixLength,
							JString16* restLinePrefix, JSize* restPrefixLength,
							JIndex* returnRuleIndex) const;
	JBoolean	CRMGetPrefix(JIndex* startChar, const JIndex endChar,
							 JString16* linePrefix, JSize* prefixLength,
							 JIndex* ruleIndex) const;
	JIndexRange	CRMMatchPrefix(const JIndexRange& textRange, JIndex* ruleIndex) const;
	JBoolean	CRMLineMatchesRest(const JIndexRange& range) const;
	JSize		CRMCalcPrefixLength(JString16* linePrefix) const;
	JString16	CRMBuildRestPrefix(const JString16& firstLinePrefix,
								   const JIndex ruleIndex, JSize* prefixLength) const;
	void		CRMTossLinePrefix(JIndex* charIndex, const JIndex endChar,
								  const JIndex ruleIndex) const;
	CRMStatus	CRMReadNextWord(JIndex* charIndex, const JIndex endIndex,
								JString16* spaceBuffer, JSize* spaceCount,
								JString16* wordBuffer, JRunArray<Font>* wordStyles,
								const JSize currentLineWidth,
								const JIndex origCaretIndex, JIndex* newCaretIndex,
								const JString16& newText, const JBoolean requireSpace) const;
	int			CRMIsEOS(const JCharacter16 c) const;
	void		CRMAppendWord(JString16* newText, JRunArray<Font>* newStyles,
							  JSize* currentLineWidth, JIndex* newCaretIndex,
							  const JString16& spaceBuffer, const JSize spaceCount,
							  const JString16& wordBuffer, const JRunArray<Font>& wordStyles,
							  const JString16& linePrefix, const JSize prefixLength) const;

	JBoolean	LocateTab(const JIndex startIndex, const JIndex endIndex,
						  JIndex* tabIndex) const;

	Font	CalcWSFont(const Font& prevFont, const Font& nextFont) const;

	JColorIndex	ColorNameToColorIndex(const JCharacter* name) const;
	JColorIndex	RGBToColorIndex(const JRGB& color) const;

	void		BroadcastCaretMessages(const CaretLocation& caretLoc,
									   const JBoolean lineChanged);
	JBoolean	BroadcastCaretPosChanged(const CaretLocation& caretLoc);

	static JInteger	GetLineHeight(const LineGeometry& data);

	// not allowed

	const JTextEditor16& operator=(const JTextEditor16& source);

public:

	// JBroadcaster messages

	static const JCharacter* kTypeChanged;
	static const JCharacter* kTextSet;
	static const JCharacter* kTextChanged;
	static const JCharacter* kCaretLineChanged;
	static const JCharacter* kCaretLocationChanged;

	class TypeChanged : public JBroadcaster::Message
		{
		public:

			TypeChanged(const Type type)
				:
				JBroadcaster::Message(kTypeChanged),
				itsType(type)
				{ };

			Type
			GetNewType() const
			{
				return itsType;
			};

		private:

			Type	itsType;
		};

	class TextSet : public JBroadcaster::Message
		{
		public:

			TextSet()
				:
				JBroadcaster::Message(kTextSet)
				{ };
		};

	class TextChanged : public JBroadcaster::Message
		{
		public:

			TextChanged()
				:
				JBroadcaster::Message(kTextChanged)
				{ };
		};

	class CaretLineChanged : public JBroadcaster::Message
		{
		public:

			CaretLineChanged(const JIndex lineIndex)
				:
				JBroadcaster::Message(kCaretLineChanged),
				itsLineIndex(lineIndex)
				{ };

			JIndex
			GetLineIndex() const
			{
				return itsLineIndex;
			};

		private:

			JIndex itsLineIndex;
		};

	class CaretLocationChanged : public JBroadcaster::Message
		{
		public:

			CaretLocationChanged(const JIndex lineIndex, const JIndex columnIndex)
				:
				JBroadcaster::Message(kCaretLocationChanged),
				itsLineIndex(lineIndex),
				itsColumnIndex(columnIndex)
				{ };

			JIndex
			GetLineIndex() const
			{
				return itsLineIndex;
			};

			JIndex
			GetColumnIndex() const
			{
				return itsColumnIndex;
			};

		private:

			JIndex itsLineIndex, itsColumnIndex;
		};
};


/******************************************************************************
 GetType

 ******************************************************************************/

inline JTextEditor16::Type
JTextEditor16::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 IsReadOnly

 ******************************************************************************/

inline JBoolean
JTextEditor16::IsReadOnly()
	const
{
	return JConvertToBoolean( itsType != kFullEditor );
}

/******************************************************************************
 TEIsActive (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor16::TEIsActive()
	const
{
	return itsActiveFlag;
}

/******************************************************************************
 TEActivate (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEActivate()
{
	if (!itsActiveFlag)
		{
		itsActiveFlag = kJTrue;
		TERefresh();
		}
}

/******************************************************************************
 TEDeactivate (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEDeactivate()
{
	if (itsActiveFlag)
		{
		itsActiveFlag = kJFalse;
		TERefresh();
		}
}

/******************************************************************************
 TESelectionIsActive (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor16::TESelectionIsActive()
	const
{
	return itsSelActiveFlag;
}

/******************************************************************************
 TEActivateSelection (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEActivateSelection()
{
	if (!itsSelActiveFlag)
		{
		itsSelActiveFlag = kJTrue;
		TERefresh();
		}
}

/******************************************************************************
 TEDeactivateSelection (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEDeactivateSelection()
{
	if (itsSelActiveFlag)
		{
		itsSelActiveFlag = kJFalse;
		TERefresh();
		}
}

/******************************************************************************
 TECaretIsVisible (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor16::TECaretIsVisible()
	const
{
	return itsCaretVisibleFlag;
}

/******************************************************************************
 TEShowCaret (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEShowCaret()
{
	if (!itsCaretVisibleFlag)
		{
		itsCaretVisibleFlag = kJTrue;
		TERefreshCaret(itsCaretLoc);
		}
}

/******************************************************************************
 TEHideCaret (protected)

 ******************************************************************************/

inline void
JTextEditor16::TEHideCaret()
{
	if (itsCaretVisibleFlag)
		{
		itsCaretVisibleFlag = kJFalse;
		TERefreshCaret(itsCaretLoc);
		}
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JTextEditor16::IsEmpty()
	const
{
	return itsBuffer->IsEmpty();
}

/******************************************************************************
 GetText16

 ******************************************************************************/

inline const JString16&
JTextEditor16::GetText16()
	const
{
	return *itsBuffer;
}

/******************************************************************************
 GetTextLength

 ******************************************************************************/

inline JSize
JTextEditor16::GetTextLength()
	const
{
	return itsBuffer->GetLength();
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JTextEditor16::IndexValid
	(
	const JIndex charIndex
	)
	const
{
	return itsBuffer->IndexValid(charIndex);
}

/******************************************************************************
 GetStyles

 ******************************************************************************/

inline const JRunArray<JTextEditor16::Font>&
JTextEditor16::GetStyles()
	const
{
	return *itsStyles;
}

/******************************************************************************
 Selection

 ******************************************************************************/

inline JBoolean
JTextEditor16::HasSelection()
	const
{
	return JNegate( itsSelection.IsEmpty() );
}

inline JBoolean
JTextEditor16::GetSelection
	(
	JIndex* startIndex,
	JIndex* endIndex
	)
	const
{
	*startIndex = itsSelection.first;
	*endIndex   = itsSelection.last;
	return JNegate( itsSelection.IsEmpty() );
}

inline JBoolean
JTextEditor16::GetSelection
	(
	JIndexRange* range
	)
	const
{
	*range = itsSelection;
	return JNegate( itsSelection.IsEmpty() );
}

inline void
JTextEditor16::SetSelection
	(
	const JIndexRange&	range,
	const JBoolean		needCaretBcast
	)
{
	SetSelection(range.first, range.last, needCaretBcast);
}

/******************************************************************************
 Multiple undo

 ******************************************************************************/

inline JBoolean
JTextEditor16::IsUsingMultipleUndo()
	const
{
	return JI2B( itsUndoList != NULL );
}

inline JSize
JTextEditor16::GetUndoDepth()
	const
{
	return itsMaxUndoCount;
}

/******************************************************************************
 Last save location

 ******************************************************************************/

inline JBoolean
JTextEditor16::IsAtLastSaveLocation()
	const
{
	return JI2B( itsUndoList != NULL &&
				 itsLastSaveRedoIndex > 0 &&
				 JIndex(itsLastSaveRedoIndex) == itsFirstRedoIndex );
}

inline void
JTextEditor16::SetLastSaveLocation()
{
	itsLastSaveRedoIndex = itsFirstRedoIndex;
}

inline void
JTextEditor16::ClearLastSaveLocation()
{
	itsLastSaveRedoIndex = 0;
}

/******************************************************************************
 IsEntireWord

	Return kJTrue if the given character range is a single, complete word.

 ******************************************************************************/

inline JBoolean
JTextEditor16::IsEntireWord
	(
	const JIndexRange& range
	)
	const
{
	return IsEntireWord(range.first, range.last);
}

/******************************************************************************
 TEGetFontManager

 ******************************************************************************/

inline const JFontManager*
JTextEditor16::TEGetFontManager()
	const
{
	return itsFontMgr;
}

/******************************************************************************
 TEGetColormap

 ******************************************************************************/

inline JColormap*
JTextEditor16::TEGetColormap()
	const
{
	return itsColormap;
}

/******************************************************************************
 Allow paste styled text

	ShouldPasteStyled() is protected because most derived classes won't
	be written to expect it to change.

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillPasteStyledText()
	const
{
	return itsPasteStyledTextFlag;
}

// protected

inline void
JTextEditor16::ShouldPasteStyledText
	(
	const JBoolean pasteStyled
	)
{
	itsPasteStyledTextFlag = pasteStyled;
}

/******************************************************************************
 Get default font

 ******************************************************************************/

inline JSize
JTextEditor16::GetDefaultFontSize()
	const
{
	return itsDefFont.size;
}

inline const JFontStyle&
JTextEditor16::GetDefaultFontStyle()
	const
{
	return itsDefFont.style;
}

inline JTextEditor16::AlignmentType
JTextEditor16::GetDefaultFontAlign()
	const
{
	return itsDefFont.align;
}

inline void
JTextEditor16::GetDefaultFont
	(
	JFontID*		id,
	JSize*			size,
	JFontStyle*		style,
	AlignmentType*	align
	)
	const
{
	*id    = itsDefFont.id;
	*size  = itsDefFont.size;
	*style = itsDefFont.style;
	*align = itsDefFont.align;
}

// protected

inline const JTextEditor16::Font&
JTextEditor16::GetDefaultFont()
	const
{
	return itsDefFont;
}

/******************************************************************************
 Set default font

 ******************************************************************************/

inline void
JTextEditor16::SetDefaultFont
	(
	const JCharacter*	name,
	const JSize			size,
	const JFontStyle&	style,
	const AlignmentType	align
	)
{
	itsDefFont.size  = size;
	itsDefFont.style = style;
	SetDefaultFontName(name);	// last, so itsDefFont.id gets set correctly
	itsDefFont.align = align;
}

// protected

inline void
JTextEditor16::SetDefaultFont
	(
	const Font& f
	)
{
	SetDefaultFont(f.id, f.size, f.style, f.align);
}

/******************************************************************************
 Default tab width

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetDefaultTabWidth()
	const
{
	return itsDefTabWidth;
}

inline void
JTextEditor16::SetDefaultTabWidth
	(
	const JCoordinate width
	)
{
	if (width != itsDefTabWidth)
		{
		itsDefTabWidth = width;
		RecalcAll(kJFalse);
		}
}

/******************************************************************************
 Auto indenting

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillAutoIndent()
	const
{
	return itsAutoIndentFlag;
}

inline void
JTextEditor16::ShouldAutoIndent
	(
	const JBoolean indent
	)
{
	itsAutoIndentFlag = indent;
}

/******************************************************************************
 Tab inserts spaces

 ******************************************************************************/

inline JBoolean
JTextEditor16::TabInsertsSpaces()
	const
{
	return itsTabToSpacesFlag;
}

inline void
JTextEditor16::TabShouldInsertSpaces
	(
	const JBoolean spaces
	)
{
	itsTabToSpacesFlag = spaces;
}

/******************************************************************************
 Whitespace is visible

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillShowWhitespace()
	const
{
	return itsDrawWhitespaceFlag;
}

inline void
JTextEditor16::ShouldShowWhitespace
	(
	const JBoolean show
	)
{
	itsDrawWhitespaceFlag = show;
}

/******************************************************************************
 Move caret to front of text instead of front of line

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillMoveToFrontOfText()
	const
{
	return itsMoveToFrontOfTextFlag;
}

inline void
JTextEditor16::ShouldMoveToFrontOfText
	(
	const JBoolean moveToFront
	)
{
	itsMoveToFrontOfTextFlag = moveToFront;
}

/******************************************************************************
 Automatically copy when select text (static)

	This is primarily useful under the X Window System.  It is static
	because all text should work the same way.

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillCopyWhenSelect()
{
	return itsCopyWhenSelectFlag;
}

inline void
JTextEditor16::ShouldCopyWhenSelect
	(
	const JBoolean copy
	)
{
	itsCopyWhenSelectFlag = copy;
}

/******************************************************************************
Replace illegal chars instead of remove them

	This is primarily useful under the X Window System.  It is static
	because all text should work the same way.

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillReplaceIllegalChars()
{
	return itsReplaceIllegalChars;
}

inline void
JTextEditor16::ShouldReplaceIllegalChars
	(
	const JBoolean replace
	)
{
	itsReplaceIllegalChars = replace;
}

/******************************************************************************
 Drag-And-Drop

 ******************************************************************************/

inline JBoolean
JTextEditor16::AllowsDragAndDrop()
	const
{
	return itsPerformDNDFlag;
}

inline void
JTextEditor16::ShouldAllowDragAndDrop
	(
	const JBoolean allow
	)
{
	itsPerformDNDFlag = allow;
}

inline void
JTextEditor16::SetPrinting
	(
	const JBoolean printing
	)
{
	itsIsPrintingFlag = printing;
}

/******************************************************************************
 CleanRightMargin() parameters

 ******************************************************************************/

inline JSize
JTextEditor16::GetCRMLineWidth()
	const
{
	return itsCRMLineWidth;
}

inline JSize
JTextEditor16::GetCRMTabCharCount()
	const
{
	return itsCRMTabCharCount;
}

inline JBoolean
JTextEditor16::GetCRMRuleList
	(
	const CRMRuleList** ruleList
	)
	const
{
	*ruleList = itsCRMRuleList;
	return JI2B( itsCRMRuleList != NULL );
}

/******************************************************************************
 GetCaretLocation

 ******************************************************************************/

inline JBoolean
JTextEditor16::GetCaretLocation
	(
	JIndex* charIndex
	)
	const
{
	*charIndex = itsCaretLoc.charIndex;
	return itsSelection.IsEmpty();
}

// protected

inline JBoolean
JTextEditor16::GetCaretLocation
	(
	CaretLocation* caretLoc
	)
	const
{
	*caretLoc = itsCaretLoc;
	return itsSelection.IsEmpty();
}

/******************************************************************************
 CalcCaretLocation (private)

 ******************************************************************************/

inline JTextEditor16::CaretLocation
JTextEditor16::CalcCaretLocation
	(
	const JIndex charIndex
	)
	const
{
	return CaretLocation(charIndex, GetLineForChar(charIndex));
}

/******************************************************************************
 TEGetBoundsWidth (protected)

 ******************************************************************************/

inline JCoordinate
JTextEditor16::TEGetBoundsWidth()
	const
{
	return itsWidth;
}

/******************************************************************************
 TEGetLeftMarginWidth

 ******************************************************************************/

inline JCoordinate
JTextEditor16::TEGetLeftMarginWidth()
	const
{
	return itsLeftMarginWidth;
}

/******************************************************************************
 WillBreakCROnly

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillBreakCROnly()
	const
{
	return itsBreakCROnlyFlag;
}

/******************************************************************************
 GetLineCount

 ******************************************************************************/

inline JSize
JTextEditor16::GetLineCount()
	const
{
	return itsLineStarts->GetElementCount();
}

/******************************************************************************
 GetLineStart

	Returns the first character on the specified line.

 ******************************************************************************/

inline JIndex
JTextEditor16::GetLineStart
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineStarts->GetElement(lineIndex);
}

/******************************************************************************
 GetLineLength

	Returns the length of the specified line.

 ******************************************************************************/

inline JSize
JTextEditor16::GetLineLength
	(
	const JIndex lineIndex
	)
	const
{
	return (GetLineEnd(lineIndex) - GetLineStart(lineIndex) + 1);
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

inline JSize
JTextEditor16::GetLineHeight
	(
	const JIndex lineIndex
	)
	const
{
	return (itsLineGeom->GetElement(lineIndex)).height;
}

/******************************************************************************
 GetLineBottom

	Returns the bottom of the specified line.

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetLineBottom
	(
	const JIndex lineIndex
	)
	const
{
	return (GetLineTop(lineIndex) + GetLineHeight(lineIndex) - 1);
}

/******************************************************************************
 GetLineWidth

	Returns the width of the specified line.

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetLineWidth
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineWidths->GetElement(lineIndex);
}

/******************************************************************************
 GetCharRight (private)

	Returns the ending x coordinate of the specified character, excluding
	the left margin width.

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetCharRight
	(
	const CaretLocation& charLoc
	)
	const
{
	return GetCharLeft(charLoc) + GetCharWidth(charLoc);
}

/******************************************************************************
 GetCharLeft

	Returns the starting x coordinate of the specified character, including
	the left margin width

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetCharLeft
	(
	const JIndex charIndex
	)
	const
{
	return itsLeftMarginWidth + GetCharLeft(CalcCaretLocation(charIndex));
}

/******************************************************************************
 GetCharRight

	Returns the ending x coordinate of the specified character, including
	the left margin width

 ******************************************************************************/

inline JCoordinate
JTextEditor16::GetCharRight
	(
	const JIndex charIndex
	)
	const
{
	return itsLeftMarginWidth + GetCharRight(CalcCaretLocation(charIndex));
}

/******************************************************************************
 Broadcast CaretLocationChanged instead of CaretLineChanged

	Broadcasting both messages is redundant.

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillBroadcastCaretLocationChanged()
{
	return itsBcastLocChangedFlag;
}

inline void
JTextEditor16::ShouldBroadcastCaretLocationChanged
	(
	const JBoolean broadcast
	)
{
	itsBcastLocChangedFlag = broadcast;
}

/******************************************************************************
 Always broadcast TextChanged

	If this is set, TextChanged is always broadcast instead of optimizing
	out redundant messages generated by the active undo object.

 ******************************************************************************/

inline JBoolean
JTextEditor16::WillBroadcastAllTextChanged()
{
	return itsBcastAllTextChangedFlag;
}

inline void
JTextEditor16::ShouldBroadcastAllTextChanged
	(
	const JBoolean broadcast
	)
{
	itsBcastAllTextChangedFlag = broadcast;
}

/******************************************************************************
 Recalc (private)

 ******************************************************************************/

inline void
JTextEditor16::Recalc
	(
	const JIndex	startChar,
	const JSize		minCharCount,
	const JBoolean	deletion,
	const JBoolean	needCaretBcast,
	const JBoolean	needAdjustStyles
	)
{
	Recalc(CalcCaretLocation(startChar), minCharCount, deletion,
		   needCaretBcast, needAdjustStyles);
}

/******************************************************************************
 Font access (protected)

 ******************************************************************************/

inline JTextEditor16::Font
JTextEditor16::GetFont
	(
	const JIndex charIndex
	)
	const
{
	return itsStyles->GetElement(charIndex);
}

/******************************************************************************
 EndsWithNewline

 ******************************************************************************/

inline JBoolean
JTextEditor16::EndsWithNewline()
	const
{
	return JConvertToBoolean(
				!itsBuffer->IsEmpty() &&
				itsBuffer->GetLastCharacter() == '\n' );
}

/******************************************************************************
 ContainsIllegalChars (static)

 ******************************************************************************/

inline JBoolean
JTextEditor16::ContainsIllegalChars
	(
	const JString16& text
	)
{
	return ContainsIllegalChars(text, text.GetLength());
}

/******************************************************************************
 Colors

 ******************************************************************************/

inline JColorIndex
JTextEditor16::GetCaretColor()
	const
{
	return itsCaretColor;
}

inline void
JTextEditor16::SetCaretColor
	(
	const JColorIndex color
	)
{
	if (color != itsCaretColor)
		{
		itsCaretColor = color;
		TERefreshCaret(itsCaretLoc);
		}
}

inline JColorIndex
JTextEditor16::GetSelectionColor()
	const
{
	return itsSelectionColor;
}

inline void
JTextEditor16::SetSelectionColor
	(
	const JColorIndex color
	)
{
	if (color != itsSelectionColor)
		{
		itsSelectionColor = color;
		if (!itsSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorIndex
JTextEditor16::GetSelectionOutlineColor()
	const
{
	return itsSelectionOutlineColor;
}

inline void
JTextEditor16::SetSelectionOutlineColor
	(
	const JColorIndex color
	)
{
	if (color != itsSelectionOutlineColor)
		{
		itsSelectionOutlineColor = color;
		if (!itsSelection.IsEmpty())
			{
			TERefresh();
			}
		}
}

inline JColorIndex
JTextEditor16::GetDragColor()
	const
{
	return itsDragColor;
}

inline void
JTextEditor16::SetDragColor
	(
	const JColorIndex color
	)
{
	itsDragColor = color;
}

inline JColorIndex
JTextEditor16::GetWhitespaceColor()
	const
{
	return itsWhitespaceColor;
}

inline void
JTextEditor16::SetWhitespaceColor
	(
	const JColorIndex color
	)
{
	if (color != itsWhitespaceColor)
		{
		itsWhitespaceColor = color;
		TERefresh();
		}
}

/******************************************************************************
 Dragging (protected)

 ******************************************************************************/

inline JBoolean
JTextEditor16::TEIsDragging()
	const
{
	return JI2B( itsDragType != kInvalidDrag || itsIsDragSourceFlag );
}

inline void
JTextEditor16::TEDNDFinished()
{
	itsIsDragSourceFlag = kJFalse;
}

/******************************************************************************
 CaretLocation operators

 ******************************************************************************/

inline int
operator==
	(
	const JTextEditor16::CaretLocation& l1,
	const JTextEditor16::CaretLocation& l2
	)
{
	return (l1.charIndex == l2.charIndex);
}

inline int
operator!=
	(
	const JTextEditor16::CaretLocation& l1,
	const JTextEditor16::CaretLocation& l2
	)
{
	return (l1.charIndex != l2.charIndex);
}

/******************************************************************************
 Font operators

 ******************************************************************************/

inline int
operator==
	(
	const JTextEditor16::Font& f1,
	const JTextEditor16::Font& f2
	)
{
	return (f1.id == f2.id && f1.size == f2.size && f1.style == f2.style && f1.align == f2.align);
}

inline int
operator!=
	(
	const JTextEditor16::Font& f1,
	const JTextEditor16::Font& f2
	)
{
	return !(f1 == f2);
}

/******************************************************************************
 LineGeometry operators

 ******************************************************************************/

inline int
operator==
	(
	const JTextEditor16::LineGeometry& g1,
	const JTextEditor16::LineGeometry& g2
	)
{
	return (g1.height == g2.height && g1.ascent == g2.ascent);
}

inline int
operator!=
	(
	const JTextEditor16::LineGeometry& g1,
	const JTextEditor16::LineGeometry& g2
	)
{
	return !(g1 == g2);
}

#endif
