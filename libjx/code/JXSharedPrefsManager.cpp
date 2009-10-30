/******************************************************************************
 JXSharedPrefsManager.cpp

	Stores preferences that are shared by all JX programs.  Since this must
	be compatible with all versions of JX, all older versions of the data
	must be written along with the latest version.  Programs that use an
	outdated version of JX cannot usually write to this file because they
	are unable to update all the relevant information.

	For single items like PartialWordModifier, the best method is to create
	another ID when values are added to the enum.  When the new version opens
	the file for the first time, it reads the original ID and creates the
	new one.  Old programs can still read and write the original ID, while
	new programs read and write the new ID.  (Old programs cannot read the
	new ID since it has illegal values.)

	Since JFileArray modifies the file every time it is opened, we use
	a separate signal file to notify other running programs when the settings
	change.  We use an idle task to check for signals from other programs.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2000 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSharedPrefsManager.h>
#include <JPrefsFile.h>
#include <JXHelpManager.h>
#include <JXWebBrowser.h>
#include <JXSaveFileInput.h>
#include <JXWindow.h>
#include <JXTimerTask.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kDataFileRoot   = "jx/globals";
static const JCharacter* kSignalFileName = "~/.jx/globals.signal";

const JSize kUpdateInterval = 1000;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSharedPrefsManager::JXSharedPrefsManager()
{
	itsFile = NULL;

	if (JExpandHomeDirShortcut(kSignalFileName, &itsSignalFileName))
		{
		JString path, name;
		JSplitPathAndName(itsSignalFileName, &path, &name);
		if (JCreateDirectory(path, 0700) != kJNoError)
			{
			itsSignalFileName.Clear();
			}
		}
	else
		{
		itsSignalFileName.Clear();
		}

	GetAll(&itsWasNewFlag);

	itsUpdateTask = new JXTimerTask(kUpdateInterval);
	assert( itsUpdateTask != NULL );
	(JXGetApplication())->InstallIdleTask(itsUpdateTask);
	ListenTo(itsUpdateTask);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSharedPrefsManager::~JXSharedPrefsManager()
{
	delete itsFile;
//	delete itsUpdateTask;	// already deleted by JXApplication!
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXSharedPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsUpdateTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		Update();
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 Update (private)

	Returns kJTrue if the settings were read in.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::Update()
{
	time_t t;
	if (!itsSignalFileName.IsEmpty() &&
		JGetModificationTime(itsSignalFileName, &t) == kJNoError &&
		t != itsSignalModTime)
		{
		JBoolean isNew;
		return GetAll(&isNew);
		}
	else
		{
		// This exists because JCore values cannot update themsevles,
		// and it seems easier to toss them all in here.

		if ((itsOrigWMStrategy         != JXWindow::GetWMPlacementStrategy() ||
			 itsOrigCopyWhenSelectFlag != JTextEditor::WillCopyWhenSelect()  ||
			 itsOrigPWMod              != JXTEBase::GetPartialWordModifier() ||
			 itsOrigCaretScrollFlag    != JXTEBase::CaretWillFollowScroll()  ||
			 itsOrigWindowsHomeEndFlag != JXTEBase::WillUseWindowsHomeEnd()  ||
			 itsOrigAllowSpaceFlag     != JXSaveFileInput::WillAllowSpace()  ||
			 itsOrigMenuDisplayStyle   != JXMenu::GetDisplayStyle()) &&
			Open())
			{
			PrivateSetWMPlacementStrategy();
			PrivateSetCopyWhenSelectFlag();
			PrivateSetPartialWordModifier();
			PrivateSetCaretFollowsScroll();
			PrivateSetWindowsHomeEnd();
			PrivateSetAllowSpaceFlag();
			PrivateSetMenuDisplayStyle();
			Close(kJTrue);
			}

		return kJFalse;
		}
}

/******************************************************************************
 GetAll (private)

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::GetAll
	(
	JBoolean* isNew
	)
{
	if (Open())
		{
		*isNew = itsFile->IsEmpty();

		JBoolean changed = kJFalse;
		std::string data;

		if (itsFile->IDValid(kWMPlacementStrategyID))
			{
			itsFile->GetData(kWMPlacementStrategyID, &data);
			std::istringstream input(data);
			input >> itsOrigWMStrategy;
			JXWindow::SetWMPlacementStrategy(itsOrigWMStrategy);
			}
		else
			{
			PrivateSetWMPlacementStrategy();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kCopyWhenSelectID))
			{
			itsFile->GetData(kCopyWhenSelectID, &data);
			std::istringstream input(data);
			input >> itsOrigCopyWhenSelectFlag;
			JTextEditor::ShouldCopyWhenSelect(itsOrigCopyWhenSelectFlag);
			}
		else
			{
			PrivateSetCopyWhenSelectFlag();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kPWModifierID))
			{
			itsFile->GetData(kPWModifierID, &data);
			std::istringstream input(data);
			input >> itsOrigPWMod;
			JXTEBase::SetPartialWordModifier(itsOrigPWMod);
			}
		else
			{
			PrivateSetPartialWordModifier();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kCaretFollowsScrollID))
			{
			itsFile->GetData(kCaretFollowsScrollID, &data);
			std::istringstream input(data);
			input >> itsOrigCaretScrollFlag;
			JXTEBase::CaretShouldFollowScroll(itsOrigCaretScrollFlag);
			}
		else
			{
			PrivateSetCaretFollowsScroll();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kWindowsHomeEndID))
			{
			itsFile->GetData(kWindowsHomeEndID, &data);
			std::istringstream input(data);
			input >> itsOrigWindowsHomeEndFlag;
			JXTEBase::ShouldUseWindowsHomeEnd(itsOrigWindowsHomeEndFlag);
			}
		else
			{
			if (!(*isNew))
				{
				JXTEBase::ShouldUseWindowsHomeEnd(kJFalse);
				}

			PrivateSetWindowsHomeEnd();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kAllowSpaceID))
			{
			itsFile->GetData(kAllowSpaceID, &data);
			std::istringstream input(data);
			input >> itsOrigAllowSpaceFlag;
			JXSaveFileInput::ShouldAllowSpace(itsOrigAllowSpaceFlag);
			}
		else
			{
			PrivateSetAllowSpaceFlag();
			changed = kJTrue;
			}

		if (itsFile->IDValid(kMenuDisplayStyleID))
			{
			itsFile->GetData(kMenuDisplayStyleID, &data);
			std::istringstream input(data);
			long temp;
			input >> temp;
			itsOrigMenuDisplayStyle = (JXMenu::Style) temp;
			if (itsOrigMenuDisplayStyle > JXMenu::kStyleMax)
				{
				itsOrigMenuDisplayStyle = JXMenu::kWindowsStyle;
				}
			JXMenu::SetDisplayStyle(itsOrigMenuDisplayStyle);
			}
		else
			{
			if (!(*isNew))
				{
				JXMenu::SetDisplayStyle(JXMenu::kMacintoshStyle);
				}

			PrivateSetMenuDisplayStyle();
			changed = kJTrue;
			}

		if (PrivateGetHelpManagerPrefs())
			{
			changed = kJTrue;
			}

		if (PrivateGetWebBrowserPrefs())
			{
			changed = kJTrue;
			}

		Close(changed);
		SaveSignalModTime();
		return kJTrue;
		}
	else
		{
		*isNew = kJTrue;
		return kJFalse;
		}
}

/******************************************************************************
 PrivateSetWMPlacementStrategy (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetWMPlacementStrategy()
{
	assert( itsFile != NULL );

	itsOrigWMStrategy = JXWindow::GetWMPlacementStrategy();

	std::ostringstream data;
	data << itsOrigWMStrategy;
	itsFile->SetData(kWMPlacementStrategyID, data);
}

/******************************************************************************
 PrivateSetCopyWhenSelectFlag (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetCopyWhenSelectFlag()
{
	assert( itsFile != NULL );

	itsOrigCopyWhenSelectFlag = JTextEditor::WillCopyWhenSelect();

	std::ostringstream data;
	data << itsOrigCopyWhenSelectFlag;
	itsFile->SetData(kCopyWhenSelectID, data);
}

/******************************************************************************
 PrivateSetPartialWordModifier (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetPartialWordModifier()
{
	assert( itsFile != NULL );

	itsOrigPWMod = JXTEBase::GetPartialWordModifier();

	std::ostringstream data;
	data << itsOrigPWMod;
	itsFile->SetData(kPWModifierID, data);
}

/******************************************************************************
 PrivateSetCaretFollowsScroll (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetCaretFollowsScroll()
{
	assert( itsFile != NULL );

	itsOrigCaretScrollFlag = JXTEBase::CaretWillFollowScroll();

	std::ostringstream data;
	data << itsOrigCaretScrollFlag;
	itsFile->SetData(kCaretFollowsScrollID, data);
}

/******************************************************************************
 PrivateSetWindowsHomeEnd (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetWindowsHomeEnd()
{
	assert( itsFile != NULL );

	itsOrigWindowsHomeEndFlag = JXTEBase::WillUseWindowsHomeEnd();

	std::ostringstream data;
	data << itsOrigWindowsHomeEndFlag;
	itsFile->SetData(kWindowsHomeEndID, data);
}

/******************************************************************************
 PrivateSetAllowSpaceFlag (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetAllowSpaceFlag()
{
	assert( itsFile != NULL );

	itsOrigAllowSpaceFlag = JXSaveFileInput::WillAllowSpace();

	std::ostringstream data;
	data << itsOrigAllowSpaceFlag;
	itsFile->SetData(kAllowSpaceID, data);
}

/******************************************************************************
 PrivateSetMenuDisplayStyle (private)

 ******************************************************************************/

