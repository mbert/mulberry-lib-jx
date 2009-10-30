/******************************************************************************
 JXTEBase16.cpp

	We implement the routines required by JTextEditor16 that only depend on JX.

	We paste the following data types:
		XA_STRING, TEXT, text/x-jxstyled0

	We accept drops of the following data types:
		text/plain, text/x-jxstyled0

	To paste other formats, override TEGetExternalClipboard().

	To accept drops of other formats that can be pasted (and should therefore
		display an insertion caret), override TEXWillAcceptDrop() and
		TEXConvertDropData().

	To accept drops of other formats that cannot be pasted, override
		WillAcceptDrop() and -all- four HandleDND*() functions.

	BASE CLASS = JXScrollableWidget, JTextEditor16

	Copyright  1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTEBase16.h>
#include <JXTEBlinkCaretTask16.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbar.h>
#include <JXWindowPainter.h>
#include <JXPSPrinter.h>
#include <JXPTPrinter.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXTextSelection16.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXActionDefs.h>
#include <jXKeysym.h>

#include <JFontManager.h>
#include <JString.h>
#include <JString16.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jTime.h>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <jAssert.h>

JXTEBase16::PartialWordModifier JXTEBase16::itsPWMod = JXTEBase16::kCtrlMetaPWMod;
JBoolean JXTEBase16::itsWindowsHomeEndFlag         = kJTrue;
JBoolean JXTEBase16::itsScrollCaretFlag            = kJFalse;

static const JCharacter* kDNDActionCopyDescrip = "copy the text";
static const JCharacter* kDNDActionMoveDescrip = "move the text";

static const JCharacter* kSelectionDataID = "JXTEBase16";

struct MenuItemInfo
{
	JTextEditor16::CmdIndex	cmd;
	const JCharacter*		id;
};

// JError data

const JCharacter* JXTEBase16::kNoData            = "NoData::JXTEBase16";
const JCharacter* JXTEBase16::kDataNotCompatible = "DataNotCompatible::JXTEBase16";

static const JCharacter* kDataNotCompatibleExtra = "DataNotCompatibleExtra::JXTEBase16";

// Edit menu

static const JCharacter* kEditMenuTitleStr    = "Edit";
static const JCharacter* kEditMenuShortcutStr = "#E";

static const JCharacter* kMacEditMenuStr =
	"    Undo       %k Meta-Z       %i" kJXUndoAction
	"  | Redo       %k Meta-Shift-Z %i" kJXRedoAction
	"%l| Cut        %k Meta-X       %i" kJXCutAction
	"  | Copy       %k Meta-C       %i" kJXCopyAction
	"  | Paste      %k Meta-V       %i" kJXPasteAction
	"  | Clear                      %i" kJXClearAction
	"%l| Select all %k Meta-A       %i" kJXSelectAllAction;

static const JCharacter* kWinEditMenuStr =
	"    Undo       %h uz %k Ctrl-Z       %i" kJXUndoAction
	"  | Redo       %h r  %k Ctrl-Shift-Z %i" kJXRedoAction
	"%l| Cut        %h tx %k Ctrl-X       %i" kJXCutAction
	"  | Copy       %h c  %k Ctrl-C       %i" kJXCopyAction
	"  | Paste      %h pv %k Ctrl-V       %i" kJXPasteAction
	"  | Clear      %h l                  %i" kJXClearAction
	"%l| Select all %h a  %k Ctrl-A       %i" kJXSelectAllAction;

#define kCleanRightMarginAction		"CleanRightMarginCmd::JXTEBase16"
#define kCoerceRightMarginAction	"CoerceRightMarginCmd::JXTEBase16"
#define kShiftSelLeftAction			"ShiftSelLeftCmd::JXTEBase16"
#define kShiftSelRightAction		"ShiftSelRightCmd::JXTEBase16"
#define kForceShiftSelLeftAction	"ForceShiftSelLeftCmd::JXTEBase16"
#define kToggleReadOnlyAction		"ToggleReadOnlyCmd::JXTEBase16"

static const JCharacter* kMacAdjustMarginsMenuStr =
	"  Clean right margin  %k Meta-Return       %i" kCleanRightMarginAction
	"| Coerce right margin %k Meta-Shift-Return %i" kCoerceRightMarginAction
	"| Shift left          %k Meta-[            %i" kShiftSelLeftAction
	"| Shift right         %k Meta-]            %i" kShiftSelRightAction
	"| Force shift left    %k Meta-{            %i" kForceShiftSelLeftAction;

static const JCharacter* kWinAdjustMarginsMenuStr =
	"  Clean right margin  %h m %k Ctrl-Return       %i" kCleanRightMarginAction
	"| Coerce right margin %h n %k Ctrl-Shift-Return %i" kCoerceRightMarginAction
	"| Shift left          %h e %k Ctrl-[            %i" kShiftSelLeftAction
	"| Shift right         %h i %k Ctrl-]            %i" kShiftSelRightAction
	"| Force shift left    %h f %k Ctrl-{            %i" kForceShiftSelLeftAction;

static const JCharacter* kMacReadOnlyMenuStr =
	"Read only %b %i" kToggleReadOnlyAction;

static const JCharacter* kWinReadOnlyMenuStr =
	"Read only %b %h o %i" kToggleReadOnlyAction;

static const MenuItemInfo kEditMenuItemInfo[] =
{
	{ JTextEditor16::kUndoCmd,      kJXUndoAction      },
	{ JTextEditor16::kRedoCmd,      kJXRedoAction      },
	{ JTextEditor16::kCutCmd,       kJXCutAction       },
	{ JTextEditor16::kCopyCmd,      kJXCopyAction      },
	{ JTextEditor16::kPasteCmd,     kJXPasteAction     },
	{ JTextEditor16::kDeleteSelCmd, kJXClearAction     },
	{ JTextEditor16::kSelectAllCmd, kJXSelectAllAction },

	{ JTextEditor16::kCleanRightMarginCmd,  kCleanRightMarginAction  },
	{ JTextEditor16::kCoerceRightMarginCmd, kCoerceRightMarginAction },
	{ JTextEditor16::kShiftSelLeftCmd,      kShiftSelLeftAction      },
	{ JTextEditor16::kShiftSelRightCmd,     kShiftSelRightAction     },
	{ JTextEditor16::kForceShiftSelLeftCmd, kForceShiftSelLeftAction },
	{ JTextEditor16::kToggleReadOnlyCmd,    kToggleReadOnlyAction    },
};
const JSize kEditMenuItemCount = sizeof(kEditMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kUndoIndex = 1, kRedoIndex,
	kCutIndex, kCopyIndex, kPasteIndex, kClearIndex,
	kSelectAllIndex
};

// Search menu

static const JCharacter* kSearchMenuTitleStr    = "Search";
static const JCharacter* kSearchMenuShortcutStr = "#S";

static const JCharacter* kMacSearchMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction;

static const JCharacter* kWinSearchMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction;

static const MenuItemInfo kSearchMenuItemInfo[] =
{
	{ JTextEditor16::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor16::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor16::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor16::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor16::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor16::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor16::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor16::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor16::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   }
};
const JSize kSearchMenuItemCount = sizeof(kSearchMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kSFindCmd = 1,
	kSFindPrevCmd,          kSFindNextCmd,
	kSEnterSearchStrCmd,    kSEnterReplaceStrCmd,
	kSFindSelectionBackCmd, kSFindSelectionFwdCmd,
	kSFindClipboardBackCmd, kSFindClipboardFwdCmd
};

// Search & Replace menu

static const JCharacter* kMacReplaceMenuStr =
	"    Find...                   %k Meta-F       %i" kJXFindDialogAction
	"  | Find previous             %k Meta-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %k Meta-G       %i" kJXFindNextAction
	"%l| Enter search string       %k Meta-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %k Meta-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %k Meta-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %k Meta-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %k Ctrl-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %k Ctrl-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %k Meta-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %k Meta-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %k Meta-L       %i" kJXReplaceFindNextAction
	"%l| Replace all backwards                     %i" kJXReplaceAllBackwardsAction
	"  | Replace all forward                       %i" kJXReplaceAllForwardAction
	"  | Replace all in selection                  %i" kJXReplaceAllInSelectionAction;

static const JCharacter* kWinReplaceMenuStr =
	"    Find...                   %h f %k Ctrl-F       %i" kJXFindDialogAction
	"  | Find previous             %h p %k Ctrl-Shift-G %i" kJXFindPreviousAction
	"  | Find next                 %h n %k Ctrl-G       %i" kJXFindNextAction
	"%l| Enter search string       %h s %k Ctrl-E       %i" kJXEnterSearchTextAction
	"  | Enter replace string      %h e %k Ctrl-Shift-E %i" kJXEnterReplaceTextAction
	"%l| Find selection backwards  %h b %k Ctrl-Shift-H %i" kJXFindSelectionBackwardsAction
	"  | Find selection forward    %h o %k Ctrl-H       %i" kJXFindSelectionForwardAction
	"%l| Find clipboard backwards  %h k %k Meta-Shift-H %i" kJXFindClipboardBackwardsAction
	"  | Find clipboard forward    %h w %k Meta-H       %i" kJXFindClipboardForwardAction
	"%l| Replace                   %h r %k Ctrl-=       %i" kJXReplaceSelectionAction
	"  | Replace and find previous %h v %k Ctrl-Shift-L %i" kJXReplaceFindPrevAction
	"  | Replace and find next     %h x %k Ctrl-L       %i" kJXReplaceFindNextAction
	"%l| Replace all backwards     %h c                 %i" kJXReplaceAllBackwardsAction
	"  | Replace all forward       %h d                 %i" kJXReplaceAllForwardAction
	"  | Replace all in selection  %h i                 %i" kJXReplaceAllInSelectionAction;

static const MenuItemInfo kReplaceMenuItemInfo[] =
{
	{ JTextEditor16::kFindDialogCmd,            kJXFindDialogAction             },
	{ JTextEditor16::kFindPreviousCmd,          kJXFindPreviousAction           },
	{ JTextEditor16::kFindNextCmd,              kJXFindNextAction               },
	{ JTextEditor16::kEnterSearchTextCmd,       kJXEnterSearchTextAction        },
	{ JTextEditor16::kEnterReplaceTextCmd,      kJXEnterReplaceTextAction       },
	{ JTextEditor16::kFindSelectionBackwardCmd, kJXFindSelectionBackwardsAction },
	{ JTextEditor16::kFindSelectionForwardCmd,  kJXFindSelectionForwardAction   },
	{ JTextEditor16::kFindClipboardBackwardCmd, kJXFindClipboardBackwardsAction },
	{ JTextEditor16::kFindClipboardForwardCmd,  kJXFindClipboardForwardAction   },
	{ JTextEditor16::kReplaceSelectionCmd,      kJXReplaceSelectionAction       },
	{ JTextEditor16::kReplaceFindPrevCmd,       kJXReplaceFindPrevAction        },
	{ JTextEditor16::kReplaceFindNextCmd,       kJXReplaceFindNextAction        },
	{ JTextEditor16::kReplaceAllBackwardCmd,    kJXReplaceAllBackwardsAction    },
	{ JTextEditor16::kReplaceAllForwardCmd,     kJXReplaceAllForwardAction      },
	{ JTextEditor16::kReplaceAllInSelectionCmd, kJXReplaceAllInSelectionAction  }
};
const JSize kReplaceMenuItemCount = sizeof(kReplaceMenuItemInfo)/sizeof(MenuItemInfo);

// used when setting images

enum
{
	kSRFindCmd = 1,
	kSRFindPrevCmd,          kSRFindNextCmd,
	kSREnterSearchStrCmd,    kSREnterReplaceStrCmd,
	kSRFindSelectionBackCmd, kSRFindSelectionFwdCmd,
	kSRFindClipboardBackCmd, kSRFindClipboardFwdCmd,
	kSRReplaceCmd,
	kSRReplaceAndFindPrevCmd, kSRReplaceAndFindNextCmd,
	kSRReplaceAllBackwardCmd, kSRReplaceAllForwardCmd,
	kSRReplaceAllInSelectionCmd
};

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXTEBase16::JXTEBase16
	(
	const Type			type,
	const JBoolean		breakCROnly,
	const JBoolean		pasteStyledText,
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
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	JTextEditor16(type, breakCROnly, pasteStyledText, kJFalse,
				GetFontManager(), GetColormap(),
				(GetColormap())->GetRedColor(),					// caret
				(GetColormap())->GetDefaultSelectionColor(),	// selection filled
				(GetColormap())->GetBlueColor(),				// selection outline
				(GetColormap())->GetBlackColor(),				// drag
				(GetColormap())->GetGray70Color(),				// whitespace
				GetApertureWidth()),

	itsWillPasteCustomFlag( kJFalse )
{
	itsEditMenu    = NULL;

	itsCanAdjustMarginsFlag  = kJFalse;
	itsCanToggleReadOnlyFlag = kJFalse;

	itsPSPrinter      = NULL;
	itsPSPrintName    = NULL;
	itsPTPrinter      = NULL;
	itsPTPrintName    = NULL;

	itsDNDDragInfo    = NULL;
	itsDNDDropInfo    = NULL;

	itsBlinkTask = new JXTEBlinkCaretTask16(this);
	assert( itsBlinkTask != NULL );
	TECaretShouldBlink(kJTrue);

	itsMinWidth = itsMinHeight = 0;
	RecalcAll(kJTrue);

	itsStyledText0XAtom =
		(GetDisplay())->RegisterXAtom(JXTextSelection16::GetStyledText0XAtomName());

	if (type == kStaticText)
		{
		WantInput(kJFalse);
		}
	else
		{
		WantInput(kJTrue);
		SetDefaultCursor(kJXTextEditCursor);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEBase16::~JXTEBase16()
{
	delete itsPSPrintName;
	delete itsPTPrintName;
	delete itsBlinkTask;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXTEBase16::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	TEDraw(p, rect);
}

/******************************************************************************
 HandleMouseEnter

 ******************************************************************************/

