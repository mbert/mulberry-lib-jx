/******************************************************************************
 JXNewDisplayDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXNewDisplayDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXNewDisplayDialog::JXNewDisplayDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXNewDisplayDialog::~JXNewDisplayDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXNewDisplayDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 310,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXNewDisplayDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 199,79, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXNewDisplayDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXNewDisplayDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,80, 60,20);
    assert( cancelButton != NULL );

    itsDisplayName =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 270,20);
    assert( itsDisplayName != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXNewDisplayDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
    assert( obj1_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Open new display");
	SetButtons(okButton, cancelButton);

	itsDisplayName->SetIsRequired();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXNewDisplayDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString displayName = itsDisplayName->GetText();
	JIndex displayIndex;
	if ((JXGetApplication())->OpenDisplay(displayName, &displayIndex))
		{
		itsDisplayIndex = displayIndex;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