void
JXSharedPrefsManager::PrivateSetMenuDisplayStyle()
{
	assert( itsFile != NULL );

	itsOrigMenuDisplayStyle = JXMenu::GetDisplayStyle();

	std::ostringstream data;
	data << (long) itsOrigMenuDisplayStyle;
	itsFile->SetData(kMenuDisplayStyleID, data);
}

/******************************************************************************
 GetHelpManagerPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::GetHelpManagerPrefs()
{
	if (itsFile != NULL)
		{
		if (PrivateGetHelpManagerPrefs())
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (!Update() && Open())
		{
		const JBoolean changed = PrivateGetHelpManagerPrefs();
		Close(changed);
		}
}

/******************************************************************************
 PrivateGetHelpManagerPrefs (private)

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateGetHelpManagerPrefs()
{
	assert( itsFile != NULL );

	JXHelpManager* helpMgr;
	if (!JXGetHelpManager(&helpMgr))
		{
		return kJFalse;
		}
	else if (itsFile->IDValid(kLatestHelpMgrVersionID))
		{
		std::string data;
		itsFile->GetData(kLatestHelpMgrVersionID, &data);
		std::istringstream versInput(data);
		JFileVersion vers;
		versInput >> vers;

		if (vers == 4)
			{
			itsFile->GetData(kHelpMgrV4, &data);
			}
		else
			{
			itsFile->GetData(kHelpMgrV3, &data);
			}
		std::istringstream input(data);
		helpMgr->ReadSetup(input);

		return kJFalse;
		}
	else
		{
		return PrivateSetHelpManagerPrefs();
		}
}

/******************************************************************************
 SetHelpManagerPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::SetHelpManagerPrefs()
{
	if (itsFile != NULL)
		{
		if (PrivateSetHelpManagerPrefs())
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (Open())
		{
		const JBoolean changed = PrivateSetHelpManagerPrefs();
		Close(changed);
		}
}

/******************************************************************************
 PrivateSetHelpManagerPrefs (private)

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateSetHelpManagerPrefs()
{
	assert( itsFile != NULL );

	JXHelpManager* helpMgr;
	if (!JXGetHelpManager(&helpMgr))
		{
		return kJFalse;
		}

	const JFileVersion currVers = helpMgr->GetCurrentSetupVersion();

	if (itsFile->IDValid(kLatestHelpMgrVersionID))
		{
		std::string data;
		itsFile->GetData(kLatestHelpMgrVersionID, &data);
		std::istringstream input(data);
		JFileVersion origVers;
		input >> origVers;
		if (origVers > currVers)
			{
			return kJFalse;
			}
		}

	std::ostringstream versData;
	versData << currVers;
	itsFile->SetData(kLatestHelpMgrVersionID, versData);

	std::ostringstream v3Data;
	helpMgr->WriteSetupV3(v3Data);
	itsFile->SetData(kHelpMgrV3, v3Data);

	std::ostringstream v4Data;
	helpMgr->WriteSetupV4(v4Data);
	itsFile->SetData(kHelpMgrV4, v4Data);

	return kJTrue;
}

/******************************************************************************
 GetWebBrowserPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::GetWebBrowserPrefs()
{
	if (itsFile != NULL)
		{
		if (PrivateGetWebBrowserPrefs())
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (!Update() && Open())
		{
		const JBoolean changed = PrivateGetWebBrowserPrefs();
		Close(changed);
		}
}

/******************************************************************************
 PrivateGetWebBrowserPrefs (private)

	Returns kJTrue if anything changed.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateGetWebBrowserPrefs()
{
	assert( itsFile != NULL );

	JXWebBrowser* browser;
	if (!JXGetWebBrowser(&browser))
		{
		return kJFalse;
		}
	else if (itsFile->IDValid(kLatestWebBrowserVersionID))
		{
		std::string data;
		itsFile->GetData(kLatestWebBrowserVersionID, &data);
		std::istringstream versInput(data);
		JFileVersion vers;
		versInput >> vers;

		if (vers == 1)
			{
			itsFile->GetData(kWebBrowserV1, &data);
			}
		else
			{
			itsFile->GetData(kWebBrowserV0, &data);
			}
		std::istringstream input(data);
		browser->ReadSetup(input);

		return kJFalse;
		}
	else
		{
		return PrivateSetWebBrowserPrefs();
		}
}

/******************************************************************************
 SetWebBrowserPrefs

 ******************************************************************************/

