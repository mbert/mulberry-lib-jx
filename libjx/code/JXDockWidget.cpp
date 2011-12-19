/******************************************************************************
 JXDockWidget.cpp

	Maintains a set of docked JXWindows.

	BASE CLASS = JXWidgetSet

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDockWidget.h>
#include <JXDockManager.h>
#include <JXDockDirector.h>
#include <JXHorizDockPartition.h>
#include <JXVertDockPartition.h>
#include <JXDockDragData.h>
#include <JXFileDocument.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXPartition.h>
#include <JXTabGroup.h>
#include <JXSelectTabTask.h>
#include <JXWindowPainter.h>
#include <JXGC.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kWindowWillNotFitID = "WindowWillNotFit::JXDockWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockWidget::JXDockWidget
	(
	JXDockDirector*		director,
	JXPartition*		partition,
	const JBoolean		isHorizPartition,
	JXTabGroup*			tabGroup,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h),
	itsDirector(director),
	itsPartition(partition),
	itsIsHorizFlag(isHorizPartition),
	itsTabGroup(tabGroup),
	itsID((JXGetDockManager())->GetUniqueDockID()),
	itsWindowList(NULL),
	itsMinSize(kDefaultMinSize, kDefaultMinSize),
	itsChildPartition(NULL)
{
	assert( itsDirector != NULL );
	assert( itsPartition != NULL );
	assert( itsTabGroup != NULL );

	if (!(JXGetDockManager())->IsReadingSetup())
		{
		(JXGetDockManager())->IDUsed(itsID);
		}

	ListenTo(itsTabGroup);
	ListenTo(itsTabGroup->GetCardEnclosure());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockWidget::~JXDockWidget()
{
	const JXDockManager::CloseDockMode mode = (JXGetDockManager())->GetCloseDockMode();
	if (itsWindowList != NULL && mode == JXDockManager::kUndockWindows)
		{
		// can't call UndockAll() because that calls UpdateMinSize()

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);
			w->Undock();
			}

		delete itsWindowList;
		}
	else if (itsWindowList != NULL)
		{
		assert( mode == JXDockManager::kCloseWindows );

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);
			w->Close();
			}

		delete itsWindowList;
		}
}

/******************************************************************************
 SetID

 ******************************************************************************/

void
JXDockWidget::SetID
	(
	const JIndex id
	)
{
	itsID = id;
	(JXGetDockManager())->IDUsed(id);
}

/******************************************************************************
 Dock

 ******************************************************************************/

JBoolean
JXDockWidget::Dock
	(
	JXWindowDirector*	d,
	const JBoolean		reportError
	)
{
	return Dock(d->GetWindow(), reportError);
}

JBoolean
JXDockWidget::Dock
	(
	JXWindow*		w,
	const JBoolean	reportError
	)
{
	if (itsChildPartition != NULL)
		{
		return kJFalse;
		}

	const JRect geom = GetApertureGlobal();
	JPoint minSize;
	if (w->Dock(this, (GetWindow())->GetXWindow(), geom, &minSize))
		{
		if (itsWindowList == NULL)
			{
			itsWindowList = new JPtrArray<JXWindow>(JPtrArrayT::kForgetAll);
			assert( itsWindowList != NULL );
			}

		const JSize count = itsWindowList->GetElementCount();
		assert( count == itsTabGroup->GetTabCount() );

		const JCharacter* title =
			JXFileDocument::SkipNeedsSavePrefix(w->GetTitle());

		JIndex index = count+1;
		for (JIndex i=1; i<=count; i++)
			{
			const JCharacter* t =
				JXFileDocument::SkipNeedsSavePrefix((itsWindowList->NthElement(i))->GetTitle());
			if (JStringCompare(title, t, kJFalse) < 0)
				{
				index = i;
				break;
				}
			}

		itsWindowList->InsertAtIndex(index, w);
		ListenTo(w);
		UpdateMinSize();

		itsTabGroup->InsertTab(index, w->GetTitle());

		// Can't call ShowTab() because window might be partially constructed,
		// so Activate() could blow up.

		JXSelectTabTask* task = new JXSelectTabTask(itsTabGroup, index);
		assert( task != NULL );
		(JXGetApplication())->InstallUrgentTask(task);

		return kJTrue;
		}
	else
		{
		if (reportError)
			{
			(JGetUserNotification())->ReportError(JGetString(kWindowWillNotFitID));
			}
		return kJFalse;
		}
}

