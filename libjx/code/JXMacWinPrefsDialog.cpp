/******************************************************************************
 JXMacWinPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright � 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "JXMacWinPrefsDialog.h"
#include <JXWindow.h>
#include <JXMenu.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 EditPrefs (static)

 ******************************************************************************/

void
JXMacWinPrefsDialog::EditPrefs()
{
	JXMacWinPrefsDialog* dlog = new JXMacWinPrefsDialog;
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXMacWinPrefsDialog::JXMacWinPrefsDialog()
	:
	JXDialogDirector(JXGetApplication(), kJTrue)
{
	BuildWindow();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMacWinPrefsDialog::~JXMacWinPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXMacWinPrefsDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 350,200, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 209,169, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXMacWinPrefsDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,170, 60,20);
    assert( cancelButton != NULL );

    itsMacStyleCB =
        new JXTextCheckbox(JGetString("itsMacStyleCB::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 320,20);
    assert( itsMacStyleCB != NULL );

    itsHomeEndCB =
        new JXTextCheckbox(JGetString("itsHomeEndCB::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 320,20);
    assert( itsHomeEndCB != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 40,40, 300,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetFontSize(10);

    itsScrollCaretCB =
        new JXTextCheckbox(JGetString("itsScrollCaretCB::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 320,20);
    assert( itsScrollCaretCB != NULL );

    itsCopyWhenSelectCB =
        new JXTextCheckbox(JGetString("itsCopyWhenSelectCB::JXMacWinPrefsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 320,20);
    assert( itsCopyWhenSelectCB != NULL );

// end JXLayout

	window->SetTitle("Macintosh / Windows / X Preferences");
	SetButtons(okButton, cancelButton);

	itsMacStyleCB->SetState(JI2B(JXMenu::GetDisplayStyle() == JXMenu::kMacintoshStyle));
	itsHomeEndCB->SetState(JXTEBase::WillUseWindowsHomeEnd());
	itsScrollCaretCB->SetState(JXTEBase::CaretWillFollowScroll());
	itsCopyWhenSelectCB->SetState(JTextEditor::WillCopyWhenSelect());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXMacWinPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			UpdateSettings();
			}
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateSettings (private)

 ******************************************************************************/

void
JXMacWinPrefsDialog::UpdateSettings()
{
	JXMenu::SetDisplayStyle(itsMacStyleCB->IsChecked() ?
							JXMenu::kMacintoshStyle : JXMenu::kWindowsStyle);

	JXTEBase::ShouldUseWindowsHomeEnd(itsHomeEndCB->IsChecked());
	JXTEBase::CaretShouldFollowScroll(itsScrollCaretCB->IsChecked());
	JTextEditor::ShouldCopyWhenSelect(itsCopyWhenSelectCB->IsChecked());
}