void
JXTEBase16::HandleMouseEnter()
{
	ShowCursor();
	itsPrevMousePt = JPoint(0,0);

	JXScrollableWidget::HandleMouseEnter();
}

/******************************************************************************
 HandleMouseHere

 ******************************************************************************/

void
JXTEBase16::HandleMouseHere
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (pt != itsPrevMousePt)
		{
		ShowCursor();
		}
	itsPrevMousePt = pt;

	JXScrollableWidget::HandleMouseHere(pt, modifiers);
}

/******************************************************************************
 HandleMouseLeave

 ******************************************************************************/

void
JXTEBase16::HandleMouseLeave()
{
	ShowCursor();
	JXScrollableWidget::HandleMouseLeave();
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const Type type = GetType();
	if (button > kJXRightButton)
		{
		ScrollForWheel(button, modifiers);
		}
	else if (type != kStaticText)
		{
		ShowCursor();

		if (button == kJXMiddleButton && type == kFullEditor)
			{
			TEHandleMouseDown(pt, 1, kJFalse, kJFalse);
			TEHandleMouseUp(kJFalse);
			Paste();
			}
		else if (button != kJXMiddleButton)
			{
			const JBoolean extendSelection = JI2B(
				button == kJXRightButton || modifiers.shift() );
			const JBoolean partialWord = JI2B(
				(itsPWMod == kCtrlMetaPWMod &&
				 modifiers.control() && modifiers.meta()) ||
				(itsPWMod != kCtrlMetaPWMod &&
				 modifiers.GetState(kJXMod2KeyIndex + itsPWMod - kMod2PWMod)));
			TEHandleMouseDown(pt, clickCount, extendSelection, partialWord);
			}
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	DNDDragInfo info(&pt, &buttonStates, &modifiers);
	itsDNDDragInfo = &info;

	TEHandleMouseDrag(pt);

	itsDNDDragInfo = NULL;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	TEHandleMouseUp(modifiers.meta());
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase16::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	return TEHitSamePart(pt1, pt2);
}

/******************************************************************************
 TEDisplayBusyCursor (virtual protected)

 ******************************************************************************/

void
JXTEBase16::TEDisplayBusyCursor()
	const
{
	(JXGetApplication())->DisplayBusyCursor();
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase16::TEBeginDND()
{
	assert( itsDNDDragInfo != NULL );

	JXTextSelection16* data = new JXTextSelection16(this, kSelectionDataID);
	assert( data != NULL );

	return BeginDND(*(itsDNDDragInfo->pt), *(itsDNDDragInfo->buttonStates),
					*(itsDNDDragInfo->modifiers), data);
}

/******************************************************************************
 DNDFinish (virtual protected)

 ******************************************************************************/

void
JXTEBase16::DNDFinish
	(
	const JBoolean		isDrop,
	const JXContainer*	target
	)
{
	TEDNDFinished();
}

/******************************************************************************
 GetDNDAction (virtual protected)

	This is called repeatedly during the drag so the drop action can be
	changed based on the current target, buttons, and modifier keys.

 ******************************************************************************/

Atom
JXTEBase16::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const Type type = GetType();
	if (type == kFullEditor && modifiers.control())
		{
		return (GetDNDManager())->GetDNDActionAskXAtom();
		}
	else if (type == kFullEditor &&
			 ((target == this && !modifiers.meta()) ||
			  (target != this &&  modifiers.meta())))
		{
		return (GetDNDManager())->GetDNDActionMoveXAtom();
		}
	else
		{
		return (GetDNDManager())->GetDNDActionCopyXAtom();
		}
}

/******************************************************************************
 GetDNDAskActions (virtual protected)

	This is called when the value returned by GetDNDAction() changes to
	XdndActionAsk.  If GetDNDAction() repeatedly returns XdndActionAsk,
	this function is not called again because it is assumed that the
	actions are the same within a single DND session.

	This function must place at least 2 elements in askActionList and
	askDescriptionList.

	The first element should be the default action.

 ******************************************************************************/

void
JXTEBase16::GetDNDAskActions
	(
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JArray<Atom>*			askActionList,
	JPtrArray<JString>*		askDescriptionList
	)
{
	assert( GetType() == kFullEditor );

	JXDNDManager* dndMgr = GetDNDManager();
	askActionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	askActionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());

	JString* s = new JString(kDNDActionCopyDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);

	s = new JString(kDNDActionMoveDescrip);
	assert( s != NULL );
	askDescriptionList->Append(s);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
JXTEBase16::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	if (strcmp(id, kSelectionDataID) == 0)
		{
		JXTextSelection16* textData = dynamic_cast(JXTextSelection16*, data);
		assert( textData != NULL );

		JString16* text = new JString16;
		assert( text != NULL );

		JRunArray<Font>* style = new JRunArray<Font>;
		assert( style != NULL );

		const JBoolean ok = GetSelection(text, style);
		assert( ok );
		textData->SetData(text, GetColormap(), style);

		if (GetType() == kFullEditor)
			{
			JIndexRange r;
			const JBoolean ok = GetSelection(&r);
			assert( ok );
			textData->SetTextEditor(this, r);
			}
		}
	else
		{
		JXScrollableWidget::GetSelectionData(data, id);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase16::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	itsWillPasteCustomFlag = kJFalse;

	if (GetType() != kFullEditor)
		{
		return kJFalse;
		}
	else if (source == this)
		{
		return kJTrue;
		}
	else if (TEXWillAcceptDrop(typeList, *action, time, source))
		{
		itsWillPasteCustomFlag = kJTrue;
		return kJTrue;
		}
	else
		{
		JXDNDManager* dndMgr = GetDNDManager();
		JBoolean canGetStyledText, canGetText;
		Atom textType;
		return JI2B(
			(*action == dndMgr->GetDNDActionCopyXAtom() ||
			 *action == dndMgr->GetDNDActionMoveXAtom() ||
			 *action == dndMgr->GetDNDActionAskXAtom()) &&
			GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType));
		}
}

