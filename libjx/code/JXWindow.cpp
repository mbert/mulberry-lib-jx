/******************************************************************************
 JXWindow.cpp

	If we ever allow a constructor that takes a JXDisplay*, we have to
	rewrite JXGetCurrFontMgr.

	BASE CLASS = JXContainer

	Copyright  1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWindow.h>
#include <JXWindowDirector.h>
#include <JXMenuManager.h>
#include <JXDNDManager.h>
#include <JXHintManager.h>
#include <JXWidget.h>
#include <JXTextMenu.h>
#include <JXDisplay.h>
#include <JXGC.h>
#include <JXColormap.h>
#include <JXWindowPainter.h>
#include <JXIconDirector.h>
#include <JXWindowIcon.h>
#include <JXImageMask.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXDockWidget.h>
#include <JXDockWindowTask.h>
#include <jXEventUtil.h>
#include <jXUtil.h>
#include <jXGlobals.h>

//#define THREAD_TEST

#ifdef THREAD_TEST
#include <ace/Thread.h>
#endif

#include <X11/Xatom.h>
#include <jXKeysym.h>

#include <JString.h>
#include <JString16.h>
#include <jASCIIConstants.h>
#include <JMinMax.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <jMath.h>
#include <jTime.h>
#include <sstream>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <jAssert.h>

#define KEYSYM2UCS

#ifdef KEYSYM2UCS
#include <keysym2ucs.h>
#endif

JBoolean JXWindow::theFoundWMFrameMethodFlag = kJFalse;
JBoolean JXWindow::theWMFrameCompensateFlag  = kJFalse;
const JCoordinate kWMFrameSlop               = 2;	// pixels
const JCharacter* JXWindow::theDefaultWMCommand = "jxapp";

static const JCharacter* kWMOffsetFileName = "~/.jx/wm_offset";
JBoolean JXWindow::theWMOffsetInitFlag     = kJFalse;
JPoint JXWindow::theWMOffset;

JBoolean JXWindow::theAutoDockNewWindowFlag = kJTrue;

// JXSelectionManager needs PropertyNotify

const unsigned int kEventMask =
	FocusChangeMask | KeyPressMask |
	ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask | PointerMotionHintMask |
	EnterWindowMask | LeaveWindowMask |
	ExposureMask | StructureNotifyMask | SubstructureNotifyMask |
	PropertyChangeMask;

const unsigned int kPointerGrabMask =
	ButtonPressMask | ButtonReleaseMask |
	PointerMotionMask | PointerMotionHintMask |
	PropertyChangeMask;

const JFileVersion kCurrentGeometryDataVersion = 1;
const JCharacter kGeometryDataEndDelimiter     = '\1';

	// version  1 saves dock ID

// JBroadcaster message types

const JCharacter* JXWindow::kIconified      = "Iconified::JXWindow";
const JCharacter* JXWindow::kDeiconified    = "Deiconified::JXWindow";
const JCharacter* JXWindow::kDocked         = "Docked::JXWindow";
const JCharacter* JXWindow::kUndocked       = "Undocked::JXWindow";
const JCharacter* JXWindow::kMinSizeChanged = "MinSizeChanged::JXWindow";
const JCharacter* JXWindow::kTitleChanged   = "TitleChanged::JXWindow";

/******************************************************************************
 Constructor

	colormap can be NULL.

 ******************************************************************************/

JXWindow::JXWindow
	(
	JXWindowDirector*	director,
	const JCoordinate	w,
	const JCoordinate	h,
	const JCharacter*	title,
	const JBoolean		ownsColormap,
	JXColormap*			colormap,
	const JBoolean		isMenu
	)
	:
	JXContainer((JXGetApplication())->GetCurrentDisplay(), this, NULL),
	itsMainWindow(NULL),
	itsTitle(title),
	itsDesktopLoc(0,0),
	itsBounds(0, 0, h, w),
	itsWMFrameLoc(0,0),
	itsIsDestructingFlag(kJFalse),
	itsHasMinSizeFlag(kJFalse),
	itsHasMaxSizeFlag(kJFalse),
	itsIsMenuFlag(isMenu),
	itsFirstClick(kJXNoButton, 0, JPoint(-1,-1)),
	itsSecondClick(kJXNoButton, 0, JPoint(-1,-1)),
	itsIsDockedFlag(kJFalse),
	itsDockXWindow(None),
	itsDockWidget(NULL),
	itsChildWindowList(NULL)
{
	assert( director != NULL );
	itsDirector = director;

	AdjustTitle();

	itsFocusWhenShowFlag    = kJFalse;
	itsBufferPixmap         = None;
	itsBufferDrawingFlag    = isMenu;
	itsKeepBufferPixmapFlag = isMenu;
	itsUseBkgdPixmapFlag    = isMenu;
	itsCursorIndex          = kJXDefaultCursor;
	itsUpdating				= kJFalse;
	itsUpdateRegion         = XCreateRegion();
	itsIcon                 = NULL;
	itsIconDir              = NULL;

	itsShortcuts = new JArray<Shortcut>;
	assert( itsShortcuts != NULL );
	itsShortcuts->SetCompareFunction(CompareShortcuts);

	itsMouseContainer      = NULL;
	itsIsDraggingFlag      = kJFalse;
	itsProcessDragFlag     = kJFalse;
	itsCursorLeftFlag      = kJFalse;
	itsCleanAfterBlockFlag = kJFalse;

	itsButtonPressReceiver = this;
	itsPointerGrabbedFlag  = kJFalse;
	itsBPRChangedFlag      = kJFalse;

	itsFocusList = new JPtrArray<JXWidget>(JPtrArrayT::kForgetAll);
	assert( itsFocusList != NULL );

	itsFocusWidget = NULL;
	itsCurrHintMgr = NULL;

	itsPrevMouseContainer = NULL;
	itsClickCount         = 1;

	// get display for this window

	itsDisplay = (JXGetApplication())->GetCurrentDisplay();

	// get colormap for this window

	if (colormap != NULL)
		{
		assert( itsDisplay == colormap->GetDisplay() );

		itsColormap         = colormap;
		itsOwnsColormapFlag = ownsColormap;
		}
	else
		{
		itsColormap         = itsDisplay->GetColormap();
		itsOwnsColormapFlag = kJFalse;
		}

	itsBackColor = itsColormap->GetDefaultBackColor();

	// create window

	const unsigned long valueMask =
		CWBackPixel | CWBorderPixel | CWColormap | CWCursor |
		CWSaveUnder | CWOverrideRedirect | CWEventMask;

	XSetWindowAttributes attr;
	attr.background_pixel  = itsColormap->GetXPixel(itsBackColor);
	attr.border_pixel      = itsColormap->GetXPixel(itsColormap->GetBlackColor());
	attr.colormap          = *itsColormap;
	attr.cursor            = itsDisplay->GetXCursorID(itsCursorIndex);
	attr.save_under        = itsIsMenuFlag;
	attr.override_redirect = itsIsMenuFlag;
	attr.event_mask        = kEventMask;

	itsXWindow =
		XCreateWindow(*itsDisplay, itsDisplay->GetRootWindow(), 0,0, w,h,
					  0, CopyFromParent, InputOutput, itsColormap->GetVisual(),
					  valueMask, &attr);

	// set window properties

	const JCharacter* t = itsTitle.GetCString();

	XTextProperty windowName;
	const int ok = XStringListToTextProperty((char**) &t, 1, &windowName);
	assert( ok );

	XSizeHints sizeHints;
	sizeHints.flags  = PSize;
	sizeHints.width  = w;
	sizeHints.height = h;

	XWMHints wmHints;
	wmHints.flags         = StateHint | InputHint;
	wmHints.initial_state = NormalState;
	wmHints.input         = True;

	const JCharacter* argv = theDefaultWMCommand;
	XSetWMProperties(*itsDisplay, itsXWindow, &windowName, &windowName,
					 (char**) &argv, 1, &sizeHints, &wmHints, NULL);

	XFree(windowName.value);

	// create JXDNDAware property for Drag-And-Drop

	(GetDNDManager())->EnableDND(itsXWindow);

	// we need to listen for map/unmap, iconify/deiconify, focus in/out

	itsIsMappedFlag    = kJFalse;
	itsIsIconifiedFlag = kJFalse;
	itsHasFocusFlag    = kJFalse;

	// trap window manager's delete message

	itsCloseAction = kCloseDirector;
	AcceptSaveYourself(kJFalse);

	// create GC to use when drawing

	itsGC = new JXGC(itsDisplay, itsColormap, itsXWindow);
	assert( itsGC != NULL );

	// init wm offset
	// Ideally, we would use ConfigureNotify::border_width, but
	// this information isn't available early enough.

	if (!theWMOffsetInitFlag)
		{
		theWMOffsetInitFlag = kJTrue;

		JString offsetName;
		if (JExpandHomeDirShortcut(kWMOffsetFileName, &offsetName))
			{
			ifstream offsetInput(offsetName);
			JPoint offset;
			offsetInput >> offset;
			if (offsetInput.good())
				{
				theWMOffset = offset;
				}
			}
		}

	// notify the display that we exist

	itsDisplay->WindowCreated(this, itsXWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWindow::~JXWindow()
{
	itsIsDestructingFlag = kJTrue;

	DeleteEnclosedObjects();	// widgets talk to us when deleted

	itsDisplay->WindowDeleted(this);

	if (itsIsDockedFlag && itsDockWidget != NULL)
		{
		(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
		}

	delete itsIcon;
	// itsIconDir deleted by itsDirector
	delete itsShortcuts;
	delete itsFocusList;
	delete itsChildWindowList;

	if (itsBufferPixmap != None)
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		}

	if (itsOwnsColormapFlag)
		{
		delete itsColormap;
		}

	delete itsGC;
	XDestroyRegion(itsUpdateRegion);
	if (itsXWindow != None)				// plug-in: might be destroyed by Netscape
		{
		XDestroyWindow(*itsDisplay, itsXWindow);
		}
	itsDisplay->Flush();
}

/******************************************************************************
 SetTitle

 ******************************************************************************/

void
JXWindow::SetTitle
	(
	const JCharacter* title
	)
{
	itsTitle = title;
	AdjustTitle();

	const JCharacter* t = itsTitle.GetCString();

	XTextProperty windowName;
	const int ok = XStringListToTextProperty((char**) &t, 1, &windowName);
	assert( ok );

	XSetWMName(*itsDisplay, itsXWindow, &windowName);
	XSetWMIconName(*itsDisplay, itsXWindow, &windowName);

	XFree(windowName.value);

	Broadcast(TitleChanged());
}

/******************************************************************************
 AdjustTitle (private)

 ******************************************************************************/

void
JXWindow::AdjustTitle()
{
	// avoid loony feature of fvwm

	if (itsTitle.EndsWith("lock"))
		{
		itsTitle.AppendCharacter(' ');
		}
}

/******************************************************************************
 SetTransientFor

	Provide information to the window manager about which window we
	are subsidiary to.

 ******************************************************************************/

void
JXWindow::SetTransientFor
	(
	const JXWindowDirector* director
	)
{
	XSetTransientForHint(*itsDisplay, itsXWindow,
						 (director->GetWindow())->itsXWindow);
}

/******************************************************************************
 SetWMClass

	Provide information to the window manager that can be used to identify
	the window's type.  This is useful when excluding certain types of
	windows from a task bar.

	Call this before SetIcon() to avoid losing the icon's mask.

 ******************************************************************************/

void
JXWindow::SetWMClass
	(
	const JCharacter* c_class,
	const JCharacter* instance
	)
{
	XClassHint hint = { const_cast<char*>(instance), const_cast<char*>(c_class) };
	XSetClassHint(*itsDisplay, itsXWindow, &hint);

	itsWindowType = instance;

	JXDockManager* mgr;
	JXDockWidget* dock;
	if (theAutoDockNewWindowFlag &&
		JXGetDockManager(&mgr) && mgr->GetDefaultDock(itsWindowType, &dock))
		{
		dock->Dock(this);
		}
}

/******************************************************************************
 HideFromTaskbar

	Exclude this window from a task bar.

	Call this before SetIcon() to avoid losing the icon's mask.

 ******************************************************************************/

void
JXWindow::HideFromTaskbar()
{
	SetWMClass("Miscellaneous", JXGetInvisibleWindowClass());
}

/******************************************************************************
 ColormapChanged

 ******************************************************************************/

void
JXWindow::ColormapChanged
	(
	JXColormap* colormap
	)
{
	XSetWindowColormap(*itsDisplay, itsXWindow, *colormap);
	Refresh();	// need to redraw to update pixel values
}

/******************************************************************************
 DispatchMouse

 ******************************************************************************/

void
JXWindow::DispatchMouse()
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	JXWindow* window;
	if (IsVisible() &&
		XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		itsDisplay->GetMouseContainer(&window) &&
		window == this)
		{
		XMotionEvent xEvent;
		xEvent.type      = MotionNotify;
		xEvent.display   = *itsDisplay;
		xEvent.window    = itsXWindow;
		xEvent.root      = rootWindow;
		xEvent.subwindow = None;
		xEvent.x         = x;
		xEvent.y         = y;
		xEvent.x_root    = root_x;
		xEvent.y_root    = root_y;
		xEvent.state     = state;

		HandleMotionNotify(xEvent);
		}
}

/******************************************************************************
 DispatchCursor

 ******************************************************************************/

void
JXWindow::DispatchCursor()
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	JXWindow* window;
	if (IsVisible() &&
		XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		itsDisplay->GetMouseContainer(&window) &&
		window == this)
		{
		itsButtonPressReceiver->
			DispatchCursor(JPoint(x,y), JXKeyModifiers(itsDisplay, state));
		}
}

