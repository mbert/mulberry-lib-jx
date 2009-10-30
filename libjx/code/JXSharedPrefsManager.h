/******************************************************************************
 JXSharedPrefsManager.h

	Copyright © 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSharedPrefsManager
#define _H_JXSharedPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXTEBase.h>	// need defn of PartialWordModifier
#include <JXMenu.h>		// need defn of Style
#include <jTime.h>

class JPrefsFile;
class JXTimerTask;

class JXSharedPrefsManager : virtual public JBroadcaster
{
public:

	JXSharedPrefsManager();

	virtual ~JXSharedPrefsManager();

	JBoolean	WasNew() const;

	void	GetHelpManagerPrefs();
	void	SetHelpManagerPrefs();

	void	GetWebBrowserPrefs();
	void	SetWebBrowserPrefs();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	// do not change or deallocate ID's once they are assigned

	enum
	{
		kWriteLockID = 0,
		kWMPlacementStrategyID,
		kCopyWhenSelectID,
		kPWModifierID,
		kCaretFollowsScrollID,
		kWindowsHomeEndID,
		kLatestHelpMgrVersionID,
		kHelpMgrV3,
		kHelpMgrV4,
		kLatestWebBrowserVersionID,
		kWebBrowserV0,
		kAllowSpaceID,
		kWebBrowserV1,
		kMenuDisplayStyleID
	};

private:

	JPrefsFile*		itsFile;			// NULL unless working
	JBoolean		itsChangedFlag;		// used when re-entrant
	JXTimerTask*	itsUpdateTask;
	JString			itsSignalFileName;	// can be empty
	time_t			itsSignalModTime;	// last time signal file was modified
	JBoolean		itsWasNewFlag;		// kJTrue => we created the file

	// this section exists because JCore values cannot update themselves

	JBoolean						itsOrigWMStrategy;
	JBoolean						itsOrigCopyWhenSelectFlag;
	JXTEBase::PartialWordModifier	itsOrigPWMod;
	JBoolean						itsOrigCaretScrollFlag;
	JBoolean						itsOrigWindowsHomeEndFlag;
	JBoolean						itsOrigAllowSpaceFlag;
	JXMenu::Style					itsOrigMenuDisplayStyle;

private:

	JBoolean	Update();
	JBoolean	GetAll(JBoolean* isNew);

	void		PrivateSetWMPlacementStrategy();
	void		PrivateSetCopyWhenSelectFlag();
	void		PrivateSetPartialWordModifier();
	void		PrivateSetCaretFollowsScroll();
	void		PrivateSetWindowsHomeEnd();
	void		PrivateSetAllowSpaceFlag();
	void		PrivateSetMenuDisplayStyle();

	JBoolean	PrivateGetHelpManagerPrefs();
	JBoolean	PrivateSetHelpManagerPrefs();

	JBoolean	PrivateGetWebBrowserPrefs();
	JBoolean	PrivateSetWebBrowserPrefs();

	JBoolean	Open();
	void		Close(const JBoolean changed);
	void		NotifyChanged();
	void		SaveSignalModTime();

	// not allowed

	JXSharedPrefsManager(const JXSharedPrefsManager& source);
	JXSharedPrefsManager& operator=(const JXSharedPrefsManager& source);
};


/******************************************************************************
 WasNew

	Returns kJTrue if prefs stored by the application should be used.

 ******************************************************************************/

inline JBoolean
JXSharedPrefsManager::WasNew()
	const
{
	return itsWasNewFlag;
}

#endif
