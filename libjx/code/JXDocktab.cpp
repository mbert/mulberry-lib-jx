/******************************************************************************
 JXDocktab.cpp

	Sits to the left of a menu bar and allows the window to be docked by
	dragging to a dock.  Also provides a menu of other, useful options.

	BASE CLASS = JXWidget

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDocktab.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXDockDragData.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <JXColormap.h>
#include <jAssert.h>

// Action menu

static const JCharacter* kActionMenuStr =
	"    Undock this window"
	"  | Undock all windows in this compartment"
	"  | Undock all windows in this dock"
	"  | Undock all windows"
	"%l| UpdateWindowTypeMap"
	"%l| Create new dock (split horizontally)"
	"  | Create new dock (split vertically)"
	"%l| Dock all windows in default configuration"
	"%l";

enum
{
	kUndockCmd = 1,
	kUndockAllCompartmentCmd,
	kUndockAllDockCmd,
	kUndockAllCmd,
	kUpdateWindowTypeMapCmd,
	kCreateDockHorizCmd,
	kCreateDockVertCmd,
	kDockAllDefConfigCmd,
	kShowFirstDockCmd
};

static const JCharacter* kAddToWindowTypeMapStr      = "Automatically dock new windows of this type in this pane";
static const JCharacter* kRemoveFromWindowTypeMapStr = "Do not automatically dock new windows of this type";

// string ID's

static const JCharacter* kHintID           = "Hint::JXDocktab";
static const JCharacter* kShowDockPrefixID = "ShowDockPrefix::JXDocktab";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocktab::JXDocktab
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsActionMenu(NULL),
	itsDockFinder(NULL),
	itsClosePushedFlag(kJFalse)
{
	SetHint(JGetString(kHintID));

	if (!(GetColormap())->AllocateStaticColor(0, 0, kJMaxRGBValue/2, &itsFocusColor))
		{
		itsFocusColor = (GetColormap())->GetBlueColor();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocktab::~JXDocktab()
{
	delete itsDockFinder;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXDocktab::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JColormap* cmap = p.GetColormap();

	// close button

	p.SetPenColor(cmap->GetBlackColor());

	JRect b;
	if ((GetWindow())->GetCloseAction() != JXWindow::kDeactivateDirector)
		{
		b.Set(0, 0, kWidth, kWidth);
		p.Line(b.left, b.top,      b.right-1, b.bottom-1);
		p.Line(b.left, b.bottom-1, b.right-1, b.top);
		if (itsClosePushedFlag)
			{
			JXDrawDownFrame(p, b, 2);
			}
		else
			{
			JXDrawUpFrame(p, b, 2);
			}
		}

	// drag region

	JXWindow* w = GetWindow();
	JXWindow* focusWindow;

	JXDockWidget* dock;
	if (w->GetDockWidget(&dock) &&
		(dock->GetDockDirector())->GetFocusWindow(&focusWindow) &&
		focusWindow == w)
		{
		JRect r = rect;
		r.top   = b.bottom;

		p.SetPenColor(itsFocusColor);
		p.SetFilling(kJTrue);
		p.JPainter::Rect(r);
		p.SetPenColor((GetColormap())->GetBlackColor());
		p.SetFilling(kJFalse);

		p.SetPenColor(cmap->GetWhiteColor());
		}
	else
		{
		p.SetPenColor(cmap->GetGray60Color());
		}

	const JRect ap       = GetAperture();
	const JCoordinate y1 = ap.top + b.height() + 1;
	const JCoordinate y2 = ap.bottom - 2;

	p.Line(3, y1, 3, y2);
	p.Line(5, y1, 5, y2);

	itsCloseRect = b;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXDocktab::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXDocktab::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && itsCloseRect.Contains(pt))
		{
		itsClosePushedFlag = kJTrue;
		Refresh();
		}
	else if (button == kJXLeftButton)
		{
		if (itsDockFinder == NULL)
			{
			itsDockFinder = new DockFinder(GetDisplay());
			assert( itsDockFinder != NULL );
			}

		JXDockDragData* data = new JXDockDragData(GetWindow());
		assert( data != NULL );

		BeginDND(pt, buttonStates, modifiers, data, itsDockFinder);
		}
	else if (button == kJXRightButton)
		{
		OpenActionMenu(pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXDocktab::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (buttonStates.left())
		{
		const JBoolean inside = itsCloseRect.Contains(pt);
		if (inside && !itsClosePushedFlag)
			{
			itsClosePushedFlag = kJTrue;
			Redraw();
			}
		else if (!inside && itsClosePushedFlag)
			{
			itsClosePushedFlag = kJFalse;
			Redraw();
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXDocktab::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && itsClosePushedFlag)
		{
		itsClosePushedFlag = kJFalse;
		Refresh();

		if (itsCloseRect.Contains(pt))
			{
			(GetWindow())->Close();		// must be last since it will probably delete us
			}
		}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
JXDocktab::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return (GetDNDManager())->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 OpenActionMenu (private)

 ******************************************************************************/

