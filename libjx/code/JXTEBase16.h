/******************************************************************************
 JXTEBase16.h

	Interface for the JXTEBase16 class

	Copyright  1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTEBase16
#define _H_JXTEBase16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <JTextEditor16.h>
#include <JError.h>

class JXMenuBar;
class JXTextMenu;
class JXPSPrinter;
class JXPTPrinter;
class JXTEBlinkCaretTask16;

class JXTEBase16 : public JXScrollableWidget, public JTextEditor16
{
	friend class JXTEBlinkCaretTask16;
	friend class JXSpellChecker;

public:

	// do not change these values because they can be stored in prefs files

	enum PartialWordModifier
	{
		kCtrlMetaPWMod = 0,
		kMod2PWMod,
		kMod3PWMod,
		kMod4PWMod,
		kMod5PWMod
	};

public:

	virtual ~JXTEBase16();

	virtual void	Activate();
	virtual void	Deactivate();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual void	HandleShortcut(const int key, const JXKeyModifiers& modifiers);

	JXTextMenu*		AppendEditMenu(JXMenuBar* menuBar,
								   const JBoolean showAdjustMarginsCmds,
								   const JBoolean allowAdjustMargins,
								   const JBoolean showToggleReadOnly,
								   const JBoolean allowToggleReadOnly);
	JXTextMenu*		ShareEditMenu(JXTEBase16* te,
								  const JBoolean allowAdjustMargins,
								  const JBoolean allowToggleReadOnly);
	void			ShareEditMenu(JXTextMenu* menu,
								  const JBoolean allowAdjustMargins,
								  const JBoolean allowToggleReadOnly);
	JBoolean		HasEditMenu() const;
	JBoolean		GetEditMenu(JXTextMenu** menu) const;
	JBoolean		EditMenuIndexToCmd(const JIndex index, CmdIndex* cmd) const;
	JBoolean		EditMenuCmdToIndex(const CmdIndex cmd, JIndex* index) const;

	virtual JBoolean	TEHasSearchText() const;

	void			SetPSPrinter(JXPSPrinter* p);
	const JString&	GetPSPrintFileName() const;
	void			SetPSPrintFileName(const JCharacter* fileName);
	void			HandlePSPageSetup();
	void			PrintPS();

	void			SetPTPrinter(JXPTPrinter* p);
	const JString&	GetPTPrintFileName() const;
	void			SetPTPrintFileName(const JCharacter* fileName);
	void			HandlePTPageSetup();
	void			PrintPT();

	virtual JColorIndex	GetSelectionColor() const;

	static PartialWordModifier	GetPartialWordModifier();
	static void					SetPartialWordModifier(const PartialWordModifier mod);

	static JBoolean	CaretWillFollowScroll();
	static void		CaretShouldFollowScroll(const JBoolean follow);

	static JBoolean	WillUseWindowsHomeEnd();
	static void		ShouldUseWindowsHomeEnd(const JBoolean use);

	void	MoveCaretToEdge(const int key);

	JCoordinate	GetMinBoundsWidth() const;
	JCoordinate	GetMinBoundsHeight() const;

protected:

	JXTEBase16(const Type type, const JBoolean breakCROnly,
			 const JBoolean pasteStyledText,
			 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			 const HSizingOption hSizing, const VSizingOption vSizing,
			 const JCoordinate x, const JCoordinate y,
			 const JCoordinate w, const JCoordinate h);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseEnter();
	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseLeave();

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual void		TEDisplayBusyCursor() const;

	virtual JBoolean	TEBeginDND();
	virtual void		DNDFinish(const JBoolean isDrop, const JXContainer* target);
	virtual Atom		GetDNDAction(const JXContainer* target,
									 const JXButtonStates& buttonStates,
									 const JXKeyModifiers& modifiers);
	virtual void		GetDNDAskActions(const JXButtonStates& buttonStates,
										 const JXKeyModifiers& modifiers,
										 JArray<Atom>* askActionList,
										 JPtrArray<JString>* askDescriptionList);
	virtual void		GetSelectionData(JXSelectionData* data,
										 const JCharacter* id);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);
	virtual void		TEPasteDropData();

	virtual JBoolean	TEXWillAcceptDrop(const JArray<Atom>& typeList,
										  const Atom action, const Time time,
										  const JXWidget* source);
	virtual JBoolean	TEXConvertDropData(const JArray<Atom>& typeList,
										   const Atom action, const Time time,
										   JString16* text, JRunArray<Font>* style);

	virtual void	HandleFocusEvent();
	virtual void	HandleUnfocusEvent();
	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);

	virtual void	HandleWindowFocusEvent();
	virtual void	HandleWindowUnfocusEvent();

	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);

	virtual void		TERefresh();
	virtual void		TERefreshRect(const JRect& rect);
	virtual void		TERedraw();
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY);
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay);
	virtual JBoolean	TEScrollForDrag(const JPoint& pt);
	virtual void		TESetVertScrollStep(const JCoordinate vStep);

	virtual void		TECaretShouldBlink(const JBoolean blink);

	virtual void		TEClipboardChanged();
	virtual JBoolean	TEOwnsClipboard() const;
	virtual JBoolean	TEGetExternalClipboard(JString16* text, JRunArray<Font>* style) const;

	virtual JCoordinate	GetPrintFooterHeight(JPagePrinter& p) const;
	virtual void		DrawPrintFooter(JPagePrinter& p, const JCoordinate footerHeight);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct DNDDragInfo
	{
		const JPoint*			pt;
		const JXButtonStates*	buttonStates;
		const JXKeyModifiers*	modifiers;

		DNDDragInfo(const JPoint* p, const JXButtonStates* b,
					const JXKeyModifiers* m)
			:
			pt(p), buttonStates(b), modifiers(m)
			{ };
	};

	struct DNDDropInfo
	{
		const JArray<Atom>* typeList;
		Atom				action;
		Time				time;

		DNDDropInfo(const JArray<Atom>* l, const Atom a, const Time t)
			:
			typeList(l), action(a), time(t)
			{ };
	};

private:

	JCoordinate	itsMinWidth;					// minimum required by JTextEditor16
	JCoordinate	itsMinHeight;

	JPoint			itsPrevMousePt;				// used while mouse is on top of us
	JXPSPrinter*	itsPSPrinter;				// not owned; can be NULL
	JString*		itsPSPrintName;				// file name when printing to a file; can be NULL
	JXPTPrinter*	itsPTPrinter;				// not owned; can be NULL
	JString*		itsPTPrintName;				// file name when printing to a file; can be NULL

	Atom			itsStyledText0XAtom;
	JBoolean		itsWillPasteCustomFlag;		// kJTrue => call TEXConvertDropData() in TEPasteDropData()

	static PartialWordModifier	itsPWMod;		// which modifier to use for partial word movement

	JXTEBlinkCaretTask16*	itsBlinkTask;

	static JBoolean		itsWindowsHomeEndFlag;	// kJTrue => use Windows/Motif Home/End action
	static JBoolean		itsScrollCaretFlag;		// kJTrue => caret follows when text scrolls

	// edit menu

	JXTextMenu*		itsEditMenu;				// can be NULL
	JBoolean		itsCanAdjustMarginsFlag;	// kJTrue => menu items usable
	JBoolean		itsCanToggleReadOnlyFlag;	// kJTrue => menu items usable

	// used to pass values from HandleMouseDrag() to TEBeginDND()

	DNDDragInfo*	itsDNDDragInfo;		// NULL if not in HandleMouseDrag()

	// used to pass values from HandleDNDDrop() to TEPasteDropData()

	DNDDropInfo*	itsDNDDropInfo;		// NULL if not in HandleDNDDrop()

private:

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

protected:	// CD need access to these to implement special clipboard behaviour
	JBoolean	GetAvailDataTypes(const JArray<Atom>& typeList,
								  JBoolean* canGetStyledText,
								  JBoolean* canGetText, Atom* textType) const;
	JError		GetSelectionData(const Atom selectionName, const Time time,
								 JString16* text, JRunArray<Font>* style) const;
	JError		GetSelectionData(const Atom selectionName,
								 const JArray<Atom>& typeList, const Time time,
								 JString16* text, JRunArray<Font>* style) const;

private:
	JBoolean	OKToPassToJTE(const int key, JCharacter16* c) const;
	void		RemapWindowsHomeEnd(int* key, JXKeyModifiers* modifiers) const;

	// not allowed

	JXTEBase16(const JXTEBase16& source);
	const JXTEBase16& operator=(const JXTEBase16& source);

public:

	// JError classes

	static const JCharacter* kNoData;
	static const JCharacter* kDataNotCompatible;

	class NoData : public JError
		{
		public:

			NoData()
				:
				JError(kNoData)
			{ };
		};

	class DataNotCompatible : public JError
		{
		public:

			DataNotCompatible(const Atom type = None, JXDisplay* d = NULL);
		};
};

istream& operator>>(istream& input, JXTEBase16::PartialWordModifier& pwMod);
ostream& operator<<(ostream& output, const JXTEBase16::PartialWordModifier pwMod);


/******************************************************************************
 Edit menu

 ******************************************************************************/