/******************************************************************************
 TEXWillAcceptDrop (virtual protected)

	Derived classes can override this to accept other data types that
	can be pasted.  Returning kJTrue guarantees that TEXConvertDropData()
	will be called, and this can be used to accept a different data type
	even when one of the default types (e.g. text/plain) is available.

	Data types that cannot be pasted must be implemented by overriding
	WillAcceptDrop() and -all- four HandleDND*() functions.

	When overriding this function, derived classes must also override
	TEXConvertDropData() to process the actual data that is dropped.

	source is non-NULL if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

JBoolean
JXTEBase16::TEXWillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXTEBase16::HandleDNDEnter()
{
	TEHandleDNDEnter();
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXTEBase16::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	TEHandleDNDHere(pt, JI2B(source == this));
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
JXTEBase16::HandleDNDLeave()
{
	TEHandleDNDLeave();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
JXTEBase16::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	DNDDropInfo info(&typeList, action, time);
	itsDNDDropInfo = &info;

	JXDNDManager* dndMgr  = GetDNDManager();
	const Atom copyAction = dndMgr->GetDNDActionCopyXAtom();
	if (!itsWillPasteCustomFlag &&
		action == dndMgr->GetDNDActionAskXAtom())
		{
		JArray<Atom> actionList;
		JPtrArray<JString> descriptionList(JPtrArrayT::kDeleteAll);
		if (!dndMgr->GetAskActions(&actionList, &descriptionList))
			{
			itsDNDDropInfo->action = copyAction;
			}
		else if (!dndMgr->ChooseDropAction(actionList, descriptionList,
										   &(itsDNDDropInfo->action)))
			{
			TEHandleDNDLeave();
			return;
			}
		}

	TEHandleDNDDrop(pt, JI2B(source == this),
					JI2B(itsDNDDropInfo->action == copyAction));

	itsDNDDropInfo = NULL;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

	Get the data that was dropped and paste it in.

 ******************************************************************************/