void
JXDocktab::OpenActionMenu
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsActionMenu == NULL)
		{
		itsActionMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsActionMenu != NULL );
		itsActionMenu->SetToHiddenPopupMenu();
		itsActionMenu->SetMenuItems(kActionMenuStr);
		itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsActionMenu);
		}

	itsActionMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDocktab::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionMenu();
		}
	else if (sender == itsActionMenu && message.Is(JXTextMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleActionMenu(selection->GetIndex());
		}

	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateActionMenu (private)

 ******************************************************************************/

void
JXDocktab::UpdateActionMenu()
{
	JXDockManager* dockMgr;
	if (!JXGetDockManager(&dockMgr))
		{
		itsActionMenu->RemoveAllItems();
		return;
		}

	while (itsActionMenu->GetItemCount() >= kShowFirstDockCmd)
		{
		itsActionMenu->RemoveItem(itsActionMenu->GetItemCount());
		}

	JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();

	const JSize dockCount = dockList->GetElementCount();
	for (JIndex i=1; i<=dockCount; i++)
		{
		JString itemText = ((dockList->NthElement(i))->GetWindow())->GetTitle();
		itemText.Prepend(JGetString(kShowDockPrefixID));
		itsActionMenu->AppendItem(itemText);
		}

	const JBoolean isDocked = (GetWindow())->IsDocked();
	itsActionMenu->SetItemEnable(kUndockCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllCompartmentCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllDockCmd, isDocked);
	itsActionMenu->SetItemEnable(kUndockAllCmd, JI2B(dockCount > 0));
	itsActionMenu->SetItemEnable(kDockAllDefConfigCmd, dockMgr->CanDockAll());

	itsActionMenu->SetItemEnable(kUpdateWindowTypeMapCmd, (GetWindow())->HasWindowType());
	if (isDocked)
		{
		itsActionMenu->SetItemText(kUpdateWindowTypeMapCmd, kAddToWindowTypeMapStr);
		}
	else
		{
		itsActionMenu->SetItemText(kUpdateWindowTypeMapCmd, kRemoveFromWindowTypeMapStr);
		}
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
JXDocktab::HandleActionMenu
	(
	const JIndex index
	)
{
	JXDockManager* dockMgr;
	if (!JXGetDockManager(&dockMgr))
		{
		return;
		}

	if (index == kUndockCmd)
		{
		(GetWindow())->Undock();
		}
	else if (index == kUndockAllCompartmentCmd)
		{
		JXDockWidget* dock;
		if ((GetWindow())->GetDockWidget(&dock))
			{
			dock->UndockAll();
			}
		}
	else if (index == kUndockAllDockCmd)
		{
		JXWindow* w;
		if ((GetWindow())->GetDockWindow(&w))
			{
			w->UndockAllChildWindows();
			}
		}
	else if (index == kUndockAllCmd)
		{
		(GetDisplay())->UndockAllWindows();
		}

	else if (index == kUpdateWindowTypeMapCmd)
		{
		JXWindow* w = GetWindow();
		const JCharacter* type;
		if (w->GetWindowType(&type))
			{
			JXDockWidget* dock;
			w->GetDockWidget(&dock);
			(JXGetDockManager())->SetDefaultDock(type, w->IsDocked() ? dock : NULL);
			}
		}

	else if (index == kCreateDockHorizCmd)
		{
		dockMgr->CreateDock(kJTrue);
		}
	else if (index == kCreateDockVertCmd)
		{
		dockMgr->CreateDock(kJFalse);
		}
	else if (index == kDockAllDefConfigCmd)
		{
		dockMgr->DockAll();
		}

	else if (index >= kShowFirstDockCmd)
		{
		JPtrArray<JXDockDirector>* dockList = dockMgr->GetDockList();
		const JIndex i                      = index - kShowFirstDockCmd + 1;
		if (dockList->IndexValid(i))		// paranoia
			{
			(dockList->NthElement(i))->Activate();
			}
		}
}

/******************************************************************************
 JXDocktab::DockFinder

 ******************************************************************************/

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocktab::DockFinder::DockFinder
	(
	JXDisplay* display
	)
	:
	TargetFinder(display)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocktab::DockFinder::~DockFinder()
{
}

/******************************************************************************
 FindTarget (virtual)

 ******************************************************************************/

JBoolean
JXDocktab::DockFinder::FindTarget
	(
	const JXContainer*	coordOwner,
	const JPoint&		pt,
	Window*				xWindow,
	Window*				msgWindow,
	JXContainer**		target,
	Atom*				vers
	)
{
	if ((GetDisplay())->FindMouseContainer(coordOwner, pt, target, xWindow))
		{
		JXDockWidget* dock;
		if (((**target).GetWindow())->GetDockWidget(&dock))
			{
			*target = dock;
			}
		return (GetDNDManager())->IsDNDAware(*xWindow, msgWindow, vers);
		}
	else
		{
		*target    = NULL;
		*msgWindow = *xWindow;
		*vers      = 0;
		return kJFalse;
		}
}