/******************************************************************************
 DisplayXCursor

 ******************************************************************************/

void
JXWindow::DisplayXCursor
	(
	const JCursorIndex index
	)
{
	if (itsCursorIndex != index)
		{
		itsCursorIndex = index;
		XDefineCursor(*itsDisplay, itsXWindow, itsDisplay->GetXCursorID(index));
		}
}

/******************************************************************************
 Close

 ******************************************************************************/

JBoolean
JXWindow::Close()
{
	if (itsCloseAction == kDeactivateDirector)
		{
		return (itsIsDockedFlag ? kJTrue : itsDirector->Deactivate());
		}
	else if (itsCloseAction == kCloseDirector)
		{
		return itsDirector->Close();
		}
	else if (itsCloseAction == kCloseDisplay)
		{
		return itsDisplay->Close();
		}
	else
		{
		assert( itsCloseAction == kQuitApp );
		(JXGetApplication())->Quit();
		return kJTrue;
		}
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JXWindow::Activate()
{
	if (!IsActive())
		{
		JXContainer::Activate();
		if (IsDocked())
			{
			Raise();
			}
		if (itsFocusWidget == NULL)
			{
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 Show (virtual)

 ******************************************************************************/

void
JXWindow::Show()
{
	if (!IsVisible())
		{
		JXContainer::Show();
		if (itsFocusWidget == NULL)
			{
			SwitchFocus(kJFalse);
			}
		if (itsUseBkgdPixmapFlag)
			{
			Redraw();
			}
		XMapWindow(*itsDisplay, itsXWindow);
		// input focus set by HandleMapNotify()

		JXWindow* parent;
		if (itsIsDockedFlag && GetDockWindow(&parent))
			{
			parent->Show();
			}
		}
}

/******************************************************************************
 Hide (virtual)

 ******************************************************************************/

void
JXWindow::Hide()
{
	if (IsVisible() && !IsDocked())
		{
		// We have to deiconify the window first because otherwise,
		// the icon won't disappear, at least under fvwm and fvwm2.

		const JBoolean wasIconified = itsIsIconifiedFlag;
		if (itsIsIconifiedFlag)
			{
			Deiconify();

			// toss the MapNotify event -- avoids error from XSetInputFocus()

			XEvent xEvent;
			XIfEvent(*itsDisplay, &xEvent, GetNextMapNotifyEvent, NULL);
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			}
		XUnmapWindow(*itsDisplay, itsXWindow);
		itsIsMappedFlag = kJFalse;
		JXContainer::Hide();

// We don't do this because it ought to be deiconified when next shown,
// but we don't want to do it in Show() because windows should stay
// iconified during program startup.
//		if (wasIconified)
//			{
//			Iconify();
//			}
		}
}

// static private

Bool
JXWindow::GetNextMapNotifyEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	if (event->type == MapNotify)
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 Raise

	Make this the top window.

 ******************************************************************************/

void
JXWindow::Raise
	(
	const JBoolean grabKeyboardFocus
	)
{
	const JBoolean wasVisible = IsVisible();
	const JBoolean wasIcon    = itsIsIconifiedFlag;

	Show();
	Activate();		// in case it wasn't already

//	Attempt to move window to current desktop on Gnome:
//	Only works if window is iconfied on its current desktop
//		XUnmapWindow(*itsDisplay, itsXWindow);
//		XMapWindow(*itsDisplay, itsXWindow);

	if (itsIsIconifiedFlag)
		{
		Deiconify();
		}
	else if (grabKeyboardFocus)
		{
		RequestFocus();
		}

	if (!(wasVisible && !itsIsMappedFlag) || wasIcon)
		{
		// Don't call Place() if window director is being activated
		// a second time before XMapWindow event arrives.

		// By calling Place() we ensure that the window is visible on
		// the current virtual desktop.

		Place(itsWMFrameLoc.x, itsWMFrameLoc.y);
		}

	XRaiseWindow(*itsDisplay, itsXWindow);

	JXWindow* parent;
	if (itsIsDockedFlag && GetDockWindow(&parent))
		{
		parent->Raise();
		}
}

/******************************************************************************
 Lower

	Make this the bottom window.

 ******************************************************************************/

void
JXWindow::Lower()
{
	XLowerWindow(*itsDisplay, itsXWindow);
}

/******************************************************************************
 RequestFocus

	Ask X for keyboard input focus.  This uses XSetInputFocus() rather
	than XGrabKeyboard().

 ******************************************************************************/

void
JXWindow::RequestFocus()
{
	if (itsIsMappedFlag)
		{
		XSetInputFocus(*itsDisplay, itsXWindow, RevertToPointerRoot, CurrentTime);
		}
}

/******************************************************************************
 Iconify

 ******************************************************************************/

void
JXWindow::Iconify()
{
	if (IsDocked())
		{
		return;
		}

	// change the initial_state hint

	SetWindowStateHint(IconicState);

	// take other necessary actions

	if (IsVisible())
		{
		// We don't modify itsIsIconifiedFlag here because the procedure might
		// fail.  We wait for an UnmapNotify event.

		XIconifyWindow(*itsDisplay, itsXWindow, itsDisplay->GetScreen());
		}
	else if (!itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJTrue;
		Broadcast(Iconified());
		}
}

/******************************************************************************
 Deiconify

 ******************************************************************************/

void
JXWindow::Deiconify()
{
	// change the initial_state hint

	SetWindowStateHint(NormalState);

	// take other necessary actions

	if (IsVisible())
		{
		// We don't modify itsIsIconifiedFlag here because we already deal
		// with it in HandleMapNotify().

		XMapWindow(*itsDisplay, itsXWindow);
		}
	else if (itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJFalse;
		Broadcast(Deiconified());
		}
}

/******************************************************************************
 SetWindowStateHint (private)

	Sets the initial_state hint.

 ******************************************************************************/

void
JXWindow::SetWindowStateHint
	(
	const int initial_state
	)
{
	XWMHints wmHints;

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != NULL)
		{
		wmHints = *origHints;
		XFree(origHints);
		}
	else
		{
		wmHints.flags = 0;
		}

	wmHints.flags        |= StateHint;
	wmHints.initial_state = initial_state;
	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);
}

/******************************************************************************
 Refresh (virtual)

 ******************************************************************************/

void
JXWindow::Refresh()
	const
{
	RefreshRect(itsBounds);
}

/******************************************************************************
 RefreshRect

 ******************************************************************************/

void
JXWindow::RefreshRect
	(
	const JRect& rect
	)
	const
{
	XRectangle xrect = JXJToXRect(rect);
	XUnionRectWithRegion(&xrect, itsUpdateRegion, itsUpdateRegion);
	itsDisplay->WindowNeedsUpdate(const_cast<JXWindow*>(this));
}

/******************************************************************************
 Redraw (virtual)

 ******************************************************************************/

void
JXWindow::Redraw()
	const
{
	Refresh();
	(const_cast<JXWindow*>(this))->Update();
}

/******************************************************************************
 RedrawRect

 ******************************************************************************/

void
JXWindow::RedrawRect
	(
	const JRect& rect
	)
	const
{
	RefreshRect(rect);
	(const_cast<JXWindow*>(this))->Update();
}

/******************************************************************************
 BufferDrawing

 ******************************************************************************/

void
JXWindow::BufferDrawing
	(
	const JBoolean bufferDrawing
	)
{
	itsBufferDrawingFlag = JI2B(bufferDrawing || itsUseBkgdPixmapFlag);
	if (!itsBufferDrawingFlag && itsBufferPixmap != None)
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		itsBufferPixmap = None;
		}
}

/******************************************************************************
 Update

	It is a bad idea to keep itsBufferPixmap between updates because this
	wastes an enormous amount of server memory.  It is only done under
	special circumstances that warrant optimization.

	jafl 11/12/97:
		Creating the JXWindowPainter each time we are called is easier
		than keeping it because we don't have to worry about what
		Reset() doesn't clear and because the cost is negligible compared
		with creating the buffer pixmap.

 ******************************************************************************/

class bool_value_change
{
public:
	bool_value_change(JBoolean& variable, JBoolean change_to)
		: mVariable(variable), mOriginalValue(variable)
		{ mVariable = change_to; }
	~bool_value_change()
		{ mVariable = mOriginalValue; }
private:
	JBoolean&	mVariable;
	JBoolean	mOriginalValue;
};

void
JXWindow::Update()
{
#ifdef THREAD_TEST
	static int main_tid = 0;
	if (main_tid == 0)
		main_tid = ACE_Thread::self();
	if (main_tid != ACE_Thread::self())
	{
		cerr << endl << "Window Update from background thread" << endl;
	}
#endif

	if (XEmptyRegion(itsUpdateRegion))
		{
		return;
		}
	else if ((!itsIsMappedFlag || itsIsIconifiedFlag) && !itsUseBkgdPixmapFlag)
		{
		XDestroyRegion(itsUpdateRegion);
		itsUpdateRegion = XCreateRegion();
		return;
		}

	assert( !itsUseBkgdPixmapFlag || itsBufferDrawingFlag );

	// Prevent re-entrant updates
	if (itsUpdating)
	{
		// Make sure display will update this window next time through its idle loop
		itsDisplay->WindowNeedsUpdate(const_cast<JXWindow*>(this));
		return;
	}

	// Set flag to indicate inside update - use stack class
	// so it gets changed back if an exception occurs
	bool_value_change _preserve_value(itsUpdating, kJTrue);

	// We clear itsUpdateRegion first so widgets call call Refresh() inside Draw()

	Region updateRegion = JXCopyRegion(itsUpdateRegion);
	XDestroyRegion(itsUpdateRegion);
	itsUpdateRegion = XCreateRegion();

	const Drawable drawable = PrepareForUpdate();
	const JRect rect        = JXGetRegionBounds(updateRegion);
	JXWindowPainter p(itsGC, drawable, itsBounds, updateRegion);
	DrawAll(p, rect);

	FinishUpdate(rect, updateRegion);
}

/******************************************************************************
 UpdateForScroll

	Before this function can be used, it must account for areas of the
	window that are obscured by other windows.  In addition, it must also
	deal with issues such as:

		JTextEditor caret is visible, but it doesn't move like the text.

		Enclosed widgets.  The input field in a table may trigger scrolling,
		and then it doesn't move like the cells.  Even when it does,
		updating needs to be smoother so it doesn't look so ugly.

 ******************************************************************************/

void
JXWindow::UpdateForScroll
	(
	const JRect& ap,
	const JRect& src,
	const JRect& dest
	)
{
	assert( src.width() == dest.width() && src.height() == dest.height() );
	assert( ap.Contains(src) && ap.Contains(dest) );

	if ((!itsIsMappedFlag || itsIsIconifiedFlag) && !itsUseBkgdPixmapFlag)
		{
		return;
		}
	Update();	// scrollbars, etc.

	assert( !itsUseBkgdPixmapFlag || itsBufferDrawingFlag );

	// move the pixels that remain visible

	const Drawable scrollD = (itsUseBkgdPixmapFlag && itsBufferPixmap != None) ?
							 itsBufferPixmap : itsXWindow;
	itsGC->SetClipRect(ap);
	itsGC->CopyPixels(scrollD, src.left, src.top, src.width(), src.height(),
					  scrollD, dest.left, dest.top);

	// redraw the strips that have moved into view

	const Drawable drawable = PrepareForUpdate();
	Region updateRegion     = XCreateRegion();

	JRect r = ap;
	if (dest.top > ap.top)
		{
		r.bottom = dest.top;
		JXWindowPainter p(itsGC, drawable, r, NULL);
		DrawAll(p, r);
		}
	else if (dest.bottom < ap.bottom)
		{
		r.top = dest.bottom;
		JXWindowPainter p(itsGC, drawable, r, NULL);
		DrawAll(p, r);
		}
	if (r.height() != ap.height())
		{
		XRectangle xrect = JXJToXRect(r);
		XUnionRectWithRegion(&xrect, updateRegion, updateRegion);
		}

	r        = ap;
	r.top    = dest.top;
	r.bottom = dest.bottom;
	if (dest.left > ap.left)
		{
		r.right = dest.left;
		JXWindowPainter p(itsGC, drawable, r, NULL);
		DrawAll(p, r);
		}
	else if (dest.right < ap.right)
		{
		r.left = dest.right;
		JXWindowPainter p(itsGC, drawable, r, NULL);
		DrawAll(p, r);
		}
	if (r.width() != ap.width())
		{
		XRectangle xrect = JXJToXRect(r);
		XUnionRectWithRegion(&xrect, updateRegion, updateRegion);
		}

	FinishUpdate(JXGetRegionBounds(updateRegion), updateRegion);
}

/******************************************************************************
 PrepareForUpdate (private)

 ******************************************************************************/

Drawable
JXWindow::PrepareForUpdate()
{
	Drawable drawable   = itsXWindow;
	const JCoordinate w = itsBounds.width();
	const JCoordinate h = itsBounds.height();

	if (itsBufferDrawingFlag && itsBufferPixmap == None)
		{
		itsBufferPixmap = XCreatePixmap(*itsDisplay, itsXWindow, w,h,
										itsDisplay->GetDepth());
		if (itsBufferPixmap != None)
			{
			drawable = itsBufferPixmap;
			}
		}
	else if (itsBufferDrawingFlag)
		{
		drawable = itsBufferPixmap;
		}

	return drawable;
}

/******************************************************************************
 FinishUpdate (private)

	region is deleted.

 ******************************************************************************/

void
JXWindow::FinishUpdate
	(
	const JRect&	rect,
	Region			region
	)
{
	if (itsUseBkgdPixmapFlag && itsBufferPixmap != None)
		{
		XSetWindowBackgroundPixmap(*itsDisplay, itsXWindow, itsBufferPixmap);
		XClearWindow(*itsDisplay, itsXWindow);
		}
	else if (itsUseBkgdPixmapFlag)
		{
		XSetWindowBackground(*itsDisplay, itsXWindow,
							 itsColormap->GetXPixel(itsBackColor));
		}
	else if (itsBufferPixmap != None)
		{
		itsGC->SetClipRegion(region);
		itsGC->CopyPixels(itsBufferPixmap,
						  rect.left, rect.top, rect.width(), rect.height(),
						  itsXWindow, rect.left, rect.top);
		}

	// Under normal conditions, we have to toss the pixmap because
	// it uses an enormous amount of server memory.

	if (!itsKeepBufferPixmapFlag && itsBufferPixmap != None &&
		!(itsIsDraggingFlag && itsProcessDragFlag))
		{
		XFreePixmap(*itsDisplay, itsBufferPixmap);
		itsBufferPixmap = None;
		}

	XDestroyRegion(region);

	itsDisplay->Flush();
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXWindow::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXWindow::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 DrawBackground (virtual protected)

 ******************************************************************************/

void
JXWindow::DrawBackground
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.SetPenColor(itsBackColor);
	p.SetFilling(kJTrue);
	p.JPainter::Rect(itsBounds);
}

/******************************************************************************
 GlobalToLocal (virtual)

 ******************************************************************************/

JPoint
JXWindow::GlobalToLocal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x,y);
}