void
JXTEBase16::TEPasteDropData()
{
	assert( itsDNDDropInfo != NULL );

	JXDNDManager* dndMgr     = GetDNDManager();
	const Atom selectionName = dndMgr->GetDNDSelectionName();

	JString16 text;
	JRunArray<Font> style;
	if (itsWillPasteCustomFlag &&
		TEXConvertDropData(*(itsDNDDropInfo->typeList), itsDNDDropInfo->action,
						   itsDNDDropInfo->time, &text, &style))
		{
		JRunArray<Font>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);
		}
	else if (!itsWillPasteCustomFlag &&
			 GetSelectionData(selectionName, *(itsDNDDropInfo->typeList),
							  itsDNDDropInfo->time, &text, &style) == kJNoError)
		{
		JRunArray<Font>* s = (style.IsEmpty() ? NULL : &style);
		Paste(text, s);

		if (itsDNDDropInfo->action == dndMgr->GetDNDActionMoveXAtom())
			{
			(GetSelectionManager())->
				SendDeleteRequest(selectionName, itsDNDDropInfo->time);
			}
		}
}

/******************************************************************************
 TEXConvertDropData (virtual protected)

	Derived classes can override this to convert other data types into
	text and styles that can be pasted.  Return kJTrue to paste the contents
	of text and style.  To paste unstyled text, leave style empty.

	This function will only be called if the derived class implemented
	TEXWillAcceptDrop() and returned kJTrue.

 ******************************************************************************/