inline JBoolean
JXTEBase16::HasEditMenu()
	const
{
	return JConvertToBoolean( itsEditMenu != NULL );
}

inline JBoolean
JXTEBase16::GetEditMenu
	(
	JXTextMenu** menu
	)
	const
{
	*menu = itsEditMenu;
	return JConvertToBoolean( itsEditMenu != NULL );
}

/******************************************************************************
 Get min bounds width/height

	This is useful because we keep JXWidget::bounds large enough to
	fill the aperture.

 ******************************************************************************/

inline JCoordinate
JXTEBase16::GetMinBoundsWidth()
	const
{
	return itsMinWidth;
}

inline JCoordinate
JXTEBase16::GetMinBoundsHeight()
	const
{
	return itsMinHeight;
}

/******************************************************************************
 Partial word movement (static)

 ******************************************************************************/

inline JXTEBase16::PartialWordModifier
JXTEBase16::GetPartialWordModifier()
{
	return itsPWMod;
}

inline void
JXTEBase16::SetPartialWordModifier
	(
	const PartialWordModifier mod
	)
{
	itsPWMod = mod;
}

/******************************************************************************
 Caret follows scrolling (static)

 ******************************************************************************/

inline JBoolean
JXTEBase16::CaretWillFollowScroll()
{
	return itsScrollCaretFlag;
}

inline void
JXTEBase16::CaretShouldFollowScroll
	(
	const JBoolean follow
	)
{
	itsScrollCaretFlag = follow;
}

/******************************************************************************
 Windows Home/End (static)

 ******************************************************************************/

inline JBoolean
JXTEBase16::WillUseWindowsHomeEnd()
{
	return itsWindowsHomeEndFlag;
}

inline void
JXTEBase16::ShouldUseWindowsHomeEnd
	(
	const JBoolean use
	)
{
	itsWindowsHomeEndFlag = use;
}

#endif