/******************************************************************************
 LocalToGlobal (virtual)

 ******************************************************************************/

JPoint
JXWindow::LocalToGlobal
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	return JPoint(x,y);
}

/******************************************************************************
 GlobalToRoot

 ******************************************************************************/

JRect
JXWindow::GlobalToRoot
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = GlobalToRoot(r.left, r.top);
	const JPoint botRight = GlobalToRoot(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 RootToGlobal

 ******************************************************************************/

JRect
JXWindow::RootToGlobal
	(
	const JRect& r
	)
	const
{
	const JPoint topLeft  = RootToGlobal(r.left, r.top);
	const JPoint botRight = RootToGlobal(r.right, r.bottom);
	return JRect(topLeft.y, topLeft.x, botRight.y, botRight.x);
}

/******************************************************************************
 CalcDesktopLocation (private)

	Shift the given point to cancel the window manager border.

	direction should be +1 or -1.
		+1 => WMFrame    -> itsXWindow
		-1 => itsXWindow -> WMFrame

	If anything fails, the window manager must be having convulsions
	(e.g. restarting).  In this case, we will get ConfigureNotify after
	things calm down, so we simply return the input values.

	Some window managers are smart enough to automatically compensate for
	the window border when we call XMoveWindow().  We like this behavior,
	so we assume it by default by setting compensate=kJFalse.  It won't
	always work, however, so we check the result in Place(), and if it
	didn't work, we change our behavior and try again.  Once we find a
	method that works, we set method=kJTrue so we ignore further failures
	as network latency problems.

 ******************************************************************************/

JPoint
JXWindow::CalcDesktopLocation
	(
	const JCoordinate origX,
	const JCoordinate origY,
	const JCoordinate origDirection
	)
	const
{
	assert( !itsIsDockedFlag );

#ifdef _J_CYGWIN

	theFoundWMFrameMethodFlag = kJTrue;
	theWMFrameCompensateFlag  = kJTrue;

	JPoint desktopPt(origX, origY);
	if (itsIsMenuFlag || !itsIsMappedFlag)
		{
		return desktopPt;
		}
	else
		{
		return desktopPt + JPoint(4, 30) * JSign(origDirection);
		}

#else

	if (origDirection > 0 && !theWMFrameCompensateFlag)
		{
		return JPoint(origX, origY);
		}

	Window rootChild;
	if (!GetRootChild(&rootChild) || rootChild == itsXWindow)
		{
		return JPoint(origX, origY);
		}

	JCoordinate desktopX = origX;
	JCoordinate desktopY = origY;

	int x,y;
	Window childWindow;
	if (XTranslateCoordinates(*itsDisplay, itsXWindow, rootChild,
							  0,0, &x,&y, &childWindow))
		{
		const JCoordinate direction = JSign(origDirection);
		desktopX += x * direction;
		desktopY += y * direction;
		}

	// adjust position to offset fvwm2 bug

	JPoint desktopPt(desktopX, desktopY);
	if (!itsIsMenuFlag)
		{
		desktopPt += theWMOffset * origDirection;
		}

	return desktopPt;

#endif
}

/******************************************************************************
 GetRootChild (private)

	If possible, returns the X window that is our ancestor and a child
	of the root window.

 ******************************************************************************/

JBoolean
JXWindow::GetRootChild
	(
	Window* rootChild
	)
	const
{
	*rootChild = None;

	Window currWindow = itsXWindow;
	while (1)
		{
		Window rootWindow, parentWindow;
		Window* childList;
		unsigned int childCount;
		if (!XQueryTree(*itsDisplay, currWindow, &rootWindow,
						&parentWindow, &childList, &childCount))
			{
			return kJFalse;
			}
		XFree(childList);

		if (parentWindow == rootWindow)
			{
			*rootChild = currWindow;
			return kJTrue;
			}
		else
			{
			currWindow = parentWindow;
			}
		}
}

/******************************************************************************
 Place (virtual)

	We can't optimize to do nothing if the size won't change because
	there might be geometry events waiting in the queue.

 ******************************************************************************/

static JSize thePlacementAttempt = 0;

void
JXWindow::Place
	(
	const JCoordinate enclX,
	const JCoordinate enclY
	)
{
	if (itsIsDockedFlag)
		{
//		itsUndockedGeom.Place(CalcDesktopLocation(enclX, enclY, +1));
		itsUndockedWMFrameLoc.Set(enclX, enclY);
		}
	else
		{
		UndockedPlace(enclX, enclY);
		}
}

void
JXWindow::UndockedPlace
	(
	const JCoordinate origEnclX,
	const JCoordinate origEnclY
	)
{
	JCoordinate enclX = origEnclX;
	JCoordinate enclY = origEnclY;

	JPoint pt(enclX, enclY);
	if (!itsIsDockedFlag)
		{
		// adjust position so at least part of window is visible
		// (important for virtual desktops)

		const JRect desktopBounds = itsDisplay->GetBounds();

		while (enclX + itsBounds.width() <= desktopBounds.left)
			{
			enclX += desktopBounds.width();
			}
		while (enclX >= desktopBounds.right)
			{
			enclX -= desktopBounds.width();
			}

		while (enclY + itsBounds.height() <= desktopBounds.top)
			{
			enclY += desktopBounds.height();
			}
		while (enclY >= desktopBounds.bottom)
			{
			enclY -= desktopBounds.height();
			}

		// compensate for width of window manager frame

		pt = CalcDesktopLocation(enclX, enclY, +1);
		}

	// tell the window manager to move us

	XMoveWindow(*itsDisplay, itsXWindow, pt.x, pt.y);

	if (!itsIsDockedFlag)
		{
		long supplied;
		XSizeHints sizeHints;
		if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
			{
			sizeHints.flags = 0;
			}

		sizeHints.flags |= PPosition;
		sizeHints.x      = pt.x;
		sizeHints.y      = pt.y;

		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		}

	// menu needs to know its location immediately
	// (This works because menus have OverrideRedirect.)

	if (itsIsMenuFlag || itsIsDockedFlag)
		{
		itsDisplay->Synchronize();
		UpdateFrame();
		}
/*
	We can't just call XSync(), because the window manager gets the
	placement request, and XSync() doesn't wait for a reply from the WM,
	only from the X server.  We can't simply use itsExpectedPosition in
	HandleEvent(), because there may be other, earlier ConfigureNotify
	events in the queue (or worse, in the network, on their way to us),
	which will confuse us.
*/
	else if (!theFoundWMFrameMethodFlag && itsIsMappedFlag &&
			 thePlacementAttempt < 2)
		{
		itsDisplay->Synchronize();
		JWait(0.5);
		UpdateFrame();
/*
		cout << endl;
		cout << thePlacementAttempt;
		cout << theFoundWMFrameMethodFlag;
		cout << theWMFrameCompensateFlag << endl;
		cout << GetDesktopLocation() << endl;
		cout << JPoint(enclX, enclY) << endl;
*/
		thePlacementAttempt++;

		const JPoint pt = GetDesktopLocation();
		if (JLAbs(pt.x - enclX) > kWMFrameSlop ||
			JLAbs(pt.y - enclY) > kWMFrameSlop)
			{
			theWMFrameCompensateFlag = JNegate(theWMFrameCompensateFlag);
			Place(enclX, enclY);
			}
		else
			{
			theFoundWMFrameMethodFlag = kJTrue;
			}

		assert( thePlacementAttempt > 0 );
		thePlacementAttempt--;
		}
	else
		{
		// same as UpdateFrame()

		itsDesktopLoc = pt;
		itsWMFrameLoc = CalcDesktopLocation(pt.x, pt.y, -1);
		}
}

/******************************************************************************
 Move (virtual)

 ******************************************************************************/

void
JXWindow::Move
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	Place(itsWMFrameLoc.x + dx, itsWMFrameLoc.y + dy);
}

void
JXWindow::UndockedMove
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	UndockedPlace(itsWMFrameLoc.x + dx, itsWMFrameLoc.y + dy);
}

/******************************************************************************
 SetSize (virtual)

	We can't optimize to do nothing if the size won't change because
	there might be geometry events waiting in the queue.

 ******************************************************************************/

void
JXWindow::SetSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	if (itsIsDockedFlag)
		{
		itsUndockedGeom.SetSize(w, h);
		}
	else
		{
		UndockedSetSize(w, h);
		}
}

void
JXWindow::UndockedSetSize
	(
	const JCoordinate origW,
	const JCoordinate origH
	)
{
	JCoordinate w = origW;
	JCoordinate h = origH;
	if (itsHasMinSizeFlag)
		{
		w = JMax(w, itsMinSize.x);
		h = JMax(h, itsMinSize.y);
		}
	if (itsHasMaxSizeFlag)
		{
		w = JMin(w, itsMaxSize.x);
		h = JMin(h, itsMaxSize.y);
		}

	XResizeWindow(*itsDisplay, itsXWindow, w, h);

	if (!itsIsDockedFlag)
		{
		long supplied;
		XSizeHints sizeHints;
		if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
			{
			sizeHints.flags = 0;
			}

		sizeHints.flags  |= PSize;
		sizeHints.width   = w;
		sizeHints.height  = h;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		}

	UpdateBounds(w, h);
}

/******************************************************************************
 AdjustSize (virtual)

 ******************************************************************************/

void
JXWindow::AdjustSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	SetSize(itsBounds.width() + dw, itsBounds.height() + dh);
}

/******************************************************************************
 UpdateFrame (private)

	We can't pass in the new frame information because ConfigureNotify
	caused by resize window gives x=0, y=0.

 ******************************************************************************/

void
JXWindow::UpdateFrame()
{
	Window rootWindow;
	int x,y;
	unsigned int w,h, bw, depth;
	const Status ok1 = XGetGeometry(*itsDisplay, itsXWindow, &rootWindow,
									&x, &y, &w, &h, &bw, &depth);
	assert( ok1 );

	const int origX = x, origY = y;

	// After XGetGeometry(), x=0 and y=0 (at least for fvwm)

	Window childWindow;
	const Bool ok2 = XTranslateCoordinates(*itsDisplay, itsXWindow, rootWindow,
										   0,0, &x, &y, &childWindow);
	assert( ok2 );

	itsDesktopLoc.Set(x,y);		// also at end of Place()
	itsWMFrameLoc =
		itsIsDockedFlag ? JPoint(origX, origY) : CalcDesktopLocation(x,y, -1);

	if (itsIsMappedFlag)
		{
		UpdateBounds(w, h);
		}
}

/******************************************************************************
 UpdateBounds (private)

 ******************************************************************************/

void
JXWindow::UpdateBounds
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	const JCoordinate dw = w - itsBounds.width();
	const JCoordinate dh = h - itsBounds.height();
	itsBounds.top    = 0;
	itsBounds.left   = 0;
	itsBounds.bottom = h;
	itsBounds.right  = w;

	if (dw != 0 || dh != 0)
		{
		NotifyBoundsResized(dw,dh);

		if (itsBufferPixmap != None)
			{
			XFreePixmap(*itsDisplay, itsBufferPixmap);
			itsBufferPixmap = None;
			}
		}
}

/******************************************************************************
 CenterOnScreen

 ******************************************************************************/

void
JXWindow::CenterOnScreen()
{
	if (!itsIsDockedFlag)
		{
		const JRect r = itsDisplay->GetBounds();
		Place((r.left + r.right - itsBounds.width())/2,
			  (r.top + r.bottom - itsBounds.height())/2);
		}
}

/******************************************************************************
 PlaceAsDialogWindow

	Following Macintosh Human Interface Guidelines, dialog windows
	have 1/3 blank space above and 2/3 blank space below.

 ******************************************************************************/

