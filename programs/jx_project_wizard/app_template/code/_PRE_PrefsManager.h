/******************************************************************************
 <PRE>PrefsManager.h

	Copyright © <Year> by <Company>. All rights reserved.

 *****************************************************************************/

#ifndef _H_<PRE>PrefsManager
#define _H_<PRE>PrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPrefsManager.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JString;
class <PRE>PrefsDialog;

// Preferences -- do not change ID's once they are assigned

enum
{
	k<PRE>ProgramVersionID = 1,
	k<PRE>gCSFSetupID,
	k<PRE>MainDirectorID,
	k<PRE>MainToolBarID
};

class <PRE>PrefsManager : public JXPrefsManager
{
public:

	<PRE>PrefsManager(JBoolean* isNew);

	virtual	~<PRE>PrefsManager();

	JString GetPrevVersionStr() const;
	void	EditPrefs();

	// called by <PRE>CleanUpBeforeSuddenDeath

	void CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	<PRE>PrefsDialog*	itsPrefsDialog;

private:

	void	SaveAllBeforeDestruct();

	// not allowed

	<PRE>PrefsManager(const <PRE>PrefsManager& source);
	const <PRE>PrefsManager& operator=(const <PRE>PrefsManager& source);
};

#endif