/******************************************************************************
 TransferAll

	Transfers as many windows as possible to the target docking location.
	Returns kJFalse if not all windows could be transferred.

 ******************************************************************************/

JBoolean
JXDockWidget::TransferAll
	(
	JXDockWidget* target
	)
{
	JBoolean success = kJTrue;

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			if (!target->Dock(w))
				{
				success = kJFalse;
				}
			}
		}

	return success;
}

/******************************************************************************
 UndockAll

 ******************************************************************************/

void
JXDockWidget::UndockAll()
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsTabGroup->DeleteTab(1);

			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);
			w->Undock();
			}

		itsWindowList->RemoveAll();
		delete itsWindowList;
		itsWindowList = NULL;

		UpdateMinSize();
		}
}

/******************************************************************************
 CloseAll

 ******************************************************************************/

JBoolean
JXDockWidget::CloseAll()
{
	if (itsWindowList != NULL)
		{
		JXDisplay* display = GetDisplay();
		Display* xDisplay  = display->GetXDisplay();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=count; i>=1; i--)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			StopListening(w);

			Window xWindow = w->GetXWindow();
			if (w->Close())
				{
				if (JXDisplay::WindowExists(display, xDisplay, xWindow))
					{
					w->Undock();
					}
				else if ((JXGetApplication())->DisplayExists(xDisplay))
					{
					itsTabGroup->DeleteTab(i);
					itsWindowList->RemoveElement(i);
					}
				else
					{
					return kJFalse;
					}
				}
			else
				{
				UpdateMinSize();
				return kJFalse;
				}
			}

		delete itsWindowList;
		itsWindowList = NULL;

		UpdateMinSize();
		}

	return kJTrue;
}

/******************************************************************************
 GetHorizChildPartition

 ******************************************************************************/

