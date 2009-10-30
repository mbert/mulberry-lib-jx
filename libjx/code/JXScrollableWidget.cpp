/******************************************************************************
 JXScrollableWidget.cpp

	If a JXScrollbarSet is provided, this class will work with the scroll
	bars to allow derived classes to scroll automatically.  The default is to
	show only the scroll bars that are necessary.  Use AlwaysShowScrollbars()
	to change this behavior.

	A JXScrollbarSet is not required because sometimes one might want
	to use a derived class in a situation where scrolling is handled in
	some other way or where scrolling is not required or desired at all.

	ScrollForDrag can be called during a mouse drag to make the widget
	automatically scroll when the mouse is dragged outside the aperture.

	BASE CLASS = JXWidget

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXScrollableWidget.h>
#include <JXAdjustScrollbarTask.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXMenu.h>
#include <JXWindow.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXKeysym.h>
#include <jXGlobals.h>
#include <JMinMax.h>
#include <jMath.h>
#include <jASCIIConstants.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JFloat kSingleStepFraction = 0.1;
const JFloat kPageStepFraction   = 0.9;
const JInteger kWheelLineCount   = 5;		// * kSingleStepFraction = 1/2 page

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JCharacter kSetupDataEndDelimiter = '\2';		// avoid conflict with JXScrollbar

/******************************************************************************
 Constructor

	scrollbarSet can be NULL

 ******************************************************************************/

