/******************************************************************************
 JXNewDisplayDialog.h

	Interface for the JXNewDisplayDialog class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXNewDisplayDialog
#define _H_JXNewDisplayDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXInputField;

class JXNewDisplayDialog : public JXDialogDirector
{
public:

	JXNewDisplayDialog(JXWindowDirector* supervisor);

	virtual ~JXNewDisplayDialog();

	JIndex	GetDisplayIndex() const;

protected:

	virtual JBoolean	OKToDeactivate();

private:

	JIndex	itsDisplayIndex;

// begin JXLayout

    JXInputField* itsDisplayName;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	JXNewDisplayDialog(const JXNewDisplayDialog& source);
	const JXNewDisplayDialog& operator=(const JXNewDisplayDialog& source);
};


/******************************************************************************
 GetDisplayIndex

 ******************************************************************************/

inline JIndex
JXNewDisplayDialog::GetDisplayIndex()
	const
{
	return itsDisplayIndex;
}

#endif