JBoolean
JXTEBase16::TEXConvertDropData
	(
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	JString16*			text,
	JRunArray<Font>*	style
	)
{
	return kJFalse;
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXTEBase16::Activate()
{
	JXScrollableWidget::Activate();
	if (IsActive() && HasFocus())
		{
		TEActivate();
		if ((GetWindow())->HasFocus())
			{
			itsBlinkTask->Reset();
			(JXGetApplication())->InstallIdleTask(itsBlinkTask);
			TEActivateSelection();
			}
		}
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

void
JXTEBase16::Deactivate()
{
	JXScrollableWidget::Deactivate();
	if (!IsActive())
		{
		TEDeactivate();
		(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
		TEDeactivateSelection();
		}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	TEActivate();
	if (IsActive() && (GetWindow())->HasFocus())
		{
		itsBlinkTask->Reset();
		(JXGetApplication())->InstallIdleTask(itsBlinkTask);
		TEActivateSelection();
		}
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleUnfocusEvent()
{
	JXScrollableWidget::HandleUnfocusEvent();
	TEDeactivate();
	(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
	TEDeactivateSelection();
}

/******************************************************************************
 HandleWindowFocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleWindowFocusEvent()
{
	JXScrollableWidget::HandleWindowFocusEvent();
	TEActivateSelection();
	if (IsActive() && HasFocus())
		{
		itsBlinkTask->Reset();
		(JXGetApplication())->InstallIdleTask(itsBlinkTask);
		}
}

/******************************************************************************
 HandleWindowUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXTEBase16::HandleWindowUnfocusEvent()
{
	JXScrollableWidget::HandleWindowUnfocusEvent();
	TEHideCaret();
	TEDeactivateSelection();
	(JXGetApplication())->RemoveIdleTask(itsBlinkTask);
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
JXTEBase16::TECaretShouldBlink
	(
	const JBoolean blink
	)
{
	itsBlinkTask->ShouldBlink(blink);
}

/******************************************************************************
 HandleKeyPress (virtual)

	We handle all the edit shortcuts here because we won't always have
	an Edit menu.

 ******************************************************************************/

void
JXTEBase16::HandleKeyPress
	(
	const int				origKey,
	const JXKeyModifiers&	origModifiers
	)
{
	int key                  = origKey;
	JXKeyModifiers modifiers = origModifiers;
	if (itsWindowsHomeEndFlag)
		{
		RemapWindowsHomeEnd(&key, &modifiers);
		}

	const JBoolean controlOn = modifiers.control();
	const JBoolean metaOn    = modifiers.meta();
	const JBoolean shiftOn   = modifiers.shift();

	const Type type = GetType();
	if (type == kFullEditor && !controlOn && !metaOn &&
		((GetDisplay())->GetLatestButtonStates()).AllOff())
		{
		HideCursor();
		}

	JBoolean processed = kJFalse;
	JCharacter16 typedKey;

	if (type == kFullEditor &&
		(((key == 'z' || key == 'Z') && !controlOn &&  metaOn && !shiftOn) ||
		 (key == JXCtrl('Z')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Undo();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'x' || key == 'X') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('X')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Cut();
		processed = kJTrue;
		}
	else if (((key == 'c' || key == 'C') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('C')         &&  controlOn && !metaOn && !shiftOn))
		{
		Copy();
		processed = kJTrue;
		}
	else if (type == kFullEditor &&
			 (((key == 'v' || key == 'V') && !controlOn &&  metaOn && !shiftOn) ||
			  (key == JXCtrl('V')         &&  controlOn && !metaOn && !shiftOn)))
		{
		Paste();
		processed = kJTrue;
		}
	else if (((key == 'a' || key == 'A') && !controlOn &&  metaOn && !shiftOn) ||
			 (key == JXCtrl('A')         &&  controlOn && !metaOn && !shiftOn))
		{
		SelectAll();
		processed = kJTrue;
		}

	else if (key == kJLeftArrow || key == kJRightArrow ||
			 key == kJUpArrow   || key == kJDownArrow)
		{
		CaretMotion motion = kMoveByCharacter;
		if ((itsPWMod == kCtrlMetaPWMod && controlOn && metaOn) ||
			(itsPWMod != kCtrlMetaPWMod &&
			 modifiers.GetState(kJXMod2KeyIndex + itsPWMod - kMod2PWMod)))
			{
			motion = kMoveByPartialWord;
			}
		else if (controlOn)
			{
			motion = kMoveByWord;
			}
		else if (metaOn)
			{
			motion = kMoveByLine;
			}

		if (type == kFullEditor || shiftOn || motion != kMoveByCharacter)
			{
			processed = TEHandleKeyPress(key, shiftOn, motion);
			}
		else
			{
			processed = kJFalse;
			}
		}

	else if (type == kFullEditor &&
			 !controlOn && !metaOn &&
			 OKToPassToJTE(key, &typedKey))
		{
		processed = TEHandleKeyPress(key, shiftOn, kMoveByCharacter);
		}

	if (!processed)
		{
		if (itsScrollCaretFlag)
			{
			// move caret to top/bottom if already scrolled there

			const JRect ap = GetAperture();
			const JRect b  = GetBounds();

			if ((key == XK_Page_Up || key == XK_KP_Page_Up) &&
				 ap.top == b.top)
				{
				SetCaretLocation(1);
				}
			else if ((key == XK_Page_Down || key == XK_KP_Page_Down) &&
					  ap.bottom == b.bottom)
				{
				SetCaretLocation(GetTextLength()+1);
				}
			}

		int k = key;
		if (k == ' ')
			{
			k = XK_Page_Down;
			}
		else if (k == kJDeleteKey)
			{
			k = XK_Page_Up;
			}

		JXScrollableWidget::HandleKeyPress(k, modifiers);

		if (itsScrollCaretFlag)
			{
			// move caret to top/bottom regardless of where it was

			if (key == XK_Home || key == XK_KP_Home)
				{
				SetCaretLocation(1);
				}
			else if (key == XK_End || key == XK_KP_End)
				{
				SetCaretLocation(GetTextLength()+1);
				}
			}
		}
}

/******************************************************************************
 OKToPassToJTE (private)

	Map Latin 1,2,3,4,9 to 8-bit ASCII.

 ******************************************************************************/

JBoolean
JXTEBase16::OKToPassToJTE
	(
	const int		key,
	JCharacter16*	c
	)
	const
{
#if 0
	const int flags = 0xFF00 & key;
	*c              = 0x00FF & key;
	return JI2B(*c != 0 &&
				(flags == 0x0000 || flags == 0x0100 || flags == 0x0200 ||
				 flags == 0x0300 || flags == 0x1300));
#else
	// This is a hack - really we need to separate out the case of a valid
	// char vs a keysym.
	const int flags = 0xFF00 & key;
	*c              = 0x00FF & key;
	return JI2B(*c != 0 &&
				(flags != 0xFF00) &&
				(flags != 0xFE00));
	
#endif
}

/******************************************************************************
 RemapWindowsHomeEnd (private)

	Home/End		=> beginning/end of line
	Ctrl-Home/End	=> scroll to top/bottom

 ******************************************************************************/

void
JXTEBase16::RemapWindowsHomeEnd
	(
	int*			key,
	JXKeyModifiers*	modifiers
	)
	const
{
	if ((*key == XK_Home || *key == XK_KP_Home ||
		 *key == XK_End  || *key == XK_KP_End) &&
		modifiers->control())
		{
		modifiers->SetState(kJXControlKeyIndex, kJFalse);
		}
	else if (*key == XK_Home || *key == XK_KP_Home)
		{
		*key = kJLeftArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
	else if (*key == XK_End || *key == XK_KP_End)
		{
		*key = kJRightArrow;
		modifiers->SetState(kJXMetaKeyIndex, kJTrue);
		}
}

/******************************************************************************
 MoveCaretToEdge

	key can be either kJUpArrow or kJDownArrow.

 ******************************************************************************/

void
JXTEBase16::MoveCaretToEdge
	(
	const int key
	)
{
	JFontID id;
	JSize size;
	JFontStyle style;
	AlignmentType align;
	GetDefaultFont(&id, &size, &style, &align);
	const JSize h = (3 * (GetFontManager())->GetLineHeight(id, size, style)) / 4;

	JPoint pt;
	if (key == kJUpArrow)
		{
		pt    = (GetAperture()).topLeft();
		pt.y += h;
		}
	else if (key == kJDownArrow)
		{
		pt    = (GetAperture()).bottomLeft();
		pt.y -= h;
		}
	else
		{
		return;
		}

	const CaretLocation caretLoc = CalcCaretLocation(pt);
	SetCaretLocation(caretLoc.charIndex);
}

/******************************************************************************
 HandleShortcut (virtual)

 ******************************************************************************/

void
JXTEBase16::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JXScrollableWidget::HandleShortcut(key, modifiers);
}

/******************************************************************************
 AdjustCursor (virtual protected)

	Show the default cursor during drag-and-drop.

 ******************************************************************************/

void
JXTEBase16::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (TEWillDragAndDrop(pt, kJFalse, modifiers.meta()))
		{
		DisplayCursor(kJXDefaultCursor);
		}
	else
		{
		JXScrollableWidget::AdjustCursor(pt, modifiers);
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXTEBase16::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXScrollableWidget::BoundsMoved(dx, dy);

	if (itsScrollCaretFlag && dy > 0 &&
		GetLineTop(GetLineForChar(GetInsertionIndex())) > (GetAperture()).bottom)
		{
		MoveCaretToEdge(kJDownArrow);
		}
	else if (itsScrollCaretFlag && dy < 0 &&
			 GetLineBottom(GetLineForChar(GetInsertionIndex())) < (GetAperture()).top)
		{
		MoveCaretToEdge(kJUpArrow);
		}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXTEBase16::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXScrollableWidget::ApertureResized(dw,dh);
	TESetBoundsWidth(GetApertureWidth());
	TESetGUIBounds(itsMinWidth, itsMinHeight, -1);
}

/******************************************************************************
 TERefresh (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase16::TERefresh()
{
	Refresh();
}

/******************************************************************************
 TERefreshRect (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase16::TERefreshRect
	(
	const JRect& rect
	)
{
	RefreshRect(rect);
}

/******************************************************************************
 TERedraw (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

void
JXTEBase16::TERedraw()
{
	Redraw();
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

	Keep the bounds at least as large as the aperture.

 ******************************************************************************/

void
JXTEBase16::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
	itsMinWidth  = w;
	itsMinHeight = h;

	const JRect ap = GetApertureGlobal();

	JCoordinate width = ap.width();
	if (width < w)
		{
		width = w;
		}

	JCoordinate height = ap.height();
	if (height < h)
		{
		height = h;
		}

	JXScrollbar *hScrollbar, *vScrollbar;
	if (changeY >= 0 && GetScrollbars(&hScrollbar, &vScrollbar))
		{
		const JCoordinate origH = GetBoundsHeight();
		if (height < origH)
			{
			vScrollbar->PrepareForLowerMaxValue(changeY, origH - height);
			}
		else if (height > origH)
			{
			vScrollbar->PrepareForHigherMaxValue(changeY, height - origH);
			}
		}

	SetBounds(width, height);
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase16::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return JI2B( width > 16000 );	// X uses 2 bytes for coordinate value
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

JBoolean
JXTEBase16::TEScrollToRect
	(
	const JRect&	rect,
	const JBoolean	centerInDisplay
	)
{
	if (centerInDisplay && rect.right <= GetApertureWidth())
		{
		JRect r = rect;
		r.left  = 0;
		return ScrollToRectCentered(r, kJFalse);
		}
	else if (centerInDisplay)
		{
		return ScrollToRectCentered(rect, kJFalse);
		}
	else
		{
		return ScrollToRect(rect);
		}
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

	Not inline because it is virtual.

 ******************************************************************************/

JBoolean
JXTEBase16::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return ScrollForDrag(pt);
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
JXTEBase16::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
	SetVertStepSize(vStep);
	SetVertPageStepContext(vStep);
}

/******************************************************************************
 TEClipboardChanged (virtual protected)

 ******************************************************************************/

void
JXTEBase16::TEClipboardChanged()
{
	if (HasSelection())
		{
		JString16* text = new JString16;
		assert( text != NULL );

		JRunArray<Font>* style = new JRunArray<Font>;
		assert( style != NULL );

		const JBoolean ok = GetSelection(text, style);
		assert( ok );

		JXTextSelection16* data = new JXTextSelection16(GetDisplay(), text, GetColormap(), style);
		assert( data != NULL );

		if (!(GetSelectionManager())->SetData(kJXClipboardName, data))
			{
			(JGetUserNotification())->ReportError("Unable to copy to the X Clipboard.");
			}
		}
}

/******************************************************************************
 TEOwnsClipboard (virtual protected)

	We told JTextEditor16 not to store the clipboard data.

 ******************************************************************************/

JBoolean
JXTEBase16::TEOwnsClipboard()
	const
{
	return kJFalse;
}

/******************************************************************************
 TEGetExternalClipboard (virtual protected)

	Returns kJTrue if there is something pasteable on the system clipboard.

 ******************************************************************************/

JBoolean
JXTEBase16::TEGetExternalClipboard
	(
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	const JError err = GetSelectionData(kJXClipboardName, CurrentTime, text, style);
	if (err.OK())
		{
		return kJTrue;
		}
	else
		{
		if (err != kNoData)
			{
			err.ReportIfError();
			}
		return kJFalse;
		}
}

/******************************************************************************
 GetAvailDataTypes (private)

	Returns kJTrue if it can find a data type that we understand.

 ******************************************************************************/

JBoolean
JXTEBase16::GetAvailDataTypes
	(
	const JArray<Atom>&	typeList,
	JBoolean*			canGetStyledText,
	JBoolean*			canGetText,
	Atom*				textType
	)
	const
{
	*canGetStyledText = kJFalse;
	*canGetText       = kJFalse;
	*textType         = None;

	JXSelectionManager* selMgr = GetSelectionManager();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		const Atom type = typeList.GetElement(i);
		if (type == XA_STRING ||
			type == selMgr->GetMimePlainTextXAtom() ||
			type == selMgr->GetUTF8StringXAtom() ||
			(!(*canGetText) && type == selMgr->GetTextXAtom()))
			{
			*canGetText = kJTrue;
			*textType   = type;
			}

		// By checking WillPasteStyledText(), we avoid wasting time
		// parsing style information.

		else if (type == itsStyledText0XAtom && WillPasteStyledText())
			{
			*canGetStyledText = kJTrue;
			}
		}

	return JConvertToBoolean( *canGetStyledText || *canGetText );
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase16::GetSelectionData
	(
	const Atom			selectionName,
	const Time			time,
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	JArray<Atom> typeList;
	if ((GetSelectionManager())->
			GetAvailableTypes(selectionName, time, &typeList))
		{
		return GetSelectionData(selectionName, typeList, time, text, style);
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 GetSelectionData (private)

	Returns kJTrue if there is something pasteable in the given selection.

 ******************************************************************************/

JError
JXTEBase16::GetSelectionData
	(
	const Atom			selectionName,
	const JArray<Atom>&	typeList,
	const Time			time,
	JString16*			text,
	JRunArray<Font>*	style
	)
	const
{
	text->Clear();
	style->RemoveAll();

	JBoolean canGetStyledText, canGetText;
	Atom textType;
	if (GetAvailDataTypes(typeList, &canGetStyledText, &canGetText, &textType))
		{
		JXSelectionManager* selMgr = GetSelectionManager();

		JBoolean gotData = kJFalse;
		JError err       = JNoError();

		Atom returnType, textReturnType;
		unsigned char* data = NULL;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;

		if (!gotData && canGetStyledText &&
			selMgr->GetData(selectionName, time, itsStyledText0XAtom,
							&returnType, &data, &dataLength, &delMethod))
			{
			if (returnType == itsStyledText0XAtom)
				{
				gotData = kJTrue;
				const std::string s(reinterpret_cast<char*>(data), dataLength);
				std::istringstream input(s);
				if (!ReadPrivateFormat(input, this, text, style))
					{
					err = DataNotCompatible();
					}
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData && canGetText &&
			selMgr->GetData(selectionName, time, textType,
							&textReturnType, &data, &dataLength, &delMethod))
			{
			if (textReturnType == XA_STRING ||
				textReturnType == selMgr->GetMimePlainTextXAtom())
				{
				gotData = kJTrue;
				JString ascii(reinterpret_cast<JCharacter*>(data), dataLength);
				JString16 utf16;
				utf16.FromASCII(ascii);
				*text = utf16;
				}
			else if (textReturnType == selMgr->GetUTF8StringXAtom())
				{
				gotData = kJTrue;
				JString utf8(reinterpret_cast<JCharacter*>(data), dataLength);
				JString16 utf16;
				utf16.FromUTF8(utf8);
				*text = utf16;
				}
			selMgr->DeleteData(&data, delMethod);
			}

		if (!gotData)
			{
			err = DataNotCompatible(canGetText ? textReturnType : None, GetDisplay());
			}

		return err;
		}
	else
		{
		return NoData();
		}
}

/******************************************************************************
 DataNotCompatible::DataNotCompatible

 ******************************************************************************/

JXTEBase16::DataNotCompatible::DataNotCompatible
	(
	const Atom	type,
	JXDisplay*	display
	)
	:
	JError(kDataNotCompatible)
{
	if (type != None)
		{
		assert( display != NULL );

		const JCharacter* map[] =
			{
			"atom", XGetAtomName(*display, type)
			};
		JString msg = JGetString(kDataNotCompatible);
		msg += JGetString(kDataNotCompatibleExtra, map, sizeof(map));
		SetMessage(msg, kJTrue);
		}
}

/******************************************************************************
 AppendEditMenu

	Call this to let us create the Edit menu for text editing.

 ******************************************************************************/

#include <jx_edit_undo.xpm>
#include <jx_edit_redo.xpm>
#include <jx_edit_cut.xpm>
#include <jx_edit_copy.xpm>
#include <jx_edit_paste.xpm>
#include <jx_edit_clear.xpm>
#include <jx_edit_clean_right_margin.xpm>
#include <jx_edit_coerce_right_margin.xpm>
#include <jx_edit_shift_left.xpm>
#include <jx_edit_shift_right.xpm>
#include <jx_edit_force_shift_left.xpm>
#include <jx_edit_read_only.xpm>

JXTextMenu*
JXTEBase16::AppendEditMenu
	(
	JXMenuBar*		menuBar,
	const JBoolean	showAdjustMarginsCmds,
	const JBoolean	allowAdjustMargins,
	const JBoolean	showToggleReadOnly,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL );

	// create basic menu

	itsEditMenu = menuBar->AppendTextMenu(kEditMenuTitleStr);
	if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
		{
		itsEditMenu->SetMenuItems(kMacEditMenuStr, "JXTEBase16");
		}
	else
		{
		itsEditMenu->SetShortcuts(kEditMenuShortcutStr);
		itsEditMenu->SetMenuItems(kWinEditMenuStr, "JXTEBase16");
		}

	itsEditMenu->SetItemImage(kUndoIndex,  jx_edit_undo);
	itsEditMenu->SetItemImage(kRedoIndex,  jx_edit_redo);
	itsEditMenu->SetItemImage(kCutIndex,   jx_edit_cut);
	itsEditMenu->SetItemImage(kCopyIndex,  jx_edit_copy);
	itsEditMenu->SetItemImage(kPasteIndex, jx_edit_paste);
	itsEditMenu->SetItemImage(kClearIndex, jx_edit_clear);

	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	ListenTo(itsEditMenu);

	// append margin adjustment commands

	if (showAdjustMarginsCmds)
		{
		itsCanAdjustMarginsFlag = allowAdjustMargins;

		const JSize itemCount = itsEditMenu->GetItemCount();
		itsEditMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsEditMenu->AppendMenuItems(kMacAdjustMarginsMenuStr, "JXTEBase16");
			}
		else
			{
			itsEditMenu->AppendMenuItems(kWinAdjustMarginsMenuStr, "JXTEBase16");
			}

		itsEditMenu->SetItemImage(itemCount+1, jx_edit_clean_right_margin);
		itsEditMenu->SetItemImage(itemCount+2, jx_edit_coerce_right_margin);
		itsEditMenu->SetItemImage(itemCount+3, jx_edit_shift_left);
		itsEditMenu->SetItemImage(itemCount+4, jx_edit_shift_right);
		itsEditMenu->SetItemImage(itemCount+5, jx_edit_force_shift_left);
		}

	// append "read only" checkbox

	if (showToggleReadOnly)
		{
		itsCanToggleReadOnlyFlag = allowToggleReadOnly;

		const JSize itemCount = itsEditMenu->GetItemCount();
		itsEditMenu->ShowSeparatorAfter(itemCount);

		if (JXMenu::GetDefaultStyle() == JXMenu::kMacintoshStyle)
			{
			itsEditMenu->AppendMenuItems(kMacReadOnlyMenuStr, "JXTEBase16");
			}
		else
			{
			itsEditMenu->AppendMenuItems(kWinReadOnlyMenuStr, "JXTEBase16");
			}

		itsEditMenu->SetItemImage(itemCount+1, jx_edit_read_only);
		}

	return itsEditMenu;
}

/******************************************************************************
 ShareEditMenu

	Call this to let us share the Edit menu with other objects.

	The JXTextMenu passed to the second version must have ID's
	assigned to the standard items:
		Undo, Redo, Cut, Copy, Paste, Clear, Select All

 ******************************************************************************/

JXTextMenu*
JXTEBase16::ShareEditMenu
	(
	JXTEBase16*		te,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL && te->itsEditMenu != NULL );

	itsEditMenu              = te->itsEditMenu;
	itsCanAdjustMarginsFlag  = allowAdjustMargins;
	itsCanToggleReadOnlyFlag = allowToggleReadOnly;

	ListenTo(itsEditMenu);
	return itsEditMenu;
}

void
JXTEBase16::ShareEditMenu
	(
	JXTextMenu*		menu,
	const JBoolean	allowAdjustMargins,
	const JBoolean	allowToggleReadOnly
	)
{
	assert( itsEditMenu == NULL && menu != NULL );

	itsEditMenu              = menu;
	itsCanAdjustMarginsFlag  = allowAdjustMargins;
	itsCanToggleReadOnlyFlag = allowToggleReadOnly;

	ListenTo(itsEditMenu);
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
JXTEBase16::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		if (HasFocus())
			{
			UpdateEditMenu();
			}
		}
	else if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		if (HasFocus())
			{
			const JXMenu::ItemSelected* selection =
				dynamic_cast(const JXMenu::ItemSelected*, &message);
			assert( selection != NULL );
			HandleEditMenu(selection->GetIndex());
			}
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			SetPSPrintFileName(itsPSPrinter->GetFileName());
			Print(*itsPSPrinter);
			}
		StopListening(itsPSPrinter);
		}

	else if (sender == itsPTPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			SetPTPrintFileName(itsPTPrinter->GetFileName());
			itsPTPrinter->Print16(GetText16());
			}
		StopListening(itsPTPrinter);
		}

	else
		{
		JXScrollableWidget::Receive(sender, message);
		JTextEditor16::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
JXTEBase16::UpdateEditMenu()
{
	assert( itsEditMenu != NULL );

	JString crmActionText, crm2ActionText;
	JBoolean isReadOnly;
	const JArray<JBoolean> enableFlags =
		GetCmdStatus(&crmActionText, &crm2ActionText, &isReadOnly);

	const JSize count = itsEditMenu->GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		CmdIndex cmd;
		if (EditMenuIndexToCmd(i, &cmd))
			{
			JBoolean enable = kJTrue;
			if (cmd == kCleanRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crmActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kCoerceRightMarginCmd)
				{
				itsEditMenu->SetItemText(i, crm2ActionText);
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kShiftSelLeftCmd  ||
					 cmd == kShiftSelRightCmd ||
					 cmd == kForceShiftSelLeftCmd)
				{
				enable = itsCanAdjustMarginsFlag;
				}
			else if (cmd == kToggleReadOnlyCmd)
				{
				if (isReadOnly)
					{
					itsEditMenu->CheckItem(i);
					}
				enable = itsCanToggleReadOnlyFlag;
				}

			itsEditMenu->SetItemEnable(i, JI2B(enable && enableFlags.GetElement(cmd)));
			}
		}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
JXTEBase16::HandleEditMenu
	(
	const JIndex index
	)
{
	assert( itsEditMenu != NULL );

	CmdIndex cmd;
	if (!EditMenuIndexToCmd(index, &cmd))
		{
		return;
		}

	if (cmd == kUndoCmd)
		{
		Undo();
		}
	else if (cmd == kRedoCmd)
		{
		Redo();
		}

	else if (cmd == kCutCmd)
		{
		Cut();
		}
	else if (cmd == kCopyCmd)
		{
		Copy();
		}
	else if (cmd == kPasteCmd)
		{
		Paste();
		}
	else if (cmd == kDeleteSelCmd)
		{
		DeleteSelection();
		}

	else if (cmd == kSelectAllCmd)
		{
		EditSelectAll();
		}

	else if (cmd == kCleanRightMarginCmd)
		{
		JIndexRange range;
		CleanRightMargin(kJFalse, &range);
		}
	else if (cmd == kCoerceRightMarginCmd)
		{
		JIndexRange range;
		CleanRightMargin(kJTrue, &range);
		}
	else if (cmd == kShiftSelLeftCmd)
		{
		TabSelectionLeft();
		}
	else if (cmd == kShiftSelRightCmd)
		{
		TabSelectionRight();
		}
	else if (cmd == kForceShiftSelLeftCmd)
		{
		TabSelectionLeft(1, kJTrue);
		}

	else if (cmd == kToggleReadOnlyCmd)
		{
		const Type type = GetType();
		if (type == kFullEditor)
			{
			SetType(kSelectableText);
			}
		else if (type == kSelectableText)
			{
			SetType(kFullEditor);
			}
		// don't change kStaticText
		}
}

/******************************************************************************
 Edit menu index <-> cmd

 ******************************************************************************/

#define ClassName    JXTEBase16
#define IndexToCmdFn EditMenuIndexToCmd
#define CmdToIndexFn EditMenuCmdToIndex
#define MenuVar      itsEditMenu
#define CmdCount     kEditMenuItemCount
#define CmdIDList    kEditMenuItemInfo
#include <JXMenuItemIDUtil.th>
#undef ClassName
#undef IndexToCmdFn
#undef CmdToIndexFn
#undef MenuVar
#undef CmdCount
#undef CmdIDList

/******************************************************************************
 TEHasSearchText (virtual)

	Call this to provide a JXPSPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

JBoolean
JXTEBase16::TEHasSearchText()
	const
{
	return kJFalse;
}

/******************************************************************************
 SetPSPrinter

	Call this to provide a JXPSPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase16::SetPSPrinter
	(
	JXPSPrinter* p
	)
{
	if (itsPSPrinter != NULL)
		{
		StopListening(itsPSPrinter);
		}
	itsPSPrinter = p;
}

/******************************************************************************
 GetPSPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase16::GetPSPrintFileName()
	const
{
	if (itsPSPrintName == NULL)
		{
		JXTEBase16* me = const_cast<JXTEBase16*>(this);
		me->itsPSPrintName = new JString;
		assert( itsPSPrintName != NULL );
		}

	return *itsPSPrintName;
}

/******************************************************************************
 SetPSPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase16::SetPSPrintFileName
	(
	const JCharacter* fileName
	)
{
	GetPSPrintFileName();		// create itsPSPrintName
	*itsPSPrintName = fileName;
}

/******************************************************************************
 HandlePSPageSetup

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase16::HandlePSPageSetup()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPS

	You must call SetPSPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase16::PrintPS()
{
	assert( itsPSPrinter != NULL );

	itsPSPrinter->SetFileName(GetPSPrintFileName());
	itsPSPrinter->BeginUserPrintSetup();
	ListenTo(itsPSPrinter);
}

/******************************************************************************
 Print footer (virtual protected)

	Overrides of JTextEditor16 functions.

 ******************************************************************************/

JCoordinate
JXTEBase16::GetPrintFooterHeight
	(
	JPagePrinter& p
	)
	const
{
	return JRound(1.5 * p.GetLineHeight());
}

void
JXTEBase16::DrawPrintFooter
	(
	JPagePrinter&		p,
	const JCoordinate	footerHeight
	)
{
	JRect pageRect = p.GetPageRect();
	pageRect.top   = pageRect.bottom - footerHeight;

	JString pageNumberStr(p.GetPageIndex(), 0);
	pageNumberStr.Prepend("Page ");

	p.String(pageRect, pageNumberStr,
			 JPainter::kHAlignCenter, JPainter::kVAlignBottom);
}

/******************************************************************************
 SetPTPrinter

	Call this to provide a JXPTPrinter object for this text.  This object
	does *not* take ownership of the printer object.

 ******************************************************************************/

void
JXTEBase16::SetPTPrinter
	(
	JXPTPrinter* p
	)
{
	if (itsPTPrinter != NULL)
		{
		StopListening(itsPTPrinter);
		}
	itsPTPrinter = p;
}

/******************************************************************************
 GetPTPrintFileName

	Call this to get the file name used in the Print Setup dialog.

 ******************************************************************************/

const JString&
JXTEBase16::GetPTPrintFileName()
	const
{
	if (itsPTPrintName == NULL)
		{
		JXTEBase16* me = const_cast<JXTEBase16*>(this);
		me->itsPTPrintName = new JString;
		assert( itsPTPrintName != NULL );
		}

	return *itsPTPrintName;
}

/******************************************************************************
 SetPTPrintFileName

	Call this to set the file name used in the Print Setup dialog.

 ******************************************************************************/

void
JXTEBase16::SetPTPrintFileName
	(
	const JCharacter* fileName
	)
{
	GetPTPrintFileName();		// create itsPTPrintName
	*itsPTPrintName = fileName;
}

/******************************************************************************
 HandlePTPageSetup

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase16::HandlePTPageSetup()
{
	assert( itsPTPrinter != NULL );

	itsPTPrinter->BeginUserPageSetup();
}

/******************************************************************************
 PrintPT

	You must call SetPTPrinter() before using this routine.

 ******************************************************************************/

void
JXTEBase16::PrintPT()
{
	assert( itsPTPrinter != NULL );

	itsPTPrinter->SetFileName(GetPTPrintFileName());
	itsPTPrinter->BeginUserPrintSetup();
	ListenTo(itsPTPrinter);
}

JColorIndex	JXTEBase16::GetSelectionColor() const
{
	return GetColormap()->GetDefaultSelectionColor();
}

/******************************************************************************
 JXTEBase16::PartialWordModifier

 ******************************************************************************/

istream&
operator>>
	(
	istream&						input,
	JXTEBase16::PartialWordModifier&	pwMod
	)
{
	long temp;
	input >> temp;
	pwMod = (JXTEBase16::PartialWordModifier) temp;
	assert( pwMod == JXTEBase16::kCtrlMetaPWMod ||
			pwMod == JXTEBase16::kMod2PWMod || pwMod == JXTEBase16::kMod3PWMod ||
			pwMod == JXTEBase16::kMod4PWMod || pwMod == JXTEBase16::kMod5PWMod );
	return input;
}

ostream&
operator<<
	(
	ostream&							output,
	const JXTEBase16::PartialWordModifier	pwMod
	)
{
	output << (long) pwMod;
	return output;
}
