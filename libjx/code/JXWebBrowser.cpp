/******************************************************************************
 JXWebBrowser.cpp

	This class is designed to be used as a global object.  All requests to
	display URL's and files should be passed to this object.  It contacts
	the appropriate program (e.g. Netscape) to display the data.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWebBrowser.h>
#include <JXEditWWWPrefsDialog.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXSharedPrefsManager.h>
#include <jXGlobals.h>
#include <JSimpleProcess.h>
#include <jAssert.h>

// If you change the variable names, update JXHelpManager::WriteSetupV3()
// to convert them back to u,f,a.
//
// "netscape -noraise -remote \"openBrowser($u)\""
// "arrow --mailto \"$a\""

static const JCharacter* kDefShowURLCmd = "firefox $u";
static const JCharacter* kURLVarName    = "u";

static const JCharacter* kFileURLPrefix          = "file:";
const JSize kFileURLPrefixLength                 = strlen(kFileURLPrefix);
static const JCharacter* kDefShowFileContentCmd  = "firefox file:'$f'";
static const JCharacter* kDefShowFileLocationCmd = "systemg '$f'";
static const JCharacter* kFileVarName            = "f";

static const JCharacter* kMailURLPrefix      = "mailto:";
const JSize kMailURLPrefixLength             = strlen(kMailURLPrefix);
static const JCharacter* kDefComposeMailCmd  = "firefox mailto:$a";
static const JCharacter* kMailAddressVarName = "a";

// setup information

const JFileVersion kCurrentSetupVersion = 1;

// version  1 includes itsShowFileLocationCmd

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWebBrowser::JXWebBrowser()
	:
	itsShowURLCmd(kDefShowURLCmd),
	itsShowFileContentCmd(kDefShowFileContentCmd),
	itsShowFileLocationCmd(kDefShowFileLocationCmd),
	itsComposeMailCmd(kDefComposeMailCmd),
	itsPrefsDialog(NULL)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWebBrowser::~JXWebBrowser()
{
}

/******************************************************************************
 ShowURL

 ******************************************************************************/

void
JXWebBrowser::ShowURL
	(
	const JCharacter* url
	)
{
	JString s = url;
	if (s.BeginsWith(kMailURLPrefix, kJFalse))
		{
		s.RemoveSubstring(1, kMailURLPrefixLength);
		ComposeMail(s);
		}
	else if (s.BeginsWith(kFileURLPrefix, kJFalse))
		{
		s.RemoveSubstring(1, kFileURLPrefixLength);
		ShowFileContent(s);
		}
	else
		{
		Exec(itsShowURLCmd, kURLVarName, url);
		}
}

/******************************************************************************
 ShowFileContent

 ******************************************************************************/

void
JXWebBrowser::ShowFileContent
	(
	const JCharacter* fileName
	)
{
	Exec(itsShowFileContentCmd, kFileVarName, fileName);
}

/******************************************************************************
 ShowFileLocation

 ******************************************************************************/

void
JXWebBrowser::ShowFileLocation
	(
	const JCharacter* fileName
	)
{
	Exec(itsShowFileLocationCmd, kFileVarName, fileName);
}

/******************************************************************************
 ComposeMail

 ******************************************************************************/

void
JXWebBrowser::ComposeMail
	(
	const JCharacter* address
	)
{
	Exec(itsComposeMailCmd, kMailAddressVarName, address);
}

/******************************************************************************
 Exec (private)

 ******************************************************************************/

void
JXWebBrowser::Exec
	(
	const JCharacter* cmd,
	const JCharacter* varName,
	const JCharacter* value
	)
	const
{
	if (!JStringEmpty(cmd))
		{
		const JCharacter* map[] =
			{
			varName, value
			};

		JString s = cmd;
		(JGetStringManager())->Replace(&s, map, sizeof(map));
		JSimpleProcess::Create(s, kJTrue);
		}
}

/******************************************************************************
 Outsourced commands

 ******************************************************************************/

void
JXWebBrowser::SetShowURLCmd
	(
	const JCharacter* cmd
	)
{
	itsShowURLCmd = cmd;
	(JXGetSharedPrefsManager())->SetWebBrowserPrefs();
}

