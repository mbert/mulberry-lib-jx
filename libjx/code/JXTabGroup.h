/******************************************************************************
 JXTabGroup.h

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTabGroup
#define _H_JXTabGroup

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCardFile.h>
#include <JPtrArray-JString.h>
#include <JFontStyle.h>

class JXTextMenu;

class JXTabGroup : public JXWidget
{
public:

	enum Edge
	{
		kTop,
		kLeft,
		kBottom,
		kRight
	};

public:

	JXTabGroup(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXTabGroup();

	virtual void	Activate();		// must call inherited

	JSize		GetTabCount() const;
	JBoolean	GetCurrentTabIndex(JIndex* index) const;

	JXWidgetSet*	InsertTab(const JIndex index, const JCharacter* title);
	JXWidgetSet*	PrependTab(const JCharacter* title);
	JXWidgetSet*	AppendTab(const JCharacter* title);

	void			InsertTab(const JIndex index, const JCharacter* title,
							  JXWidgetSet* card);
	void			PrependTab(const JCharacter* title, JXWidgetSet* card);
	void			AppendTab(const JCharacter* title, JXWidgetSet* card);

	JXWidgetSet*	RemoveTab(const JIndex index);
	void			DeleteTab(const JIndex index);
	void			KillFocusOnCurrentTab();

	const JString&	GetTabTitle(const JIndex index) const;
	void			SetTabTitle(const JIndex index, const JCharacter* title);

	JBoolean		ShowTab(const JIndex index);
	JBoolean		ShowTab(JXWidgetSet* card);

	Edge	GetTabEdge() const;
	void	SetTabEdge(const Edge edge);

	const JString&		GetFontName() const;
	void				SetFontName(const JCharacter* name);

	JSize				GetFontSize() const;
	void				SetFontSize(const JSize size);

	const JFontStyle&	GetFontStyle() const;
	void				SetFontStyle(const JFontStyle& style);

	void				SetFont(const JCharacter* name, const JSize size,
								const JFontStyle& style);

	JXContainer*	GetCardEnclosure();

	void		ReadSetup(istream& input);
	static void	SkipSetup(istream& input);
	void		WriteSetup(ostream& output) const;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum DragAction
	{
		kInvalidClick,
		kScrollUp,
		kScrollDown
	};

private:

	Edge				itsEdge;
	JString				itsFontName;
	JSize				itsFontSize;
	JFontStyle			itsFontStyle;
	JPtrArray<JString>*	itsTitles;
	JBoolean			itsCanScrollUpFlag;
	JRect				itsScrollUpRect;
	JBoolean			itsCanScrollDownFlag;
	JRect				itsScrollDownRect;
	JIndex				itsFirstDrawIndex;
	JIndex				itsLastDrawIndex;
	JArray<JRect>*		itsTabRects;		// 1 <=> itsFirstDrawIndex
	JXCardFile*			itsCardFile;
	JXTextMenu*			itsContextMenu;		// NULL until first used

	// used during dragging

	DragAction	itsDragAction;
	JBoolean	itsScrollUpPushedFlag;
	JBoolean	itsScrollDownPushedFlag;

private:

	void	UpdateAppearance();
	void	PlaceCardFile();
	void	DrawTabBorder(JXWindowPainter& p, const JRect& rect,
						  const JBoolean isSelected);
	void	DrawScrollButtons(JXWindowPainter& p, const JCoordinate lineHeight);
	void	ScrollWait(const JFloat delta) const;

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	ScrollUpToTab(const JIndex index);

	// not allowed

	JXTabGroup(const JXTabGroup& source);
	const JXTabGroup& operator=(const JXTabGroup& source);

public:

	// JBroadcaster messages

	static const JCharacter* kAppearanceChanged;

	class AppearanceChanged : public JBroadcaster::Message
		{
		public:

			AppearanceChanged()
				:
				JBroadcaster::Message(kAppearanceChanged)
				{ };
		};
};


/******************************************************************************
 GetTabCount

 ******************************************************************************/

inline JSize
JXTabGroup::GetTabCount()
	const
{
	return itsTitles->GetElementCount();
}