void
JXWindow::PlaceAsDialogWindow()
{
	if (!itsIsDockedFlag)
		{
		const JRect r = itsDisplay->GetBounds();
		Place((r.left + r.right - itsBounds.width())/2,
			  (r.top + r.bottom - itsBounds.height())/3);
		}
}

/******************************************************************************
 SetMinSize

 ******************************************************************************/

void
JXWindow::SetMinSize
	(
	const JCoordinate origW,
	const JCoordinate origH
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if ((sizeHints.flags & PMaxSize) != 0)
		{
		w = JMin(w, (JCoordinate) sizeHints.max_width);
		h = JMin(h, (JCoordinate) sizeHints.max_height);
		}

	sizeHints.flags     |= PMinSize;
	sizeHints.min_width  = w;
	sizeHints.min_height = h;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();

	itsHasMinSizeFlag = kJTrue;
	itsMinSize.x      = w;
	itsMinSize.y      = h;

	if (itsBounds.width() < w || itsBounds.height() < h)
		{
		SetSize(JMax(w,itsBounds.width()), JMax(h,itsBounds.height()));
		}

	Broadcast(MinSizeChanged());
}

/******************************************************************************
 ClearMinSize

 ******************************************************************************/

void
JXWindow::ClearMinSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PMinSize) != 0)
		{
		sizeHints.flags     -= PMinSize;
		sizeHints.min_width  = 0;
		sizeHints.min_height = 0;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}

	itsHasMinSizeFlag = kJFalse;
}

/******************************************************************************
 SetMaxSize

 ******************************************************************************/

void
JXWindow::SetMaxSize
	(
	const JCoordinate origW,
	const JCoordinate origH
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	JCoordinate w = origW;
	JCoordinate h = origH;
	if ((sizeHints.flags & PMinSize) != 0)
		{
		w = JMax(w, (JCoordinate) sizeHints.min_width);
		h = JMax(h, (JCoordinate) sizeHints.min_height);
		}

	sizeHints.flags     |= PMaxSize;
	sizeHints.max_width  = w;
	sizeHints.max_height = h;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();

	itsHasMaxSizeFlag = kJTrue;
	itsMaxSize.x      = w;
	itsMaxSize.y      = h;

	if (itsBounds.width() > w || itsBounds.height() > h)
		{
		SetSize(JMin(w,itsBounds.width()), JMin(h,itsBounds.height()));
		}
}

/******************************************************************************
 ClearMaxSize

 ******************************************************************************/

void
JXWindow::ClearMaxSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PMaxSize) != 0)
		{
		sizeHints.flags     -= PMaxSize;
		sizeHints.max_width  = INT_MAX;
		sizeHints.max_height = INT_MAX;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}

	itsHasMaxSizeFlag = kJFalse;
}

/******************************************************************************
 SetStepSize

 ******************************************************************************/

void
JXWindow::SetStepSize
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	long supplied;
	XSizeHints sizeHints;
	if (!XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied))
		{
		sizeHints.flags = 0;
		}

	sizeHints.flags     |= PResizeInc;
	sizeHints.width_inc  = dw;
	sizeHints.height_inc = dh;
	XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
	itsDisplay->Flush();
}

/******************************************************************************
 ClearStepSize

 ******************************************************************************/

void
JXWindow::ClearStepSize()
{
	long supplied;
	XSizeHints sizeHints;
	if (XGetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints, &supplied) &&
		(sizeHints.flags & PResizeInc) != 0)
		{
		sizeHints.flags     -= PResizeInc;
		sizeHints.width_inc  = 1;
		sizeHints.height_inc = 1;
		XSetWMNormalHints(*itsDisplay, itsXWindow, &sizeHints);
		itsDisplay->Flush();
		}
}

/******************************************************************************
 SetBackColor

 ******************************************************************************/

void
JXWindow::SetBackColor
	(
	const JColorIndex color
	)
{
	itsBackColor = color;
	if (!itsUseBkgdPixmapFlag)
		{
		XSetWindowBackground(*itsDisplay, itsXWindow, itsColormap->GetXPixel(color));
		}
}

/******************************************************************************
 SetIcon

	Sets the icon to be displayed when the window is iconified.

	Call this after SetWMClass() to avoid losing the icon's mask.

	We take ownership of icon.

	Once you use an icon with a mask, you will not able to clear the mask.
	You must instead specify a square mask.  This is a design flaw in X.

 ******************************************************************************/

void
JXWindow::SetIcon
	(
	JXImage* icon
	)
{
	XWMHints wmHints;
	wmHints.flags = 0;

	// get current hints

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != NULL)
		{
		wmHints = *origHints;
		XFree(origHints);
		}

	// set new icon

	if (itsIconDir != NULL)
		{
		const JBoolean ok = itsIconDir->Close();
		assert( ok );
		itsIconDir = NULL;
		}

	delete itsIcon;
	itsIcon = icon;

	icon->ConvertToPixmap();

	wmHints.flags       |= IconPixmapHint;
	wmHints.icon_pixmap  = icon->GetPixmap();

	JXImageMask* mask;
	if (icon->GetMask(&mask))
		{
		mask->ConvertToPixmap();

		wmHints.flags    |= IconMaskHint;
		wmHints.icon_mask = mask->GetPixmap();
		}

	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);
}

/******************************************************************************
 SetIcon

	Sets the icon to be displayed when the window is iconified.  This
	version creates a window with a JXWindowIcon that you can ListenTo()
	to accept DND.

	Call this after SetWMClass() to avoid losing the icon's mask.

	We take ownership of the icons.

	Once you use this version, you will no longer be able to use the
	version that doesn't use an X window.  This is a design flaw in X.

 ******************************************************************************/

JXWindowIcon*
JXWindow::SetIcon
	(
	JXImage* normalIcon,
	JXImage* dropIcon
	)
{
	XWMHints wmHints;
	wmHints.flags = 0;

	// get current hints

	XWMHints* origHints = XGetWMHints(*itsDisplay, itsXWindow);
	if (origHints != NULL)
		{
		wmHints = *origHints;
		XFree(origHints);
		}

	// set new icon

	delete itsIcon;
	itsIcon = NULL;

	if (itsIconDir == NULL)
		{
		itsIconDir = new JXIconDirector(itsDirector, normalIcon, dropIcon);
		assert( itsIconDir != NULL );
		(itsIconDir->GetWindow())->itsMainWindow = this;
		}
	else
		{
		(itsIconDir->GetIconWidget())->SetIcons(normalIcon, dropIcon);
		}

	wmHints.flags       |= IconWindowHint;
	wmHints.icon_window  = (itsIconDir->GetWindow())->GetXWindow();

	XSetWMHints(*itsDisplay, itsXWindow, &wmHints);

	return itsIconDir->GetIconWidget();
}

/******************************************************************************
 GetIconWidget

 ******************************************************************************/

JBoolean
JXWindow::GetIconWidget
	(
	JXWindowIcon** widget
	)
	const
{
	if (itsIconDir != NULL)
		{
		*widget = itsIconDir->GetIconWidget();
		return kJTrue;
		}
	else
		{
		*widget = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXWindow::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
}

/******************************************************************************
 EnclosingBoundsMoved (virtual protected)

	We will never get this message.

 ******************************************************************************/

void
JXWindow::EnclosingBoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	assert( 0 );
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXWindow::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
}

/******************************************************************************
 EnclosingBoundsResized (virtual protected)

	We will never get this message.

 ******************************************************************************/

void
JXWindow::EnclosingBoundsResized
	(
	const JCoordinate dwb,
	const JCoordinate dhb
	)
{
	assert( 0 );
}

/******************************************************************************
 GetBoundsGlobal (virtual)

	Returns the bounds in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetBoundsGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 GetFrameGlobal (virtual)

	Returns the frame in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetFrameGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 GetApertureGlobal (virtual)

	Returns the aperture in global coordinates.

 ******************************************************************************/

JRect
JXWindow::GetApertureGlobal()
	 const
{
	return itsBounds;
}

/******************************************************************************
 ReadGeometry

	Read in position, size, and state and adjust ourselves.

	In the version that takes JString&, the string can be empty, in which
	case, nothing changes.  If the string is not empty, it is assumed
	to contain only the output of WriteGeometry().

 ******************************************************************************/

void
JXWindow::ReadGeometry
	(
	const JString& data
	)
{
	const JSize dataLength = data.GetLength();
	if (dataLength > 0)
		{
		const std::string s(data.GetCString(), dataLength);
		std::istringstream input(s);
		ReadGeometry(input);
		}
}

void
JXWindow::ReadGeometry
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentGeometryDataVersion)
		{
		JIgnoreUntil(input, kGeometryDataEndDelimiter);
		return;
		}

	JPoint desktopLoc;
	JCoordinate w,h;
	JBoolean iconified;
	input >> desktopLoc >> w >> h >> iconified;

	JBoolean docked = kJFalse;
	if (vers >= 1)
		{
		JIndex id;
		input >> id;

		JXDockManager* mgr;
		JXDockWidget* dock;
		if (id != JXDockManager::kInvalidDockID &&
			JXGetDockManager(&mgr) &&
			mgr->FindDock(id, &dock))
			{
			docked = dock->Dock(this);
			}
		}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);

	if (docked)
		{
		itsUndockedWMFrameLoc = desktopLoc;
		itsUndockedGeom.Set(0, 0, h, w);
		}
	else
		{
		if (itsIsDockedFlag)
			{
			Undock();
			}

		Place(desktopLoc.x, desktopLoc.y);
		SetSize(w,h);

		if (iconified)
			{
			Iconify();
			}
		else
			{
			Deiconify();
			}
		}
}

/******************************************************************************
 SkipGeometry (static)

 ******************************************************************************/