void
JXSharedPrefsManager::SetWebBrowserPrefs()
{
	if (itsFile != NULL)
		{
		if (PrivateSetWebBrowserPrefs())
			{
			itsChangedFlag = kJTrue;
			}
		}
	else if (Open())
		{
		const JBoolean changed = PrivateSetWebBrowserPrefs();
		Close(changed);
		}
}

/******************************************************************************
 PrivateSetWebBrowserPrefs (private)

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::PrivateSetWebBrowserPrefs()
{
	assert( itsFile != NULL );

	JXWebBrowser* browser;
	if (!JXGetWebBrowser(&browser))
		{
		return kJFalse;
		}

	const JFileVersion currVers = browser->GetCurrentSetupVersion();

	if (itsFile->IDValid(kLatestWebBrowserVersionID))
		{
		std::string data;
		itsFile->GetData(kLatestWebBrowserVersionID, &data);
		std::istringstream input(data);
		JFileVersion origVers;
		input >> origVers;
		if (origVers > currVers)
			{
			return kJFalse;
			}
		}

	std::ostringstream versData;
	versData << currVers;
	itsFile->SetData(kLatestWebBrowserVersionID, versData);

	std::ostringstream v0Data;
	browser->WriteSetupV0(v0Data);
	itsFile->SetData(kWebBrowserV0, v0Data);

	std::ostringstream v1Data;
	browser->WriteSetupV1(v1Data);
	itsFile->SetData(kWebBrowserV1, v0Data);

	return kJTrue;
}

/******************************************************************************
 Open (private)

	This code suffers from a race condition.  If it ever becomes a problem,
	JFileArray will need to use lockfile instead.

 ******************************************************************************/

JBoolean
JXSharedPrefsManager::Open()
{
	assert( itsFile == NULL );

	const JError err = JPrefsFile::Create(kDataFileRoot, &itsFile,
										  JFileArray::kDeleteIfWaitTimeout);
	itsChangedFlag = kJFalse;
	return err.OK();
}

/******************************************************************************
 Close (private)

 ******************************************************************************/

void
JXSharedPrefsManager::Close
	(
	const JBoolean changed
	)
{
	delete itsFile;
	itsFile = NULL;

	if (changed || itsChangedFlag)
		{
		NotifyChanged();
		}
}

/******************************************************************************
 NotifyChanged (private)

 ******************************************************************************/

void
JXSharedPrefsManager::NotifyChanged()
{
	if (!itsSignalFileName.IsEmpty())
		{
		ofstream temp(itsSignalFileName);
		temp.close();

		SaveSignalModTime();
		}
}

/******************************************************************************
 SaveSignalModTime (private)

 ******************************************************************************/

void
JXSharedPrefsManager::SaveSignalModTime()
{
	if (!itsSignalFileName.IsEmpty())
		{
		JGetModificationTime(itsSignalFileName, &itsSignalModTime);
		}
}