/******************************************************************************
 GetCurrentTabIndex

 ******************************************************************************/

inline JBoolean
JXTabGroup::GetCurrentTabIndex
	(
	JIndex* index
	)
	const
{
	return itsCardFile->GetCurrentCardIndex(index);
}

/******************************************************************************
 InsertTab

	The first version creates a new card, inserts it at the specified index,
	and returns it.

	The second version lets you provide the card, so that you can use
	a derived class to add intelligence.  The card's enclosure must be
	GetCardEnclosure().  We own the card after you give it to us.

 ******************************************************************************/

inline JXWidgetSet*
JXTabGroup::InsertTab
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	Refresh();
	itsTitles->InsertAtIndex(index, title);
	return itsCardFile->InsertCard(index);
}

inline void
JXTabGroup::InsertTab
	(
	const JIndex		index,
	const JCharacter*	title,
	JXWidgetSet*		card
	)
{
	Refresh();
	itsTitles->InsertAtIndex(index, title);
	itsCardFile->InsertCard(index, card);
}

/******************************************************************************
 PrependTab

 ******************************************************************************/

inline JXWidgetSet*
JXTabGroup::PrependTab
	(
	const JCharacter* title
	)
{
	return InsertTab(1, title);
}

inline void
JXTabGroup::PrependTab
	(
	const JCharacter*	title,
	JXWidgetSet*		card
	)
{
	InsertTab(1, title, card);
}

/******************************************************************************
 AppendTab

 ******************************************************************************/

inline JXWidgetSet*
JXTabGroup::AppendTab
	(
	const JCharacter* title
	)
{
	return InsertTab(GetTabCount()+1, title);
}

inline void
JXTabGroup::AppendTab
	(
	const JCharacter*	title,
	JXWidgetSet*		card
	)
{
	InsertTab(GetTabCount()+1, title, card);
}

/******************************************************************************
 DeleteTab

	Deletes the specified tab and everything on it.

 ******************************************************************************/

inline void
JXTabGroup::DeleteTab
	(
	const JIndex index
	)
{
	JXWidgetSet* card = RemoveTab(index);
	delete card;
}

/******************************************************************************
 KillFocusOnCurrentTab

	If one of the widgets on the current card has focus, we call KillFocus().
	This is useful when you want to discard the current card without saving
	its contents.

 ******************************************************************************/

inline void
JXTabGroup::KillFocusOnCurrentTab()
{
	itsCardFile->KillFocusOnCurrentCard();
}

/******************************************************************************
 Tab edge

 ******************************************************************************/

inline JXTabGroup::Edge
JXTabGroup::GetTabEdge()
	const
{
	return itsEdge;
}

inline void
JXTabGroup::SetTabEdge
	(
	const Edge edge
	)
{
	itsEdge = edge;
	UpdateAppearance();
}

/******************************************************************************
 Tab title

 ******************************************************************************/

inline const JString&
JXTabGroup::GetTabTitle
	(
	const JIndex index
	)
	const
{
	return *(itsTitles->NthElement(index));
}

inline void
JXTabGroup::SetTabTitle
	(
	const JIndex		index,
	const JCharacter*	title
	)
{
	*(itsTitles->NthElement(index)) = title;
	Refresh();
}

/******************************************************************************
 Get font info

 ******************************************************************************/

inline const JString&
JXTabGroup::GetFontName()
	const
{
	return itsFontName;
}

inline JSize
JXTabGroup::GetFontSize()
	const
{
	return itsFontSize;
}

inline const JFontStyle&
JXTabGroup::GetFontStyle()
	const
{
	return itsFontStyle;
}

/******************************************************************************
 GetCardEnclosure

	Returns the enclosure that must be used for all cards.

 ******************************************************************************/

inline JXContainer*
JXTabGroup::GetCardEnclosure()
{
	return itsCardFile;
}

/******************************************************************************
 UpdateAppearance (private)

 ******************************************************************************/

inline void
JXTabGroup::UpdateAppearance()
{
	itsFirstDrawIndex = 1;
	PlaceCardFile();
	Refresh();
	Broadcast(AppearanceChanged());
}

#endif