void
JXWindow::SkipGeometry
	(
	istream& input
	)
{
	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

	Write out our position, size, and state (iconified).

	We have to write out an ending delimiter that is never used anywhere
	else so we can at least ignore the data if we can't read the given
	version.

 ******************************************************************************/

void
JXWindow::WriteGeometry
	(
	JString* data
	)
	const
{
	std::ostringstream output;
	WriteGeometry(output);
	*data = output.str();
}

void
JXWindow::WriteGeometry
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentGeometryDataVersion;

	if (itsIsDockedFlag)
		{
		output << ' ' << itsUndockedWMFrameLoc;
		output << ' ' << itsUndockedGeom.width();
		output << ' ' << itsUndockedGeom.height();
		output << ' ' << kJFalse;
		}
	else
		{
		output << ' ' << GetDesktopLocation();
		output << ' ' << GetFrameWidth();
		output << ' ' << GetFrameHeight();
		output << ' ' << IsIconified();
		}

	output << ' ' << (itsDockWidget != NULL ? itsDockWidget->GetID() :
											  JXDockManager::kInvalidDockID);

	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 HandleEvent

 ******************************************************************************/

void
JXWindow::HandleEvent
	(
	const XEvent& xEvent
	)
{
	if (xEvent.type == EnterNotify)
		{
		JXClearKeyPressPreprocessor();
		HandleEnterNotify(xEvent.xcrossing);
		}
	else if (xEvent.type == LeaveNotify)
		{
		JXClearKeyPressPreprocessor();
		HandleLeaveNotify(xEvent.xcrossing);
		}
	else if (xEvent.type == MotionNotify)
		{
		HandleMotionNotify(xEvent.xmotion);
		}
	else if (xEvent.type == ButtonPress)
		{
		JXClearKeyPressPreprocessor();
		HandleButtonPress(xEvent.xbutton);
		}
	else if (xEvent.type == ButtonRelease)
		{
		JXClearKeyPressPreprocessor();
		HandleButtonRelease(xEvent.xbutton);
		}

	else if (xEvent.type == FocusIn)
		{
		JXClearKeyPressPreprocessor();
		HandleFocusIn(xEvent.xfocus);
		}
	else if (xEvent.type == FocusOut)
		{
		JXClearKeyPressPreprocessor();
		HandleFocusOut(xEvent.xfocus);
		}
	else if (xEvent.type == KeyPress)
		{
		HandleKeyPress(xEvent);
		}

	else if (xEvent.type == Expose && !itsUseBkgdPixmapFlag)
		{
		XRectangle xrect;
		xrect.x      = xEvent.xexpose.x;
		xrect.y      = xEvent.xexpose.y;
		xrect.width  = xEvent.xexpose.width;
		xrect.height = xEvent.xexpose.height;
		XUnionRectWithRegion(&xrect, itsUpdateRegion, itsUpdateRegion);
		itsDisplay->WindowNeedsUpdate(this);
		}

	else if (xEvent.type == ConfigureNotify && xEvent.xconfigure.window == itsXWindow)
		{
		UpdateFrame();
		UpdateChildWindowGeometry();	// update itsDesktopLoc
		}
	else if (xEvent.type == ConfigureNotify)
		{
		SetChildWindowGeometry(xEvent.xconfigure.window,
			JRect(xEvent.xconfigure.y, xEvent.xconfigure.x,
				  xEvent.xconfigure.y + xEvent.xconfigure.height,
				  xEvent.xconfigure.x + xEvent.xconfigure.width));
		}

	else if (xEvent.type == MapNotify && xEvent.xmap.window == itsXWindow)
		{
		JXClearKeyPressPreprocessor();
		HandleMapNotify(xEvent.xmap);
		}
	else if (xEvent.type == UnmapNotify && xEvent.xunmap.window == itsXWindow)
		{
		JXClearKeyPressPreprocessor();
		HandleUnmapNotify(xEvent.xunmap);
		}
	else if (xEvent.type == MapNotify || xEvent.type == UnmapNotify)
		{
		SetChildWindowVisible(xEvent.xmap.window,
							  JI2B(xEvent.type == MapNotify));
		}
	else if (xEvent.type             == PropertyNotify &&
			 xEvent.xproperty.window == itsXWindow &&
			 xEvent.xproperty.atom   == itsDisplay->GetWMStateXAtom() &&
			 xEvent.xproperty.state  == PropertyNewValue)
		{
		HandleWMStateChange();
		}

	else if (xEvent.type == ReparentNotify && xEvent.xreparent.window == itsXWindow)
		{
		// When window is dragged to different compartment in same window
		// with exactly the same size, we don't receive ConfigureNotify!
		UpdateFrame();
		}
	else if (xEvent.type == ReparentNotify)
		{
		UpdateChildWindowList(xEvent.xreparent.window,
							  JI2B(xEvent.xreparent.parent == itsXWindow));
		}

	else if (xEvent.type == DestroyNotify && xEvent.xdestroywindow.window == itsXWindow)
		{
		// we have been deleted -- primarily for Netscape plug-in

		itsXWindow     = None;
		itsFocusWidget = NULL;		// too late to validate
		itsDirector->Close();		// deletes us
		}
	else if (xEvent.type == DestroyNotify)
		{
		UpdateChildWindowList(xEvent.xdestroywindow.window, kJFalse);
		}

	else if (IsDeleteWindowMessage(itsDisplay, xEvent))		// trap WM_DELETE_WINDOW first
		{
		Close();											// can delete us
		}
	else if (IsSaveYourselfMessage(itsDisplay, xEvent))
		{
		const JString& cmd = (JXGetApplication())->GetRestartCommand();
		char* s            = const_cast<JCharacter*>(cmd.GetCString());
		XSetCommand(*itsDisplay, itsXWindow, &s, 1);
		}
	else if (xEvent.type == ClientMessage)					// otherwise, send to all widgets
		{
		DispatchClientMessage(xEvent.xclient);
		}
}

/******************************************************************************
 IsDeleteWindowMessage (static)

 ******************************************************************************/

JBoolean
JXWindow::IsDeleteWindowMessage
	(
	const JXDisplay*	display,
	const XEvent&		xEvent
	)
{
	return JI2B(
		xEvent.type                       == ClientMessage &&
		xEvent.xclient.message_type       == display->GetWMProtocolsXAtom() &&
		xEvent.xclient.format             == 32 &&
		((Atom) xEvent.xclient.data.l[0]) == display->GetDeleteWindowXAtom());
}

/******************************************************************************
 IsSaveYourselfMessage (static)

 ******************************************************************************/

JBoolean
JXWindow::IsSaveYourselfMessage
	(
	const JXDisplay*	display,
	const XEvent&		xEvent
	)
{
	return JI2B(
		xEvent.type                       == ClientMessage &&
		xEvent.xclient.message_type       == display->GetWMProtocolsXAtom() &&
		xEvent.xclient.format             == 32 &&
		((Atom) xEvent.xclient.data.l[0]) == display->GetSaveYourselfXAtom());
}

/******************************************************************************
 AcceptSaveYourself

 ******************************************************************************/

void
JXWindow::AcceptSaveYourself
	(
	const JBoolean accept
	)
{
	Atom protocolList[2] =
		{
		itsDisplay->GetDeleteWindowXAtom(),		// always accept this one
		itsDisplay->GetSaveYourselfXAtom()
		};
	XSetWMProtocols(*itsDisplay, itsXWindow, protocolList, (accept ? 2 : 1));
}

/******************************************************************************
 HandleEnterNotify (private)

	If the cursor is grabbed, then this event means that we no longer
	need to process the previous LeaveNotify.

 ******************************************************************************/

void
JXWindow::HandleEnterNotify
	(
	const XCrossingEvent& xEvent
	)
{
	if (JXButtonStates::AllOff(xEvent.state))
		{
		itsDisplay->SetMouseContainer(this);
		itsButtonPressReceiver->
			DispatchNewMouseEvent(EnterNotify, JPoint(xEvent.x, xEvent.y),
								  kJXNoButton, xEvent.state);
		}
	else	// cursor grabbed
		{
		itsCursorLeftFlag = kJFalse;
		}
}

/******************************************************************************
 HandleLeaveNotify (private)

	If the cursor is grabbed, then we don't want to process this event
	until the drag is finished.

 ******************************************************************************/

void
JXWindow::HandleLeaveNotify
	(
	const XCrossingEvent& xEvent
	)
{
	if (JXButtonStates::AllOff(xEvent.state))
		{
		SetMouseContainer(NULL, JPoint(xEvent.x, xEvent.y), xEvent.state);
		itsDisplay->SetMouseContainer(NULL);
		}
	else	// cursor grabbed
		{
		itsCursorLeftFlag = kJTrue;
		}
}

/******************************************************************************
 HandleMotionNotify (private)

 ******************************************************************************/

void
JXWindow::HandleMotionNotify
	(
	const XMotionEvent& xEvent
	)
{
	XEvent discardEvent;
	while (XCheckMaskEvent(*itsDisplay, PointerMotionMask, &discardEvent))
		{ };

	// XQueryPointer returns the current mouse state, not the state
	// when the XMotionEvent was generated.  This can cause HandleMouseDrag()
	// before HandleMouseDown().

	Window rootWindow, childWindow;
	int x_root, y_root, x,y;
	unsigned int state;
	if (XQueryPointer(*itsDisplay, itsXWindow, &rootWindow, &childWindow,
					  &x_root, &y_root, &x, &y, &state))
		{
		const JBoolean isDrag = JNegate(JXButtonStates::AllOff(xEvent.state));
		if (itsIsDraggingFlag && isDrag &&			// otherwise wait for ButtonPress
			itsProcessDragFlag && itsMouseContainer != NULL)
			{
			JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
			Display* xDisplay  = *display;
			Window xWindow     = itsXWindow;

			const JPoint pt = itsMouseContainer->GlobalToLocal(x,y);
			itsMouseContainer->DispatchMouseDrag(pt, JXButtonStates(xEvent.state),
												 JXKeyModifiers(itsDisplay, xEvent.state));

			if (JXDisplay::WindowExists(display, xDisplay, xWindow))
				{
				Update();
				}
			}
		else if (!itsIsDraggingFlag && !isDrag && itsButtonPressReceiver->IsVisible())
			{
			itsButtonPressReceiver->
				DispatchNewMouseEvent(MotionNotify, JPoint(x,y),
									  kJXNoButton, xEvent.state);
			}
		// otherwise wait for ButtonRelease
		}
}

/******************************************************************************
 HandleButtonPress (private)

	Returns kJFalse if the window is deleted.

 ******************************************************************************/

JBoolean
JXWindow::HandleButtonPress
	(
	const XButtonEvent& xEvent
	)
{
	RequestFocus();

	if (itsIsMappedFlag && itsIsIconifiedFlag)
		{
		// hack around window managers that don't notify us correctly

		itsIsIconifiedFlag = kJFalse;
		Broadcast(Deiconified());
		Refresh();
		}

	const JXMouseButton currButton = (JXMouseButton) xEvent.button;
	const unsigned int state = JXButtonStates::SetState(xEvent.state, currButton, kJTrue);

	if (!itsIsDraggingFlag)
		{
		itsIsDraggingFlag  = kJTrue;
		itsProcessDragFlag = kJFalse;		// JXContainer tells us if it wants it

		const JPoint ptG(xEvent.x, xEvent.y);

		itsFirstClick         = itsSecondClick;
		itsSecondClick.button = currButton;
		itsSecondClick.time   = xEvent.time;
		// itsSecondClick.pt calculated by JXContainer that accepts click

		JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
		Display* xDisplay  = *display;
		Window xWindow     = itsXWindow;

		itsButtonPressReceiver->
			DispatchNewMouseEvent(ButtonPress, ptG, currButton, state);

		if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
			{
			return kJFalse;
			}

		itsCursorLeftFlag = kJFalse;
		Update();
		}

	else if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != NULL)
		{
		const JPoint pt = itsMouseContainer->GlobalToLocal(xEvent.x, xEvent.y);
		itsMouseContainer->MouseDown(pt, currButton, 1,
									 JXButtonStates(state),
									 JXKeyModifiers(itsDisplay, state));
		}

	// If a blocking dialog window was popped up, then we will never get
	// the ButtonRelease event because HandleOneEventForWindow() tossed it.

	if ((JXGetApplication())->HadBlockingWindow())
		{
		itsCleanAfterBlockFlag = kJTrue;
		if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != NULL)
			{
			EndDrag(itsMouseContainer, JPoint(xEvent.x, xEvent.y),
					JXButtonStates(state), JXKeyModifiers(itsDisplay, state));
			}
		itsIsDraggingFlag      = kJFalse;
		itsCleanAfterBlockFlag = kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 CountClicks

	pt must be in the local coordinates of the given container.

 ******************************************************************************/

JSize
JXWindow::CountClicks
	(
	JXContainer*	container,
	const JPoint&	pt
	)
{
	assert( itsMouseContainer != NULL && container == itsMouseContainer );

	itsSecondClick.pt = pt;

	if (itsPrevMouseContainer == itsMouseContainer &&
		itsFirstClick.button == itsSecondClick.button &&
		itsSecondClick.time - itsFirstClick.time <= kJXDoubleClickTime &&
		itsMouseContainer->HitSamePart(itsFirstClick.pt, itsSecondClick.pt))
		{
		itsClickCount++;
		}
	else
		{
		itsClickCount = 1;
		}

	// set it here so it stays NULL until after we check it the first time

	itsPrevMouseContainer = itsMouseContainer;

	return itsClickCount;
}

/******************************************************************************
 HandleButtonRelease (private)

	Returns kJFalse if the window is deleted.

 ******************************************************************************/

JBoolean
JXWindow::HandleButtonRelease
	(
	const XButtonEvent& xEvent
	)
{
	if (!itsIsDraggingFlag)
		{
		// We can't use assert() because JXMenuTable gets blasted after
		// the left button is released, while an other button might still
		// be down.  (pretty unlikely, but still possible)

		return kJTrue;
		}

	const JXMouseButton currButton = (JXMouseButton) xEvent.button;
	const unsigned int state = JXButtonStates::SetState(xEvent.state, currButton, kJFalse);

	if (itsProcessDragFlag && itsMouseContainer != NULL)
		{
		JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
		Display* xDisplay  = *display;
		Window xWindow     = itsXWindow;

		// this could delete us

		const JPoint pt = itsMouseContainer->GlobalToLocal(xEvent.x, xEvent.y);
		itsMouseContainer->DispatchMouseUp(pt, currButton,
										   JXButtonStates(state),
										   JXKeyModifiers(itsDisplay, state));

		if (!JXDisplay::WindowExists(display, xDisplay, xWindow))
			{
			return kJFalse;
			}
		}

	// If drag is finished, release the pointer and process buffered LeaveNotify.

	if (JXButtonStates::AllOff(state))
		{
		itsIsDraggingFlag = kJFalse;
		if (!itsPointerGrabbedFlag)
			{
			// balance XGrabPointer() in BeginDrag()
			XUngrabPointer(*itsDisplay, xEvent.time);
			itsDisplay->SetMouseGrabber(NULL);
			}
		if (itsCursorLeftFlag)
			{
			SetMouseContainer(NULL, JPoint(xEvent.x, xEvent.y), state);
			}
		}

	// If a blocking dialog window was popped up, then we will never get
	// other ButtonRelease events because HandleOneEventForWindow() tossed them.

	else if ((JXGetApplication())->HadBlockingWindow() && !itsCleanAfterBlockFlag)
		{
		itsCleanAfterBlockFlag = kJTrue;
		if (itsIsDraggingFlag && itsProcessDragFlag && itsMouseContainer != NULL)
			{
			EndDrag(itsMouseContainer, JPoint(xEvent.x, xEvent.y),
					JXButtonStates(state), JXKeyModifiers(itsDisplay, state));
			}
		itsIsDraggingFlag      = kJFalse;
		itsCleanAfterBlockFlag = kJFalse;
		}

	return kJTrue;
}

/******************************************************************************
 SetMouseContainer

 ******************************************************************************/

void
JXWindow::SetMouseContainer
	(
	JXContainer*		obj,
	const JPoint&		ptG,
	const unsigned int	state
	)
{
	if (obj != NULL)
		{
		itsDisplay->SetMouseContainer(this);	// may not get EnterNotify
		}

	if (itsMouseContainer != obj)
		{
		if (itsMouseContainer != NULL)
			{
			itsMouseContainer->MouseLeave();
			itsMouseContainer->DeactivateCursor();
			}

		itsMouseContainer = obj;
		if (itsMouseContainer != NULL)
			{
			itsMouseContainer->ActivateCursor(ptG, JXKeyModifiers(itsDisplay, state));
			itsMouseContainer->MouseEnter();
			}
		}
}

/******************************************************************************
 BeginDrag

	Generate fake messages to get the given widget ready for HandleMouseDrag().

 ******************************************************************************/

JBoolean
JXWindow::BeginDrag
	(
	JXContainer*			obj,
	const JPoint&			ptG,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	assert( obj->GetWindow() == this && !itsIsDraggingFlag );
	assert( !buttonStates.AllOff() );

	const int grabResult =
		XGrabPointer(*itsDisplay, itsXWindow, False, kPointerGrabMask,
					 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
	if (grabResult != GrabSuccess)
		{
		return kJFalse;
		}
	itsDisplay->SetMouseContainer(this);
	itsDisplay->SetMouseGrabber(this);

	JXContainer* savedReceiver = itsButtonPressReceiver;
	itsButtonPressReceiver     = obj;
	itsBPRChangedFlag          = kJFalse;

	XButtonPressedEvent xEvent;
	xEvent.type = ButtonPress;
	xEvent.time = CurrentTime;
	xEvent.x    = ptG.x;
	xEvent.y    = ptG.y;

	JXButtonStates newButtonStates;
	for (JIndex i=1; i<=kXButtonCount; i++)
		{
		if (buttonStates.button(i))
			{
			xEvent.state  = newButtonStates.GetState() | modifiers.GetState();
			xEvent.button = i;
			if (!HandleButtonPress(xEvent))
				{
				return kJFalse;		// window was deleted
				}
			newButtonStates.SetState(i, kJTrue);
			}
		}

	if (!itsBPRChangedFlag)
		{
		itsButtonPressReceiver = savedReceiver;
		}
	return itsProcessDragFlag;
}

/******************************************************************************
 EndDrag

	Generate fake messages to the current drag recipient so that it thinks
	that the mouse was released.

 ******************************************************************************/

void
JXWindow::EndDrag
	(
	JXContainer*			obj,
	const JPoint&			ptG,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
//	assert( itsMouseContainer == obj && itsIsDraggingFlag && itsProcessDragFlag );
//	assert( !buttonStates.AllOff() );

	if (itsMouseContainer != obj || !itsIsDraggingFlag || !itsProcessDragFlag ||
		buttonStates.AllOff())
		{
		return;
		}

	XButtonReleasedEvent xEvent;
	xEvent.type = ButtonRelease;
	xEvent.time = CurrentTime;
	xEvent.x    = ptG.x;
	xEvent.y    = ptG.y;

	JXButtonStates newButtonStates(buttonStates);
	for (JIndex i=1; i<=kXButtonCount; i++)
		{
		if (buttonStates.button(i))
			{
			xEvent.state  = newButtonStates.GetState() | modifiers.GetState();
			xEvent.button = i;
			if (!HandleButtonRelease(xEvent))
				{
				return;		// window was deleted
				}
			newButtonStates.SetState(i, kJFalse);
			}
		}

	XUngrabPointer(*itsDisplay, CurrentTime);
	itsDisplay->SetMouseGrabber(NULL);
}

/******************************************************************************
 GrabPointer

 ******************************************************************************/

JBoolean
JXWindow::GrabPointer
	(
	JXContainer* obj
	)
{
	assert( obj->GetWindow() == this );

	const int success =
		XGrabPointer(*itsDisplay, itsXWindow, False, kPointerGrabMask,
					 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	if (success == GrabSuccess)
		{
		itsPointerGrabbedFlag  = kJTrue;
		itsButtonPressReceiver = obj;
		itsBPRChangedFlag      = kJTrue;		// notify BeginDrag()
		itsDisplay->SetMouseContainer(this);
		itsDisplay->SetMouseGrabber(this);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UngrabPointer

 ******************************************************************************/

void
JXWindow::UngrabPointer
	(
	JXContainer* obj
	)
{
	assert( obj == itsButtonPressReceiver );

	XUngrabPointer(*itsDisplay, CurrentTime);
	itsPointerGrabbedFlag  = kJFalse;
	itsButtonPressReceiver = this;
	itsBPRChangedFlag      = kJTrue;		// notify BeginDrag()
	itsDisplay->SetMouseGrabber(NULL);
}

/******************************************************************************
 HandleFocusIn (private)

 ******************************************************************************/

void
JXWindow::HandleFocusIn
	(
	const XFocusChangeEvent& xEvent
	)
{
	itsHasFocusFlag = kJTrue;
	if (itsFocusWidget != NULL)
		{
		itsFocusWidget->HandleWindowFocusEvent();
		}

	if (itsIsDockedFlag && itsDockWidget != NULL)
		{
		(itsDockWidget->GetDockDirector())->SetFocusWindow(this);
		}

	
	//if (itsDirector && !itsDirector->IsClosing())
	if (itsDirector)
		itsDirector->WindowFocussed(kJTrue);

//	cout << "in : " << xEvent.window << ' ' << xEvent.detail << endl;
}

/******************************************************************************
 HandleFocusOut (private)

 ******************************************************************************/

void
JXWindow::HandleFocusOut
	(
	const XFocusChangeEvent& xEvent
	)
{
	itsHasFocusFlag = kJFalse;
	if (itsFocusWidget != NULL)
		{
		itsFocusWidget->HandleWindowUnfocusEvent();
		}

	if (itsIsDockedFlag && itsDockWidget != NULL)
		{
		(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
		}
	
	//if (itsDirector && !itsDirector->IsClosing())
	if (itsDirector)
		itsDirector->WindowFocussed(kJFalse);

//	cout << "out: " << xEvent.window << ' ' << xEvent.detail << endl;
}

/******************************************************************************
 HandleKeyPress (private)

 ******************************************************************************/

void
JXWindow::HandleKeyPress
	(
	const XEvent& xEvent	// avoids cast in call to JXGetButtonAndModifierStates()
	)
{
	if (!IsActive())
		{
		return;
		}
	if (itsMainWindow != NULL)	// send keypresses to main window, not icon
		{
		itsMainWindow->HandleKeyPress(xEvent);
		return;
		}

	JCharacter buffer[10];
	KeySym keySym;
#undef X_HAVE_UTF8_STRING
#ifdef X_HAVE_UTF8_STRING
	JSize charCount =
		Xutf8LookupString(NULL, const_cast<XKeyEvent*>(&(xEvent.xkey)), buffer, 10, &keySym, NULL);
#else
	JSize charCount =
		XLookupString(const_cast<XKeyEvent*>(&(xEvent.xkey)), buffer, 10, &keySym, NULL);
#endif
	if (charCount == 0)
		{
		buffer[0] = '\0';
		}

	TossKeyRepeatEvents(xEvent.xkey.keycode, xEvent.xkey.state, keySym);

	// translate some useful keys

	if (keySym == XK_KP_Tab || keySym == XK_ISO_Left_Tab)
		{
		keySym = XK_Tab;
		}
	else if (keySym == XK_Left || keySym == XK_KP_Left)
		{
		charCount = 1;
		buffer[0] = kJLeftArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Up || keySym == XK_KP_Up)
		{
		charCount = 1;
		buffer[0] = kJUpArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Right || keySym == XK_KP_Right)
		{
		charCount = 1;
		buffer[0] = kJRightArrow;
		buffer[1] = '\0';
		}
	else if (keySym == XK_Down || keySym == XK_KP_Down)
		{
		charCount = 1;
		buffer[0] = kJDownArrow;
		buffer[1] = '\0';
		}

	// the control modifier causes these to be mis-interpreted

	else if (XK_space <= keySym && keySym <= XK_question)
		{
		charCount = 1;
		buffer[0] = keySym;
		buffer[1] = '\0';
		}
	else if (XK_KP_0 <= keySym && keySym <= XK_KP_9)
		{
		charCount = 1;
		buffer[0] = '0' + (keySym - XK_KP_0);
		buffer[1] = '\0';
		}

	// handle character modifiers
	// (may modify buffer, so must be after simple translations)

	if (!JXPreprocessKeyPress(keySym, buffer))
		{
		return;
		}

	// dispatch key

	unsigned int state;
	const JBoolean foundState = JXGetButtonAndModifierStates(xEvent, itsDisplay, &state);
	assert( foundState );

	const JXKeyModifiers modifiers(itsDisplay, state);

	// Get current state without shift
	JXKeyModifiers noShift(modifiers);
	noShift.SetState(kJXShiftKeyIndex, kJFalse);

	// Don't treat caps/num/scroll-locks as modifiers
	noShift.SetState(kJXShiftLockKeyIndex, kJFalse);
	noShift.SetState(kJXNumLockKeyIndex, kJFalse);
	noShift.SetState(kJXScrollLockKeyIndex, kJFalse);
	const JBoolean modsOff = noShift.AllOff();
	const JBoolean shiftOn = JBoolean(modifiers.shift());

	JXDisplay* display = itsDisplay;	// need local copy, since we might be deleted
	Display* xDisplay  = *display;
	Window xWindow     = itsXWindow;

	// ESC cancels Drag-And-Drop

	if (charCount == 1 && buffer[0] == kJEscapeKey &&
		(GetDNDManager())->IsDragging())
		{
		if ((GetDNDManager())->CancelDND())
			{
			EndDrag(itsMouseContainer, JPoint(0,0), JXButtonStates(state), modifiers);
			}

		// We won't dispatch any motion events until the user releases
		// the mouse, but the user needs some reassurance that the drop
		// really has been cancelled.

		DisplayXCursor(kJXDefaultCursor);
		}

	// We check WillAcceptFocus() because we don't want to send keypresses
	// to invisible or inactive widgets.

	else if (keySym == XK_Tab && itsFocusWidget != NULL &&
			 itsFocusWidget->WillAcceptFocus() &&
			 (( modsOff && !shiftOn && itsFocusWidget->WantsTab()) ||
			  ( modsOff && shiftOn && itsFocusWidget->WantsShiftTab()) ||
			  (!modsOff && itsFocusWidget->WantsModifiedTab())))
		{
		// We send tab directly to the focus widget so it
		// doesn't get lost as a shortcut.

		DeactivateHint();
		itsFocusWidget->HandleKeyPress(kJTabKey, modifiers);
		}
	else if (keySym == XK_Tab)
		{
		SwitchFocus(modifiers.shift());
		}
	else if (IsShortcut(keySym, state))
		{
		// IsShortcut() dispatches event
		}
	else if (itsFocusWidget != NULL && itsFocusWidget->WillAcceptFocus())
		{
		DeactivateHint();

		if (charCount > 0)
			{
#ifdef X_HAVE_UTF8_STRING
			// Convert buffer to UTF16
			JString utf8(buffer, charCount);
			JString16 utf16;
			utf16.FromUTF8(utf8);
			JSize utf16Count = utf16.GetLength();
			for (JIndex i=0; i<utf16Count; i++)
				{
				if (utf16[i] < 0x100)
					itsFocusWidget->HandleKeyPress((unsigned char) utf16[i], modifiers);
				else
				{
					// Look for JTEBase16 and use special utf16 key input
				}
				if (utf16Count > 1 && i < utf16Count-1 &&
					!JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
					break;
					}
				}
#else
			// We are going to assume UTF-8 locale so characters returned
			// are UTF-8 and we must convert to UCS2 (int)
#if 0
			for (JIndex i=0; i<charCount; i++)
				{
				itsFocusWidget->HandleKeyPress((unsigned char) buffer[i], modifiers);
				if (charCount > 1 && i < charCount-1 &&
					!JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
					break;
					}
				}
#else
			if (charCount == 1)
				itsFocusWidget->HandleKeyPress((unsigned char) buffer[0], modifiers);
			else if (charCount > 1)
				{
				buffer[charCount] = '\0';
				JString16 ucs2;
				ucs2.FromUTF8(buffer);
				JSize ucsCount = ucs2.GetLength();
				const JCharacter16* ucsBuffer = ucs2.GetCString();
				for (JIndex i=0; i<ucsCount; i++)
					{
					itsFocusWidget->HandleKeyPress(ucsBuffer[i], modifiers);
					if (ucsCount > 1 && i < ucsCount-1 &&
						!JXDisplay::WindowExists(display, xDisplay, xWindow))
						{
						break;
						}
					}
				}
#endif
#endif
			}
		else
			{
#ifdef KEYSYM2UCS
			int codepoint = keysym2ucs(keySym);
			if (codepoint != -1)
				itsFocusWidget->HandleKeyPress(codepoint, modifiers);
			else
#endif
			itsFocusWidget->HandleKeyPress(keySym, modifiers);
			}
		}

	if (JXDisplay::WindowExists(display, xDisplay, xWindow))
		{
		Update();
		}
}

/******************************************************************************
 TossKeyRepeatEvents (private)

	If the user holds down a key, we don't want to accumulate a backlog of
	KeyPress,KeyRelease events.

 ******************************************************************************/

void
JXWindow::TossKeyRepeatEvents
	(
	const unsigned int	keycode,
	const unsigned int	state,
	const KeySym		keySym
	)
{
	if (keySym != XK_Left  && keySym != XK_KP_Left &&
		keySym != XK_Up    && keySym != XK_KP_Up &&
		keySym != XK_Right && keySym != XK_KP_Right &&
		keySym != XK_Down  && keySym != XK_KP_Down &&

		keySym != XK_Page_Up   && keySym != XK_KP_Page_Up &&
		keySym != XK_Page_Down && keySym != XK_KP_Page_Down &&

		keySym != XK_BackSpace && keySym != XK_Delete)
		{
		return;
		}

	XEvent xEvent;
	while (XPending(*itsDisplay) > 0)
		{
		XPeekEvent(*itsDisplay, &xEvent);
		if ((xEvent.type == KeyPress || xEvent.type == KeyRelease) &&
			xEvent.xkey.keycode == keycode &&
			xEvent.xkey.state   == state)
			{
			XNextEvent(*itsDisplay, &xEvent);
			}
		else
			{
			break;
			}
		}
}

/******************************************************************************
 DeactivateHint

 ******************************************************************************/

void
JXWindow::DeactivateHint()
{
	if (itsCurrHintMgr != NULL)
		{
		itsCurrHintMgr->Deactivate();
		itsCurrHintMgr = NULL;
		}
}

/******************************************************************************
 RegisterFocusWidget

 ******************************************************************************/

void
JXWindow::RegisterFocusWidget
	(
	JXWidget* widget
	)
{
	if (!itsFocusList->Includes(widget))
		{
		itsFocusList->Append(widget);
		if (itsFocusList->GetElementCount() == 1)
			{
			assert( itsFocusWidget == NULL );
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 UnregisterFocusWidget

 ******************************************************************************/

void
JXWindow::UnregisterFocusWidget
	(
	JXWidget* widget
	)
{
	if (!itsIsDestructingFlag)
		{
		itsFocusList->Remove(widget);
		if (widget == itsFocusWidget)
			{
			itsFocusWidget = NULL;
			SwitchFocus(kJFalse);
			}
		}
}

/******************************************************************************
 SwitchFocusToFirstWidget

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToFirstWidget()
{
	JXWidget* firstWidget;
	if (!FindNextFocusWidget(0, &firstWidget))
		{
		return kJTrue;
		}

	if (itsFocusWidget == firstWidget)
		{
		return kJTrue;
		}
	else if (UnfocusCurrentWidget())
		{
		itsFocusWidget = firstWidget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SwitchFocusToFirstWidgetWithAncestor

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToFirstWidgetWithAncestor
	(
	JXContainer* ancestor
	)
{
	JXWidget* firstWidget = NULL;

	const JSize count = itsFocusList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXWidget* widget = itsFocusList->NthElement(i);
		if (widget->WillAcceptFocus() &&
			ancestor->IsAncestor(widget))
			{
			firstWidget = widget;
			break;
			}
		}

	if (firstWidget == NULL || itsFocusWidget == firstWidget)
		{
		return kJTrue;
		}
	else if (UnfocusCurrentWidget())
		{
		itsFocusWidget = firstWidget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 OKToUnfocusCurrentWidget

 ******************************************************************************/

JBoolean
JXWindow::OKToUnfocusCurrentWidget()
	const
{
	if (itsFocusWidget == NULL ||
		itsFocusWidget->OKToUnfocus())
		{
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UnfocusCurrentWidget

 ******************************************************************************/

JBoolean
JXWindow::UnfocusCurrentWidget()
{
	if (itsFocusWidget == NULL)
		{
		return kJTrue;
		}
	else if (itsFocusWidget->OKToUnfocus())
		{
		KillFocus();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 KillFocus

	*** This does not check for valid input.  Use with caution!

 ******************************************************************************/

void
JXWindow::KillFocus()
{
	if (itsFocusWidget != NULL)
		{
		JXWidget* origWidget = itsFocusWidget;
		itsFocusWidget = NULL;		// clear this first
		origWidget->NotifyFocusLost();
		}
}

/******************************************************************************
 SwitchFocusToWidget

 ******************************************************************************/

JBoolean
JXWindow::SwitchFocusToWidget
	(
	JXWidget* widget
	)
{
	if (itsFocusWidget == widget)
		{
		return kJTrue;
		}
	else if (itsFocusList->Includes(widget) &&
			 widget->WillAcceptFocus() &&
			 UnfocusCurrentWidget())
		{
		itsFocusWidget = widget;
		itsFocusWidget->Focus(0);
		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 SwitchFocus (private)

 ******************************************************************************/

void
JXWindow::SwitchFocus
	(
	const JBoolean backward
	)
{
	if (itsFocusList->IsEmpty())
		{
		// nothing to do
		assert( itsFocusWidget == NULL );
		}
	else if (itsFocusWidget == NULL && backward)
		{
		if (FindPrevFocusWidget(0, &itsFocusWidget))
			{
			itsFocusWidget->Focus(0);
			}
		}
	else if (itsFocusWidget == NULL)
		{
		if (FindNextFocusWidget(0, &itsFocusWidget))
			{
			itsFocusWidget->Focus(0);
			}
		}
	else if (itsFocusWidget->OKToUnfocus())
		{
		JXWidget* widget = itsFocusWidget;
		KillFocus();

		// In the following code, we assume nothing about itsFocusList
		// because NotifyFocusLost() could do anything.  (Usually,
		// of course, it should not affect itsFocusList at all.)

		const JBoolean empty = itsFocusList->IsEmpty();

		JIndex currIndex;
		if (!empty && itsFocusList->Find(widget, &currIndex))
			{
			if (backward)
				{
				FindPrevFocusWidget(currIndex, &widget);
				}
			else
				{
				FindNextFocusWidget(currIndex, &widget);
				}
			}
		else if (!empty && backward)
			{
			FindPrevFocusWidget(0, &widget);
			}
		else if (!empty)
			{
			FindNextFocusWidget(0, &widget);
			}
		else
			{
			widget = NULL;
			}

		if (widget != NULL)
			{
			itsFocusWidget = widget;
			itsFocusWidget->Focus(0);
			}
		}
}

/******************************************************************************
 FindNextFocusWidget (private)

	Look forward in the list to find a widget after the specified one.
	If nothing can be found, sets *widget = NULL and returns kJFalse.

	(startIndex == 0) => start at beginning of list

 ******************************************************************************/

JBoolean
JXWindow::FindNextFocusWidget
	(
	const JIndex	origStartIndex,
	JXWidget**		focusWidget
	)
	const
{
	*focusWidget = NULL;

	const JSize count = itsFocusList->GetElementCount();
	if (count == 0)
		{
		return kJFalse;
		}

	JIndex startIndex = origStartIndex;
	if (startIndex == 0 || startIndex > count)
		{
		startIndex = count;
		}

	JIndex i = startIndex+1;
	if (i > count)
		{
		i = 1;
		}

	while (1)
		{
		JXWidget* widget = itsFocusList->NthElement(i);
		if (widget->WillAcceptFocus())
			{
			*focusWidget = widget;
			return kJTrue;
			}
		else if (i == startIndex)
			{
			return kJFalse;
			}

		i++;
		if (i > count)
			{
			i = 1;
			}
		}
}

/******************************************************************************
 FindPrevFocusWidget (private)

	Look backwards in the list to find a widget in front of the specified one.
	If nothing can be found, sets *widget = NULL and returns kJFalse.

	(startIndex == 0) => start at end of list

 ******************************************************************************/

JBoolean
JXWindow::FindPrevFocusWidget
	(
	const JIndex	origStartIndex,
	JXWidget**		focusWidget
	)
	const
{
	*focusWidget = NULL;

	const JSize count = itsFocusList->GetElementCount();
	if (count == 0)
		{
		return kJFalse;
		}

	JIndex startIndex = origStartIndex;
	if (startIndex == 0 || startIndex > count)
		{
		startIndex = 1;
		}

	JIndex i = startIndex-1;
	if (i == 0)
		{
		i = count;
		}

	while (1)
		{
		JXWidget* widget = itsFocusList->NthElement(i);
		if (widget->WillAcceptFocus())
			{
			*focusWidget = widget;
			return kJTrue;
			}
		else if (i == startIndex)
			{
			return kJFalse;
			}

		i--;
		if (i == 0)
			{
			i = count;
			}
		}
}

/******************************************************************************
 InstallShortcut

	To specify a control character, you must pass in the unmodified character
	along with modifiers.control().  e.g. Pass in 'A',control instead
	of JXCtrl('A'),control.

 ******************************************************************************/

JBoolean
JXWindow::InstallShortcut
	(
	JXWidget*				widget,
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	return InstallShortcut(Shortcut(widget, key, modifiers.GetState()));
}

/******************************************************************************
 InstallMenuShortcut

 ******************************************************************************/

JBoolean
JXWindow::InstallMenuShortcut
	(
	JXTextMenu*				menu,
	const JIndex			menuItem,
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	return InstallShortcut(Shortcut(menu, menuItem, key, modifiers.GetState()));
}

/******************************************************************************
 InstallShortcut (private)

	Returns kJTrue if shortcut was successfully registered.

	ShiftLock is always ignored.

 ******************************************************************************/

JBoolean
JXWindow::InstallShortcut
	(
	const Shortcut& origShortcut
	)
{
	Shortcut s = origShortcut;
	s.state    = JXKeyModifiers::SetState(itsDisplay, s.state,
										  kJXShiftLockKeyIndex, kJFalse);

	const JBoolean inserted = itsShortcuts->InsertSorted(s, kJFalse);

	// For characters other than the alphabet or tab, return, etc., we
	// never know whether or not the Shift key is required in order to type
	// them.  We therefore install both cases.

	if (inserted &&
		0 < origShortcut.key && origShortcut.key <= 255 &&
		!isalpha(origShortcut.key) && !iscntrl(origShortcut.key))
		{
		s.state = JXKeyModifiers::ToggleState(itsDisplay, s.state, kJXShiftKeyIndex);
		itsShortcuts->InsertSorted(s, kJFalse);
		}

	return inserted;
}

/******************************************************************************
 MenuItemInserted

	Update the item indices of the shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemInserted
	(
	JXTextMenu*		menu,
	const JIndex	newItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem >= newItem)
			{
			(s.menuItem)++;
			itsShortcuts->SetElement(i, s);
			}
		}
}

/******************************************************************************
 MenuItemRemoved

	Remove the shortcuts for the item and update the item indices of the
	other shortcuts.

 ******************************************************************************/

void
JXWindow::MenuItemRemoved
	(
	JXTextMenu*		menu,
	const JIndex	oldItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem > oldItem)
			{
			(s.menuItem)--;
			itsShortcuts->SetElement(i, s);
			}
		else if (s.menu == menu && s.menuItem == oldItem)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 InstallShortcuts

	Parses the given string to get the shortcuts.  A caret (^) indicates
	that the next character is a control character.  A hash (#) indicates that
	the next character requires the Meta modifier.  F1 - F35 are parsed into
	function keys.

 ******************************************************************************/

void
JXWindow::InstallShortcuts
	(
	JXWidget*			widget,
	const JCharacter*	list
	)
{
	if (JStringEmpty(list))
		{
		return;
		}

	JXKeyModifiers modifiers(itsDisplay);

	const JSize length = strlen(list);
	for (JIndex i=0; i<length; i++)
		{
		const int c = (unsigned char) list[i];
		JSize fnIndex;
		if (c == '^')
			{
			i++;
			if (i < length)
				{
				const int c1 = toupper((unsigned char) list[i]);
				const int c2 = tolower((unsigned char) list[i]);
				modifiers.SetState(kJXControlKeyIndex, kJTrue);		// e.g. Ctrl-M
				InstallShortcut(widget, c1, modifiers);
				if (c2 != c1)
					{
					InstallShortcut(widget, c2, modifiers);
					}
				modifiers.SetState(kJXControlKeyIndex, kJFalse);	// e.g. return key
				InstallShortcut(widget, (unsigned char) JXCtrl(c1), modifiers);
				}
			}
		// cd: Added option for alt-key shortcut
		else if (c == '@')
			{
			i++;
			if (i < length)
				{
				const int c1 = toupper((unsigned char) list[i]);
				const int c2 = tolower((unsigned char) list[i]);
				modifiers.SetState(kJXAltKeyIndex, kJTrue);
				InstallShortcut(widget, c1, modifiers);
				if (c2 != c1)
					{
					InstallShortcut(widget, c2, modifiers);
					}
				modifiers.SetState(kJXAltKeyIndex, kJFalse);
				}
			}
		else if (c == '#')
			{
			i++;
			if (i < length)
				{
				const int c1 = toupper((unsigned char) list[i]);
				const int c2 = tolower((unsigned char) list[i]);
				modifiers.SetState(kJXMetaKeyIndex, kJTrue);
				InstallShortcut(widget, c1, modifiers);
				if (c2 != c1)
					{
					InstallShortcut(widget, c2, modifiers);
					}
				modifiers.SetState(kJXMetaKeyIndex, kJFalse);
				}
			}
		else if (c == 'F' &&
				 (fnIndex = strtoul(list+i+1, NULL, 10)) > 0 &&
				 fnIndex <= 35)
			{
			InstallShortcut(widget, XK_F1 + fnIndex-1, modifiers);
			i++;
			if (fnIndex >= 10)
				{
				i++;
				}
			}
		else
			{
			InstallShortcut(widget, c, modifiers);
			}
		}
}

/******************************************************************************
 GetULShortcutIndex (static)

	Returns the index into label of the first shortcut character in
	the given list.  Returns zero if the first shortcut character is not
	found in label.  We return zero rather than JBoolean because
	JXWindowPainter::String() accepts zero to mean "no shortcut".

	We perform a case-insensitive search first for "^c", then " c",
	then "[^a-z]c", and finally anywhere.

	For convenience, list can be NULL.

 ******************************************************************************/

JIndex
JXWindow::GetULShortcutIndex
	(
	const JString&	label,
	const JString*	list
	)
{
	if (label.IsEmpty() || list == NULL || list->IsEmpty())
		{
		return 0;
		}

	JCharacter c = list->GetCharacter(1);
	if (c == '^' || c == '#' || c == '@')		// cd: Added option for alt-key shortcut
		{
		if (list->GetLength() < 2)
			{
			return 0;
			}
		c = list->GetCharacter(2);
		}
	c = tolower(c);

	if (c == tolower(label.GetFirstCharacter()))
		{
		return 1;
		}

	JIndex bdryIndex = 0, anyIndex = 0;
	const JSize length = label.GetLength();
	for (JIndex i=2; i<=length; i++)
		{
		if (c == tolower(label.GetCharacter(i)))
			{
			if (isspace(label.GetCharacter(i-1)))
				{
				return i;
				}
			else if (bdryIndex == 0 && !isalpha(label.GetCharacter(i-1)))
				{
				bdryIndex = i;
				}
			else if (anyIndex == 0)
				{
				anyIndex = i;
				}
			}
		}

	return (bdryIndex > 0 ? bdryIndex :
			(anyIndex > 0 ? anyIndex : 0));
}

/******************************************************************************
 ClearShortcuts

 ******************************************************************************/

void
JXWindow::ClearShortcuts
	(
	JXWidget* widget
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.widget == widget)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 ClearMenuShortcut

 ******************************************************************************/

void
JXWindow::ClearMenuShortcut
	(
	JXTextMenu*		menu,
	const JIndex	menuItem
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu && s.menuItem == menuItem)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 ClearAllMenuShortcuts

 ******************************************************************************/

void
JXWindow::ClearAllMenuShortcuts
	(
	JXTextMenu* menu
	)
{
	const JSize count = itsShortcuts->GetElementCount();
	for (JIndex i=count; i>=1; i--)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.menu == menu)
			{
			itsShortcuts->RemoveElement(i);
			}
		}
}

/******************************************************************************
 IsShortcut (private)

 ******************************************************************************/

//#include <stdio.h>

JBoolean
JXWindow::IsShortcut
	(
	const KeySym		keySym,
	const unsigned int	origState
	)
{
	int key            = keySym;
	unsigned int state = origState;

//	printf("IsShortcut: origState: %X\n", state);

	JXKeyModifiers modifiers(itsDisplay, state);
	modifiers.SetState(kJXNumLockKeyIndex, kJFalse);
	modifiers.SetState(kJXScrollLockKeyIndex, kJFalse);
	if (modifiers.shiftLock())
		{
		key = tolower(key);
		modifiers.SetState(kJXShiftLockKeyIndex, kJFalse);
		}
	state = modifiers.GetState();

	if (0 < key && key <= 255)
		{
		key = tolower(key);
		}
	else if (key == XK_Return || key == XK_KP_Enter)
		{
		key = '\r';
		}
	else if (key == XK_Escape)
		{
		key = kJEscapeKey;
		}
	else if (keySym == XK_KP_Tab || keySym == XK_ISO_Left_Tab)
		{
		key = kJTabKey;
		}
	else if (keySym == XK_Left || keySym == XK_KP_Left)
		{
		key = kJLeftArrow;
		}
	else if (keySym == XK_Up || keySym == XK_KP_Up)
		{
		key = kJUpArrow;
		}
	else if (keySym == XK_Right || keySym == XK_KP_Right)
		{
		key = kJRightArrow;
		}
	else if (keySym == XK_Down || keySym == XK_KP_Down)
		{
		key = kJDownArrow;
		}

//	printf("IsShortcut: newState: %X\n\n", state);

	Shortcut target(NULL, key, state);
	JIndex i;
	const JBoolean found = itsShortcuts->SearchSorted(target, JOrderedSetT::kAnyMatch, &i);

	if (found)
		{
		const Shortcut s = itsShortcuts->GetElement(i);
		if (s.widget != NULL && (s.widget)->WillAcceptShortcut())
			{
			(s.widget)->HandleShortcut(s.key, modifiers);
			}
		else if (s.menu != NULL)
			{
			(s.menu)->HandleNMShortcut(s.menuItem, modifiers);
			}
		}

	// We always return kJTrue if it is a shortcut, even if the Widget didn't
	// want to accept it.  This guarantees that keys will always behave
	// the same.  Otherwise, a deactivated button's shortcut would go to the
	// active input field.

	return found;
}

/******************************************************************************
 CompareShortcuts (static private)

	Sorts by key, then by state.

 ******************************************************************************/

JOrderedSetT::CompareResult
JXWindow::CompareShortcuts
	(
	const Shortcut& s1,
	const Shortcut& s2
	)
{
	if (s1.key < s2.key)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (s1.key > s2.key)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (s1.state < s2.state)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (s1.state > s2.state)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

/******************************************************************************
 HandleMapNotify (private)

 ******************************************************************************/

void
JXWindow::HandleMapNotify
	(
	const XMapEvent& mapEvent
	)
{
	itsIsMappedFlag = kJTrue;

	if (itsIsMenuFlag)
		{
		// Menu windows are not touched by the window manager.
		return;
		}

	// focus to the window for convenience

	if (itsFocusWhenShowFlag)
		{
		(itsDisplay->GetMenuManager())->CloseCurrentMenus();	// avoid deadlock
		RequestFocus();
		}

	if (itsIsDockedFlag)
		{
		itsIsIconifiedFlag = kJFalse;
		return;
		}

	// broadcast whether window is iconified or deiconified

	HandleWMStateChange();

	// The user only expects the window to start iconified the first time.

	SetWindowStateHint(NormalState);
}

/******************************************************************************
 HandleUnmapNotify (private)

 ******************************************************************************/

void
JXWindow::HandleUnmapNotify
	(
	const XUnmapEvent& unmapEvent
	)
{
	itsIsMappedFlag = kJFalse;

	if (IsVisible() && !itsIsIconifiedFlag)
		{
		itsIsIconifiedFlag = kJTrue;
		Broadcast(Iconified());
		}

//	cout << "JXWindow::HandleUnmapNotify: " << itsIsMappedFlag << ' ' << itsIsIconifiedFlag << endl;
}

/******************************************************************************
 HandleWMStateChange (private)

 ******************************************************************************/

void
JXWindow::HandleWMStateChange()
{
	if (itsIsMenuFlag || itsIsDockedFlag || !itsIsMappedFlag)
		{
		// Menu windows are not touched by the window manager.
		return;
		}

	// broadcast whether window is iconified or deiconified

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* xdata;

	const int result =
		XGetWindowProperty(*itsDisplay, itsXWindow, itsDisplay->GetWMStateXAtom(),
						   0, LONG_MAX, False, AnyPropertyType,
						   &actualType, &actualFormat,
						   &itemCount, &remainingBytes, &xdata);
	if (actualType != itsDisplay->GetWMStateXAtom() || actualFormat != 32)
		{
#ifndef _J_CYGWIN
		cerr << endl;
		cerr << "Error detected in JXWindow::HandleMapNotify():" << endl;
		cerr << "Your window manager is not setting the WM_STATE property correctly!" << endl;
		cerr << endl;
//		JXApplication::Abort(JXDocumentManager::kServerDead, kJFalse);
#endif

		if (itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			Refresh();
			}
		}
	else
		{
		assert( remainingBytes == 0 );

		if (*reinterpret_cast<long*>(xdata) == NormalState && itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJFalse;
			Broadcast(Deiconified());
			Refresh();
			}
		else if (*reinterpret_cast<long*>(xdata) == IconicState && !itsIsIconifiedFlag)
			{
			itsIsIconifiedFlag = kJTrue;
			Broadcast(Iconified());
			}
		}

//	cout << "JXWindow::HandleWMStateChange: " << itsIsMappedFlag << ' ' << itsIsIconifiedFlag << endl;

	XFree(xdata);
}

/******************************************************************************
 CheckForMapOrExpose

	For use by blocking loops that want to refresh a particular window.
	This isn't safe in general because it can invoke huge amounts of
	code.  Use with caution.

 ******************************************************************************/

void
JXWindow::CheckForMapOrExpose()
{
	XEvent xEvent;
	while (XCheckIfEvent(*itsDisplay, &xEvent, GetNextMapOrExposeEvent,
						 reinterpret_cast<char*>(&itsXWindow)))
		{
		HandleEvent(xEvent);
		}
}

// static private

Bool
JXWindow::GetNextMapOrExposeEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	const Window window    = (event->xany).window;
	const Window itsWindow = *(reinterpret_cast<Window*>(arg));
	if (window == itsWindow &&
		(event->type == MapNotify || event->type == UnmapNotify ||
		 event->type == Expose))
		{
		return True;
		}
	else
		{
		return False;
		}
}

/******************************************************************************
 GetDockWindow

 ******************************************************************************/

inline JBoolean
JXWindow::GetDockWindow
	(
	JXWindow** window
	)
	const
{
	return itsDisplay->FindXWindow(itsDockXWindow, window);
}

/******************************************************************************
 Dock

	dock can be NULL.  We take an X Window instead of a JXWindow in order to
	allow JXWindows to be docked in other program's windows.

	geom must be in the local coordinates of parent.

	Returns kJFalse if the window cannot be docked.

 ******************************************************************************/

JBoolean
JXWindow::Dock
	(
	JXDockWidget*	dock,
	const Window	parent,
	const JRect&	geom,
	JPoint*			minSize
	)
{
	if (itsIsMenuFlag || itsMainWindow != NULL ||
		geom.width() < itsMinSize.x || geom.height() < itsMinSize.y)
		{
		return kJFalse;
		}

	if (itsIsDockedFlag)
		{
		Broadcast(Undocked());
		}
	else
		{
		itsUndockedGeom = itsBounds;
		itsUndockedGeom.Shift(itsDesktopLoc);
		itsUndockedWMFrameLoc = itsWMFrameLoc;

		Deiconify();
		}

	XReparentWindow(*itsDisplay, itsXWindow, parent, geom.left, geom.top);
	JXDockWindowTask::Dock(this, parent, JPoint(geom.left, geom.top));

	itsIsDockedFlag = kJTrue;
	UndockedSetSize(geom.width(), geom.height());	// after setting itsIsDockedFlag

	itsDockXWindow = parent;
	itsDockWidget  = dock;
	Broadcast(Docked());

	*minSize = itsMinSize;
	return kJTrue;
}

/******************************************************************************
 Undock

 ******************************************************************************/

void
JXWindow::Undock()
{
	if (itsIsDockedFlag)
		{
		const JBoolean wasVisible = IsVisible();

		if (itsDockWidget != NULL)
			{
			(itsDockWidget->GetDockDirector())->ClearFocusWindow(this);
			}

		Hide();
		itsIsMappedFlag = kJFalse;

		XReparentWindow(*itsDisplay, itsXWindow, itsDisplay->GetRootWindow(),
						itsWMFrameLoc.x, itsWMFrameLoc.y);

		itsIsDockedFlag = kJFalse;
		itsDockXWindow  = None;
		itsDockWidget   = NULL;
		Place(itsUndockedWMFrameLoc.x, itsUndockedWMFrameLoc.y);
		SetSize(itsUndockedGeom.width(), itsUndockedGeom.height());

		if (wasVisible)
			{
			Show();
			}

		Broadcast(Undocked());
		}
}

/******************************************************************************
 UndockAllChildWindows

 ******************************************************************************/

void
JXWindow::UndockAllChildWindows()
{
	if (itsChildWindowList != NULL)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			JXWindow* w;
			if (itsDisplay->FindXWindow(info.xWindow, &w))
				{
				w->Undock();
				}
			}
		}
}

/******************************************************************************
 UpdateChildWindowList (private)

 ******************************************************************************/

void
JXWindow::UpdateChildWindowList
	(
	const Window	xWindow,
	const JBoolean	add
	)
{
	if (add && itsChildWindowList == NULL)
		{
		itsChildWindowList = new JArray<ChildWindowInfo>;
		assert( itsChildWindowList != NULL );
		}

	if (itsChildWindowList != NULL)
		{
		// check if xWindow is already in the list

		JIndex index = 0;

		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				index = i;
				break;
				}
			}

		// add/remove xWindow

		XWindowAttributes attr;
		if (add && index == 0 &&
			XGetWindowAttributes(*itsDisplay, xWindow, &attr))
			{
			ChildWindowInfo info(xWindow, JI2B(attr.map_state != IsUnmapped),
								 JRect(attr.y, attr.x,
									   attr.y+attr.height, attr.x+attr.width));
			itsChildWindowList->AppendElement(info);
			}
		else if (!add && index > 0)
			{
			itsChildWindowList->RemoveElement(index);
			}

		// toss list if it is empty

		if (itsChildWindowList->IsEmpty())
			{
			delete itsChildWindowList;
			itsChildWindowList = NULL;
			}
		}
}

/******************************************************************************
 SetChildWindowGeometry (private)

 ******************************************************************************/

void
JXWindow::SetChildWindowGeometry
	(
	const Window	xWindow,
	const JRect&	geom
	)
{
	if (itsChildWindowList != NULL)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				if (info.geom != geom)
					{
					info.geom = geom;
					itsChildWindowList->SetElement(i, info);
					}
				break;
				}
			}
		}
}

/******************************************************************************
 UpdateChildWindowGeometry (private)

 ******************************************************************************/

void
JXWindow::UpdateChildWindowGeometry()
{
	if (itsChildWindowList != NULL)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			JXWindow* w;
			if (itsDisplay->FindXWindow(info.xWindow, &w))
				{
				w->UpdateFrame();
				}
			}
		}
}

/******************************************************************************
 SetChildWindowVisible (private)

 ******************************************************************************/

void
JXWindow::SetChildWindowVisible
	(
	const Window	xWindow,
	const JBoolean	visible
	)
{
	if (itsChildWindowList != NULL)
		{
		const JSize count = itsChildWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			ChildWindowInfo info = itsChildWindowList->GetElement(i);
			if (info.xWindow == xWindow)
				{
				if (info.visible != visible)
					{
					info.visible = visible;
					itsChildWindowList->SetElement(i, info);
					}
				break;
				}
			}
		}
}

#define JTemplateType JXWindow::Shortcut
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXWindow::ChildWindowInfo
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXWindow
#include <JPtrArray.tmpls>
#undef JTemplateType
