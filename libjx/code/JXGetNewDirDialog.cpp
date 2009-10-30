/******************************************************************************
 JXGetNewDirDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXGetNewDirDialog.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetNewDirDialog::JXGetNewDirDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	windowTitle,
	const JCharacter*	message,
	const JCharacter*	initialName,
	const JCharacter*	basePath,
	const JBoolean		modal
	)
	:
	JXDialogDirector(supervisor, modal)
{
	assert( JDirectoryExists(basePath) );

	itsBasePath = new JString(basePath);
	assert( itsBasePath != NULL );

	BuildWindow(windowTitle, message, initialName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetNewDirDialog::~JXGetNewDirDialog()
{
	delete itsBasePath;
}

/******************************************************************************
 GetNewDirName

 ******************************************************************************/

JString
JXGetNewDirDialog::GetNewDirName()
	const
{
	JString dirName = itsPathName->GetText();
	if (JIsRelativePath(dirName))
		{
		dirName = JCombinePathAndName(*itsBasePath, dirName);
		}
	return dirName;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXGetNewDirDialog::BuildWindow
	(
	const JCharacter* windowTitle,
	const JCharacter* message,
	const JCharacter* initialName
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 270,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXGetNewDirDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 149,79, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXGetNewDirDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXGetNewDirDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 60,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::JXGetNewDirDialog::shortcuts::JXLayout"));

    itsPathName =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 230,20);
    assert( itsPathName != NULL );

    JXStaticText* messageObj =
        new JXStaticText(JGetString("messageObj::JXGetNewDirDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 230,20);
    assert( messageObj != NULL );

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	messageObj->SetText(message);

	itsPathName->SetText(initialName);
	itsPathName->SetIsRequired();
	itsPathName->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXGetNewDirDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString pathName = GetNewDirName();
	if (JDirectoryExists(pathName))
		{
		(JGetUserNotification())->ReportError("This directory already exists.");
		return kJFalse;
		}
	else if (JNameUsed(pathName))
		{
		(JGetUserNotification())->ReportError("This name is in use.");
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
