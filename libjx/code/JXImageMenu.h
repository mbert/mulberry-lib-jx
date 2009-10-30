/******************************************************************************
 JXImageMenu.h

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImageMenu
#define _H_JXImageMenu

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXMenu.h>
#include <JXPM.h>

class JXImage;
class JXImageMenuData;

class JXImageMenu : public JXMenu
{
public:

	JXImageMenu(const JCharacter* title, const JSize columnCount,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(JXImage* image, const JBoolean menuOwnsImage,
				const JSize columnCount, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	JXImageMenu(const JSize columnCount, JXMenu* owner, const JIndex itemIndex,
				JXContainer* enclosure);

	virtual ~JXImageMenu();

	void			InsertItem(const JIndex index, JXImage* image,
							   const JBoolean menuOwnsImage,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* id = NULL);
	void			PrependItem(JXImage* image, const JBoolean menuOwnsImage,
								const JBoolean isCheckbox = kJFalse,
								const JBoolean isRadio = kJFalse,
								const JCharacter* id = NULL);
	void			AppendItem(JXImage* image, const JBoolean menuOwnsImage,
							   const JBoolean isCheckbox = kJFalse,
							   const JBoolean isRadio = kJFalse,
							   const JCharacter* id = NULL);

	const JXImage*	GetItemImage(const JIndex index) const;
	void			SetItemImage(const JIndex index, JXImage* image,
								 const JBoolean menuOwnsImage);
	void			SetItemImage(const JIndex index, const JXPM& data);

	// provides safe downcasting

	virtual JXImageMenu*		CastToJXImageMenu();
	virtual const JXImageMenu*	CastToJXImageMenu() const;

protected:

	virtual JXMenuDirector*	CreateMenuWindow(JXWindowDirector* supervisor);
	virtual void			AdjustPopupChoiceTitle(const JIndex index);

	JXImageMenuData*	GetIconMenuData();

private:

	JXImageMenuData*	itsIconMenuData;

private:

	void	JXImageMenuX(const JSize columnCount);

	// not allowed

	JXImageMenu(const JXImageMenu& source);
	const JXImageMenu& operator=(const JXImageMenu& source);
};


/******************************************************************************
 GetIconMenuData (protected)

 ******************************************************************************/

inline JXImageMenuData*
JXImageMenu::GetIconMenuData()
{
	return itsIconMenuData;
}

#endif