void
JXWebBrowser::SetShowFileContentCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileContentCmd = cmd;
	(JXGetSharedPrefsManager())->SetWebBrowserPrefs();
}

void
JXWebBrowser::SetShowFileLocationCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileLocationCmd = cmd;
	(JXGetSharedPrefsManager())->SetWebBrowserPrefs();
}

void
JXWebBrowser::SetComposeMailCmd
	(
	const JCharacter* cmd
	)
{
	itsComposeMailCmd = cmd;
	(JXGetSharedPrefsManager())->SetWebBrowserPrefs();
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
JXWebBrowser::EditPrefs()
{
	assert( itsPrefsDialog == NULL );

	itsPrefsDialog = new JXEditWWWPrefsDialog(JXGetApplication(), itsShowURLCmd,
											  itsShowFileContentCmd,
											  itsShowFileLocationCmd,
											  itsComposeMailCmd);
	assert( itsPrefsDialog != NULL );
	itsPrefsDialog->BeginDialog();
	ListenTo(itsPrefsDialog);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXWebBrowser::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsPrefsDialog->GetPrefs(&itsShowURLCmd, &itsShowFileContentCmd,
									 &itsShowFileLocationCmd, &itsComposeMailCmd);
			(JXGetSharedPrefsManager())->SetWebBrowserPrefs();
			}
		itsPrefsDialog = NULL;
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 GetCurrentSetupVersion

 ******************************************************************************/

JFileVersion
JXWebBrowser::GetCurrentSetupVersion()
	const
{
	return kCurrentSetupVersion;
}

/******************************************************************************
 ReadSetup

	We assert that we can read the given data because there is no
	way to skip over it.

 ******************************************************************************/

void
JXWebBrowser::ReadSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );

	input >> itsShowURLCmd >> itsShowFileContentCmd >> itsComposeMailCmd;

	if (vers >= 1)
		{
		input >> itsShowFileLocationCmd;
		}
}

/******************************************************************************
 WriteLatestSetup

 ******************************************************************************/

void
JXWebBrowser::WriteLatestSetup
	(
	ostream& output
	)
	const
{
	assert( kCurrentSetupVersion == 1 );
	WriteSetupV1(output);
}

/******************************************************************************
 WriteSetupV0

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JXWebBrowser::WriteSetupV0
	(
	ostream& output
	)
	const
{
	output << ' ' << 0;		// version
	output << ' ' << itsShowURLCmd;
	output << ' ' << itsShowFileContentCmd;
	output << ' ' << itsComposeMailCmd;
	output << ' ';
}

/******************************************************************************
 WriteSetupV1

	This is for use by JXGlobalPrefsManager.  Nobody else should store
	our settings.

 ******************************************************************************/

void
JXWebBrowser::WriteSetupV1
	(
	ostream& output
	)
	const
{
	output << ' ' << 1;		// version
	output << ' ' << itsShowURLCmd;
	output << ' ' << itsShowFileContentCmd;
	output << ' ' << itsComposeMailCmd;
	output << ' ' << itsShowFileLocationCmd;
	output << ' ';
}

/******************************************************************************
 ConvertVarNames (static)

	Convert % to $ when followed by any character in varNameList.
	Backslashes and dollars are also backslashed, as required by JSubstitute.

 ******************************************************************************/

void
JXWebBrowser::ConvertVarNames
	(
	JString*			s,
	const JCharacter*	varNameList
	)
{
	// escape existing backslashes

	JIndex i = 1;
	while (s->LocateNextSubstring("\\", &i))
		{
		s->InsertSubstring("\\", i);
		i += 2;		// move past both backslashes
		}

	// escape existing dollars

	i = 1;
	while (s->LocateNextSubstring("$", &i))
		{
		s->InsertSubstring("\\", i);
		i += 2;		// move past $
		}

	// convert % to $ if followed by a variable name

	i = 1;
	while (s->LocateNextSubstring("%", &i) && i < s->GetLength())
		{
		const JCharacter c = s->GetCharacter(i+1);
		if (strchr(varNameList, c) != NULL)
			{
			s->SetCharacter(i, '$');
			}
		i += 2;		// move past variable name
		}
}
