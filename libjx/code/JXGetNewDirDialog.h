/******************************************************************************
 JXGetNewDirDialog.h

	Interface for the JXGetNewDirDialog class

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXGetNewDirDialog
#define _H_JXGetNewDirDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JString;
class JXInputField;

class JXGetNewDirDialog : public JXDialogDirector
{
public:

	JXGetNewDirDialog(JXDirector* supervisor, const JCharacter* windowTitle,
					  const JCharacter* message, const JCharacter* initialName,
					  const JCharacter* basePath, const JBoolean modal = kJTrue);

	virtual ~JXGetNewDirDialog();

	JString	GetNewDirName() const;

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JString*	itsBasePath;

// begin JXLayout

    JXInputField* itsPathName;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* windowTitle,
						const JCharacter* message, const JCharacter* initialName);

	// not allowed

	JXGetNewDirDialog(const JXGetNewDirDialog& source);
	const JXGetNewDirDialog& operator=(const JXGetNewDirDialog& source);
};

#endif