JBoolean
JXDockWidget::GetHorizChildPartition
	(
	JXHorizDockPartition** p
	)
	const
{
	if (!itsIsHorizFlag && itsChildPartition != NULL)
		{
		*p = dynamic_cast(JXHorizDockPartition*, itsChildPartition);
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 GetVertChildPartition

 ******************************************************************************/

JBoolean
JXDockWidget::GetVertChildPartition
	(
	JXVertDockPartition** p
	)
	const
{
	if (itsIsHorizFlag && itsChildPartition != NULL)
		{
		*p = dynamic_cast(JXVertDockPartition*, itsChildPartition);
		assert( *p != NULL );
		return kJTrue;
		}
	else
		{
		*p = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 SetChildPartition

 ******************************************************************************/

void
JXDockWidget::SetChildPartition
	(
	JXPartition* p
	)
{
	assert( itsWindowList == NULL );

	if (itsChildPartition != NULL)
		{
		StopListening(itsChildPartition);
		}

	itsChildPartition = p;

	if (itsChildPartition != NULL)
		{
		ListenTo(itsChildPartition);
		}

	UpdateMinSize();
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept JXDockDragData.

 ******************************************************************************/

JBoolean
JXDockWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	// we only accept local drops

	if (source == NULL)
		{
		return kJFalse;
		}

	// we accept drops of type JXDockDragData

	const Atom minSizeAtom = (JXGetDockManager())->GetDNDMinSizeAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == minSizeAtom)
			{
			JXSelectionManager* selMgr = GetSelectionManager();
			JXDNDManager* dndMgr       = GetDNDManager();

			*action = dndMgr->GetDNDActionPrivateXAtom();

			// check if window will fit

			JBoolean acceptDrop = kJFalse;

			const Atom selectionName = dndMgr->GetDNDSelectionName();
			Atom returnType;
			unsigned char* data;
			JSize dataLength;
			JXSelectionManager::DeleteMethod delMethod;
			if (selMgr->GetData(selectionName, time, minSizeAtom,
								&returnType, &data, &dataLength, &delMethod))
				{
				if (returnType == XA_POINT)
					{
					XPoint* minSize = (XPoint*) data;
					if (GetApertureWidth()  >= minSize->x &&
						GetApertureHeight() >= minSize->y)
						{
						acceptDrop = kJTrue;
						}
					}
				selMgr->DeleteData(&data, delMethod);
				}

			return acceptDrop;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
JXDockWidget::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData((GetDNDManager())->GetDNDSelectionName(),
						time, (JXGetDockManager())->GetDNDWindowAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		JXWindow* window;
		if (returnType == XA_WINDOW &&
			(GetDisplay())->FindXWindow(* (Window*) data, &window))
			{
			Dock(window, kJTrue);
			}

		selMgr->DeleteData(&data, delMethod);
		}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	Remove the blue border.

 ******************************************************************************/

void
JXDockWidget::HandleDNDLeave()
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->Refresh();
			}
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXDockWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect ap = GetAperture();
	if (IsDNDTarget())
		{
		(p.GetGC())->SetSubwindowMode(IncludeInferiors);
		JXDrawDNDBorder(p, ap, 2);
		(p.GetGC())->SetSubwindowMode(ClipByChildren);
		}
	else
		{
		JXDrawDownFrame(p, ap, 2);
		}
}

/******************************************************************************
 BoundsMoved (virtual protected)

 ******************************************************************************/

void
JXDockWidget::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXWidgetSet::BoundsMoved(dx, dy);

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->UndockedMove(dx, dy);
			}
		}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
JXDockWidget::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXWidgetSet::BoundsResized(dw, dh);

	if (itsWindowList != NULL)
		{
		// Use SetSize() instead of AdjustSize() because JXWindow might
		// have max size, in which case deltas will be wrong.

		const JRect boundsG = GetBoundsGlobal();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->UndockedSetSize(boundsG.width(), boundsG.height());
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXDockWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXWindow* w;
	JIndex i;
	if (sender != GetWindow() && message.Is(JXWindow::kUndocked))
		{
		RemoveWindow(sender);
		StopListening(sender);
		}
	else if (FindWindow(sender, &w, &i))
		{
		if (message.Is(JXWindow::kMinSizeChanged))
			{
			const JPoint minSize = w->GetMinSize();
			const JRect boundsG  = GetBoundsGlobal();
			if (boundsG.width()  < minSize.x ||
				boundsG.height() < minSize.y)
				{
				w->Undock();
				}
			}
		else if (message.Is(JXWindow::kTitleChanged))
			{
			itsTabGroup->SetTabTitle(i, w->GetTitle());
			}
		}
	else if (sender == itsTabGroup && message.Is(JXTabGroup::kAppearanceChanged))
		{
		UpdateMinSize();
		}
	else if (sender == itsTabGroup->GetCardEnclosure() &&
			 message.Is(JXCardFile::kCardIndexChanged))
		{
		const JXCardFile::CardIndexChanged* info =
			dynamic_cast(const JXCardFile::CardIndexChanged*, &message);
		assert( info != NULL );

		JIndex index;
		if (info->GetCardIndex(&index))
			{
			JXWindow* w = itsWindowList->NthElement(index);
			(w->GetDirector())->Activate();
			w->RequestFocus();
			}
		}
	else
		{
		JXWidgetSet::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXDockWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (RemoveWindow(sender))
		{
		return;
		}
	else if (sender == itsChildPartition)
		{
		itsChildPartition = NULL;
		}
	else
		{
		JXWidgetSet::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 FindWindow (private)

 ******************************************************************************/

JBoolean
JXDockWidget::FindWindow
	(
	JBroadcaster*	sender,
	JXWindow**		window,
	JIndex*			index
	)
	const
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JXWindow* w = itsWindowList->NthElement(i);
			if (w == sender)
				{
				*window = w;
				*index  = i;
				return kJTrue;
				}
			}
		}

	*window = NULL;
	*index  = 0;
	return kJFalse;
}

/******************************************************************************
 RemoveWindow (private)

 ******************************************************************************/

JBoolean
JXDockWidget::RemoveWindow
	(
	JBroadcaster* sender
	)
{
	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (itsWindowList->NthElement(i) == sender)
				{
				itsTabGroup->DeleteTab(i);

				itsWindowList->RemoveElement(i);
				if (itsWindowList->IsEmpty())
					{
					delete itsWindowList;
					itsWindowList = NULL;
					}

				UpdateMinSize();
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 HasWindows

 ******************************************************************************/

JBoolean
JXDockWidget::HasWindows()
	const
{
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		return dynamic_cast(JXVertDockPartition*, itsChildPartition)->HasWindows();
		}
	else if (itsChildPartition != NULL)
		{
		return dynamic_cast(JXHorizDockPartition*, itsChildPartition)->HasWindows();
		}
	else
		{
		if (itsWindowList == NULL || itsWindowList->IsEmpty())
			{
			return kJFalse;
			}

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((itsWindowList->NthElement(i))->IsVisible())
				{
				return kJTrue;
				}
			}

		return kJFalse;
		}
}

/******************************************************************************
 CloseAllWindows

 ******************************************************************************/

JBoolean
JXDockWidget::CloseAllWindows()
{
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		return dynamic_cast(JXVertDockPartition*, itsChildPartition)->CloseAllWindows();
		}
	else if (itsChildPartition != NULL)
		{
		return dynamic_cast(JXHorizDockPartition*, itsChildPartition)->CloseAllWindows();
		}
	else
		{
		return CloseAll();
		}
}