JXScrollableWidget::JXScrollableWidget
	(
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsScrollbarSet         = scrollbarSet;
	itsAlwaysShowScrollFlag = kJFalse;
	itsAdjustingFlag        = kJFalse;
	itsShouldRedrawFlag     = kJTrue;
	itsAdjustScrollbarTask  = NULL;

	itsHStepSize = itsVStepSize = 0;
	itsHPageStepContext = itsVPageStepContext = -1;

	itsHCtrl = kJTrue;
	itsHMeta = kJTrue;
	itsVCtrl = kJTrue;
	itsVMeta = kJFalse;

	UnlockBounds();
	TurnOnBufferedDrawing();

	if (itsScrollbarSet != NULL)
		{
		WantInput(kJTrue);		// get home, end, page up, page down

		ListenTo(itsScrollbarSet->GetHScrollbar());
		ListenTo(itsScrollbarSet->GetVScrollbar());

		NeedAdjustScrollbars();
		}

	SetBorderWidth(kJXDefaultBorderWidth);

	// changing border width doesn't move bounds

	if (itsScrollbarSet != NULL)
		{
		ScrollTo(0,0);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrollableWidget::~JXScrollableWidget()
{
	delete itsAdjustScrollbarTask;
}

/******************************************************************************
 GetScrollbars

 ******************************************************************************/

JBoolean
JXScrollableWidget::GetScrollbars
	(
	JXScrollbar** hScrollbar,
	JXScrollbar** vScrollbar
	)
	const
{
	if (itsScrollbarSet != NULL)
		{
		*hScrollbar = itsScrollbarSet->GetHScrollbar();
		*vScrollbar = itsScrollbarSet->GetVScrollbar();
		return kJTrue;
		}
	else
		{
		*hScrollbar = NULL;
		*vScrollbar = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXScrollableWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	const JSize w = GetBorderWidth();
	if (IsDNDTarget())
		{
		JXDrawDNDBorder(p, frame, w);
		}
	else if (w > 0)
		{
		JXDrawDownFrame(p, frame, w);
		}
}

/******************************************************************************
 ScrollForDrag (protected)

	Scrolls the widget to make the given point (in local coords) visible.
	Returns kJTrue if scrolling was actually necessary.

 ******************************************************************************/

JBoolean
JXScrollableWidget::ScrollForDrag
	(
	const JPoint& pt
	)
{
	const JPoint truePt = JPinInRect(pt, GetBounds());
	const JRect rect(truePt.y-1, truePt.x-1, truePt.y+1, truePt.x+1);
	if (ScrollToRect(rect))
		{
		Redraw();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXScrollableWidget::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXWidget::BoundsMoved(dx,dy);
	AdjustScrollbars();
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXScrollableWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	NeedAdjustScrollbars();		// block future AdjustScrollbars() calls
	JXWidget::BoundsResized(dw,dh);
}

/******************************************************************************
 ApertureMoved (virtual protected)

 ******************************************************************************/

void
JXScrollableWidget::ApertureMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXWidget::ApertureMoved(dx,dy);
	AdjustScrollbars();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXScrollableWidget::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	NeedAdjustScrollbars();		// block future AdjustScrollbars() calls
	JXWidget::ApertureResized(dw,dh);
}

/******************************************************************************
 Receive (virtual protected)

	Listen for adjustments in scrollbars.

 ******************************************************************************/

void
JXScrollableWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXScrollbar* hScrollbar = NULL;
	JXScrollbar* vScrollbar = NULL;
	if (itsScrollbarSet != NULL)
		{
		hScrollbar = itsScrollbarSet->GetHScrollbar();
		vScrollbar = itsScrollbarSet->GetVScrollbar();
		}

	if ((sender == hScrollbar || sender == vScrollbar) &&
		message.Is(JXScrollbar::kScrolled) && !itsAdjustingFlag)
		{
		ScrollTo(hScrollbar->GetValue(), vScrollbar->GetValue());
		if (itsShouldRedrawFlag)
			{
			Redraw();	// keep us up to date with scrollbar appearance
			}
		}
	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 NeedAdjustScrollbars (private)

	We can't call AdjustScrollbars() during a resize because it confuses
	everybody.  So we do it as soon as everything has settled down.

	As an example, derived classes must call inherited::ApertureResized()
	before doing anything else.  If we adjusted the scrollbars directly,
	the derived class would get a second ApertureResized() before the first
	one completed.

 ******************************************************************************/

void
JXScrollableWidget::NeedAdjustScrollbars()
{
	if (itsScrollbarSet != NULL && itsAdjustScrollbarTask == NULL)
		{
		itsAdjustScrollbarTask = new JXAdjustScrollbarTask(this);
		assert( itsAdjustScrollbarTask != NULL );
		(JXGetApplication())->InstallUrgentTask(itsAdjustScrollbarTask);
		}
}

/******************************************************************************
 UpdateScrollbars

	The scrollbars sometimes have to be adjusted via an urgent task, as
	explained for NeedAdjustScrollbars(), because the current call needs to
	be completed first.  This can be a pain because this means that our
	aperture is not guaranteed to be correct until the -event- has finished.

	This function is provided so that you can update the aperture immediately.
	However, you must not call it in code triggered by a change in the bounds
	or the aperture.

	We need a while loop because AdjustScrollbars() calls
	itsScrollbarSet->ShowScrollbars(), which can call BoundsResized(),
	which creates another urgent task.

 ******************************************************************************/

void
JXScrollableWidget::UpdateScrollbars()
{
	while (itsAdjustScrollbarTask != NULL)
		{
		delete itsAdjustScrollbarTask;
		itsAdjustScrollbarTask = NULL;
		AdjustScrollbars();
		}
}

/******************************************************************************
 AdjustScrollbars (private)

	We rely on the fact that JXWidget keeps Aperture inside Bounds.

 ******************************************************************************/

void
JXScrollableWidget::AdjustScrollbars()
{
	if (itsScrollbarSet == NULL || itsAdjustScrollbarTask != NULL)
		{
		return;
		}

	JXScrollbar* hScrollbar = itsScrollbarSet->GetHScrollbar();
	JXScrollbar* vScrollbar = itsScrollbarSet->GetVScrollbar();

	const JRect boundsG   = GetBoundsGlobal();
	const JRect ap        = GetAperture();
	const JCoordinate apW = ap.width();
	const JCoordinate apH = ap.height();

	// avoid hysteresis

	if (!itsAlwaysShowScrollFlag &&
		(hScrollbar->WouldBeVisible() || vScrollbar->WouldBeVisible()))
		{
		JCoordinate maxApWidth = ap.width();
		if (GetHSizing() == kHElastic && vScrollbar->WouldBeVisible())
			{
			maxApWidth += vScrollbar->GetFrameWidth();
			}

		JCoordinate maxApHeight = ap.height();
		if (GetVSizing() == kVElastic && hScrollbar->WouldBeVisible())
			{
			maxApHeight += hScrollbar->GetFrameHeight();
			}

		if (boundsG.width() <= maxApWidth && boundsG.height() <= maxApHeight)
			{
			itsScrollbarSet->ShowScrollbars(kJFalse, kJFalse);
			return;
			}
		}

	// adjust all the scrollbar settings

	itsAdjustingFlag = kJTrue;

	const JCoordinate xmax = JMax(boundsG.width() - apW, (JCoordinate) 0);
	hScrollbar->SetMaxValue(xmax);
	hScrollbar->SetValue(ap.left);
	JCoordinate hStep = itsHStepSize;
	if (hStep <= 0)
		{
		hStep = JRound(apW * kSingleStepFraction);
		}
	hScrollbar->SetStepSize(hStep);
	if (itsHPageStepContext >= 0 && apW - itsHPageStepContext >= hStep)
		{
		hScrollbar->SetPageStepSize(apW - itsHPageStepContext);
		}
	else
		{
		hScrollbar->SetPageStepSize(JRound(apW * kPageStepFraction));
		}

	const JCoordinate ymax = JMax(boundsG.height() - apH, (JCoordinate) 0);
	vScrollbar->SetMaxValue(ymax);
	vScrollbar->SetValue(ap.top);
	JCoordinate vStep = itsVStepSize;
	if (vStep <= 0)
		{
		vStep = JRound(apH * kSingleStepFraction);
		}
	vScrollbar->SetStepSize(vStep);
	if (itsVPageStepContext >= 0 && apH - itsVPageStepContext >= vStep)
		{
		vScrollbar->SetPageStepSize(apH - itsVPageStepContext);
		}
	else
		{
		vScrollbar->SetPageStepSize(JRound(apH * kPageStepFraction));
		}

	itsAdjustingFlag = kJFalse;

	itsScrollbarSet->ShowScrollbars(
						JConvertToBoolean(itsAlwaysShowScrollFlag || xmax > 0),
						JConvertToBoolean(itsAlwaysShowScrollFlag || ymax > 0));
}

/******************************************************************************
 HandleMouseDown (virtual protected)

	The default is to support scrolling via a wheel mouse.

 ******************************************************************************/

void
JXScrollableWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ScrollForWheel(button, modifiers);
}

/******************************************************************************
 ScrollForWheel (protected)

	Call this to support scrolling via a wheel mouse.  Returns kJTrue if
	it handled the event.

	The second version is designed for row and column headers that scroll
	but don't have a JXScrollbarSet.  The function therefore accepts NULL
	for either scrollbar object.

 ******************************************************************************/

JBoolean
JXScrollableWidget::ScrollForWheel
	(
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsScrollbarSet != NULL)
		{
		return ScrollForWheel(button, modifiers,
							  itsScrollbarSet->GetHScrollbar(),
							  itsScrollbarSet->GetVScrollbar());
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JXScrollableWidget::ScrollForWheel
	(
	const JXMouseButton		button,
	const JXKeyModifiers&	modifiers,
	JXScrollbar*			hScrollbar,
	JXScrollbar*			vScrollbar
	)
{
	JXScrollbar* scrollbar = modifiers.meta() ? hScrollbar : vScrollbar;
	if (scrollbar == NULL)
		{
		return kJFalse;
		}

	JCoordinate delta;
	if (button == kJXButton4)
		{
		delta = -1;
		}
	else if (button == kJXButton5)
		{
		delta = +1;
		}
	else
		{
		return kJFalse;
		}

#ifdef _J_OSX

	if (modifiers.control())
		{
		scrollbar->StepLine(kWheelLineCount * delta);
		}
	else
		{
		scrollbar->StepLine(delta);
		}

#else

	if (modifiers.shift())
		{
		scrollbar->StepLine(delta);
		}
	else if (modifiers.control())
		{
		scrollbar->StepPage(delta);
		}
	else
		{
		scrollbar->StepLine(kWheelLineCount * delta);
		}

#endif

	return kJTrue;
}

/******************************************************************************
 HandleKeyPress (virtual)

	Call this if you want to support Home, End, Page up, Page down keys.

 ******************************************************************************/

void
JXScrollableWidget::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JXScrollbar* hScrollbar = NULL;
	JXScrollbar* vScrollbar = NULL;
	if (itsScrollbarSet != NULL)
		{
		hScrollbar = itsScrollbarSet->GetHScrollbar();
		vScrollbar = itsScrollbarSet->GetVScrollbar();
		}

	const JXMenu::Style style = JXMenu::GetDisplayStyle();
	const JBoolean ctrl =
		(style == JXMenu::kMacintoshStyle ? modifiers.control() : modifiers.meta());
	const JBoolean meta =
		(style == JXMenu::kMacintoshStyle ? modifiers.meta() : modifiers.control());

	JXScrollbar* scrollbar = modifiers.meta() ? hScrollbar : vScrollbar;

	if (itsScrollbarSet != NULL &&
		(key == XK_Home || key == XK_KP_Home))
		{
		ScrollTo(0,0);
		}
	else if (itsScrollbarSet != NULL &&
			 (key == XK_End || key == XK_KP_End))
		{
		scrollbar->ScrollToMax();
		}
	else if (itsScrollbarSet != NULL &&
			 (key == XK_Page_Up || key == XK_KP_Page_Up))
		{
		scrollbar->StepPage(modifiers.shift() ? -0.5 : -1.0);
		}
	else if (itsScrollbarSet != NULL &&
			 (key == XK_Page_Down || key == XK_KP_Page_Down))
		{
		scrollbar->StepPage(modifiers.shift() ? +0.5 : +1.0);
		}

	else if (itsScrollbarSet != NULL && key == kJLeftArrow)
		{
		hScrollbar->StepLine(-1);
		}
	else if (itsScrollbarSet != NULL && key == kJRightArrow)
		{
		hScrollbar->StepLine(+1);
		}
	else if (itsScrollbarSet != NULL && key == kJUpArrow)
		{
		vScrollbar->StepLine(-1);
		}
	else if (itsScrollbarSet != NULL && key == kJDownArrow)
		{
		vScrollbar->StepLine(+1);
		}

	// scroll to tab

	else if (itsScrollbarSet != NULL &&
			 '1' <= key && key <= '9' &&
			 ctrl == itsVCtrl &&
			 meta == itsVMeta)
		{
		vScrollbar->ScrollToTab(key - '0');
		}
	else if (itsScrollbarSet != NULL &&
			 '1' <= key && key <= '9' &&
			 ctrl == itsHCtrl &&
			 meta == itsHMeta)
		{
		hScrollbar->ScrollToTab(key - '0');
		}

	else
		{
		JXWidget::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 ReadScrollSetup

 ******************************************************************************/

void
JXScrollableWidget::ReadScrollSetup
	(
	istream& input
	)
{
	UpdateScrollbars();		// make sure scrollbars can accept the given values

	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		JBoolean hadScrollbars;
		input >> hadScrollbars;

		JXScrollbar *hScrollbar, *vScrollbar;
		const JBoolean hasScrollbars = GetScrollbars(&hScrollbar, &vScrollbar);
		assert( hasScrollbars == hadScrollbars );

		if (hasScrollbars)
			{
			// Redraw() is unnecessary in this case, and it causes problems
			// for JXTreeListWidget when the nodes are sorted while the
			// widget is scrolled to the very bottom.

			itsShouldRedrawFlag = kJFalse;
			hScrollbar->ReadSetup(input);
			vScrollbar->ReadSetup(input);
			itsShouldRedrawFlag = kJTrue;
			}
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 SkipScrollSetup (static)

 ******************************************************************************/

void
JXScrollableWidget::SkipScrollSetup
	(
	istream& input
	)
{
	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteScrollSetup

 ******************************************************************************/

void
JXScrollableWidget::WriteScrollSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	JXScrollbar *hScrollbar, *vScrollbar;
	const JBoolean hasScrollbars = GetScrollbars(&hScrollbar, &vScrollbar);
	output << ' ' << hasScrollbars;

	if (hasScrollbars)
		{
		output << ' ';
		hScrollbar->WriteSetup(output);
		output << ' ';
		vScrollbar->WriteSetup(output);
		}

	output << kSetupDataEndDelimiter;
}
