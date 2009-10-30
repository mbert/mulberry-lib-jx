/******************************************************************************
 JXDockWidget.h

	Copyright � 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDockWidget
#define _H_JXDockWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JXWindowDirector;
class JXDockDirector;
class JXPartition;
class JXHorizDockPartition;
class JXVertDockPartition;
class JXTabGroup;

class JXDockWidget : public JXWidgetSet
{
public:

	enum
	{
		kDefaultMinSize = 10
	};

public:

	JXDockWidget(JXDockDirector* director, JXPartition* partition,
				 const JBoolean isHorizPartition, JXTabGroup* tabGroup,
				 JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXDockWidget();

	JXDockDirector*	GetDockDirector() const;
	JXTabGroup*		GetTabGroup() const;
	JPoint			GetMinSize() const;

	JBoolean	HasWindows() const;
	JBoolean	CloseAllWindows();		// recursive

	JBoolean	Dock(JXWindowDirector* d, const JBoolean reportError = kJFalse);
	JBoolean	Dock(JXWindow* w, const JBoolean reportError = kJFalse);
	JBoolean	TransferAll(JXDockWidget* target);
	void		UndockAll();
	JBoolean	CloseAll();				// not recursive

	JBoolean	GetHorizChildPartition(JXHorizDockPartition** p) const;
	JBoolean	GetVertChildPartition(JXVertDockPartition** p) const;
	void		SetChildPartition(JXPartition* p);

	JIndex	GetID() const;
	void	SetID(const JIndex id);

protected:

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);
	virtual void		HandleDNDLeave();

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	BoundsMoved(const JCoordinate dx, const JCoordinate dy);
	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXDockDirector*			itsDirector;
	JXPartition*			itsPartition;
	const JBoolean			itsIsHorizFlag;
	JXTabGroup*				itsTabGroup;
	JIndex					itsID;
	JPtrArray<JXWindow>*	itsWindowList;		// NULL if contains partition
	JPoint					itsMinSize;
	JXPartition*			itsChildPartition;	// NULL if contains docked windows

private:

	void		UpdateMinSize();
	JBoolean	FindWindow(JBroadcaster* sender, JXWindow** window, JIndex* index) const;
	JBoolean	RemoveWindow(JBroadcaster* sender);

	// not allowed

	JXDockWidget(const JXDockWidget& source);
	const JXDockWidget& operator=(const JXDockWidget& source);
};


/******************************************************************************
 GetDockDirector

 ******************************************************************************/

inline JXDockDirector*
JXDockWidget::GetDockDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetTabGroup

 ******************************************************************************/

inline JXTabGroup*
JXDockWidget::GetTabGroup()
	const
{
	return itsTabGroup;
}

/******************************************************************************
 GetID

 ******************************************************************************/

inline JIndex
JXDockWidget::GetID()
	const
{
	return itsID;
}

#endif