/******************************************************************************
 GetMinSize

 ******************************************************************************/

JPoint
JXDockWidget::GetMinSize()
	const
{
	if (itsChildPartition != NULL && itsIsHorizFlag)
		{
		return dynamic_cast(JXVertDockPartition*, itsChildPartition)->CalcMinSize();
		}
	else if (itsChildPartition != NULL)
		{
		return dynamic_cast(JXHorizDockPartition*, itsChildPartition)->CalcMinSize();
		}
	else
		{
		return itsMinSize;
		}
}

/******************************************************************************
 UpdateMinSize (private)

	Only call this when itsWindowList changes.

 ******************************************************************************/

void
JXDockWidget::UpdateMinSize()
{
	itsMinSize.x = kDefaultMinSize;
	itsMinSize.y = kDefaultMinSize;

	if (itsWindowList != NULL)
		{
		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JPoint pt = (itsWindowList->NthElement(i))->GetMinSize();
			itsMinSize.x    = JMax(pt.x, itsMinSize.x);
			itsMinSize.y    = JMax(pt.y, itsMinSize.y);
			}
		}

	const JRect r1 = itsTabGroup->GetApertureGlobal();
	const JRect r2 = this->GetApertureGlobal();
	itsMinSize.x  += (r2.left - r1.left) + (r1.right - r2.right);
	itsMinSize.y  += (r2.top - r1.top)   + (r1.bottom - r2.bottom);

	if (itsIsHorizFlag)
		{
		itsPartition->SetMinCompartmentSize(itsTabGroup->GetEnclosure(), itsMinSize.x);
		}
	else
		{
		itsPartition->SetMinCompartmentSize(itsTabGroup->GetEnclosure(), itsMinSize.y);
		}
	itsDirector->UpdateMinSize();
}

#define JTemplateType JXDockWidget
#include <JPtrArray.tmpls>
#undef JTemplateType
