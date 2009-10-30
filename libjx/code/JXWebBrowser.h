/******************************************************************************
 JXWebBrowser.h

	Copyright � 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWebBrowser
#define _H_JXWebBrowser

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirector.h>
#include <JString.h>

class JXEditWWWPrefsDialog;

class JXWebBrowser : virtual public JBroadcaster
{
public:

	JXWebBrowser();

	virtual ~JXWebBrowser();

	void	ShowURL(const JCharacter* url);
	void	ShowFileContent(const JCharacter* fileName);
	void	ShowFileLocation(const JCharacter* fileName);
	void	ComposeMail(const JCharacter* address);

	void	EditPrefs();

	JFileVersion	GetCurrentSetupVersion() const;
	void			ReadSetup(istream& input);
	void			WriteLatestSetup(ostream& output) const;
	void			WriteSetupV0(ostream& output) const;
	void			WriteSetupV1(ostream& output) const;

	const JString&	GetShowURLCmd() const;
	void			SetShowURLCmd(const JCharacter* cmd);

	const JString&	GetShowFileContentCmd() const;
	void			SetShowFileContentCmd(const JCharacter* cmd);

	const JString&	GetShowFileLocationCmd() const;
	void			SetShowFileLocationCmd(const JCharacter* cmd);

	const JString&	GetComposeMailCmd() const;
	void			SetComposeMailCmd(const JCharacter* cmd);

	// upgrading from previous versions -- public because used by other classes

	static void	ConvertVarNames(JString* s, const JCharacter* varNameList);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JString	itsShowURLCmd;
	JString	itsShowFileContentCmd;
	JString	itsShowFileLocationCmd;
	JString	itsComposeMailCmd;

	JXEditWWWPrefsDialog*	itsPrefsDialog;

private:

	void	Exec(const JCharacter* cmd,
				 const JCharacter* varName, const JCharacter* value) const;

	// not allowed

	JXWebBrowser(const JXWebBrowser& source);
	const JXWebBrowser& operator=(const JXWebBrowser& source);
};


/******************************************************************************
 Outsourced commands

 ******************************************************************************/

inline const JString&
JXWebBrowser::GetShowURLCmd()
	const
{
	return itsShowURLCmd;
}

inline const JString&
JXWebBrowser::GetShowFileContentCmd()
	const
{
	return itsShowFileContentCmd;
}

inline const JString&
JXWebBrowser::GetShowFileLocationCmd()
	const
{
	return itsShowFileLocationCmd;
}

inline const JString&
JXWebBrowser::GetComposeMailCmd()
	const
{
	return itsComposeMailCmd;
}

#endif
