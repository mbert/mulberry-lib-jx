/******************************************************************************
 JXTEBlinkCaretTask16.cpp

	BASE CLASS = JXIdleTask

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTEBlinkCaretTask16.h>
#include <JXTEBase16.h>
#include <JXWindow.h>
#include <jAssert.h>

const Time kHalfPeriod = 600;	// 0.6 seconds (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTEBlinkCaretTask16::JXTEBlinkCaretTask16
	(
	JXTEBase16* te
	)
	:
	JXIdleTask(kHalfPeriod)
{
	itsTE         = te;
	itsActiveFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEBlinkCaretTask16::~JXTEBlinkCaretTask16()
{
}

/******************************************************************************
 Reset

 ******************************************************************************/

void
JXTEBlinkCaretTask16::Reset()
{
	ResetTimer();
	if (itsTE->IsActive() && itsTE->HasFocus() &&
		(itsTE->GetWindow())->HasFocus())
		{
		itsTE->TEShowCaret();	// hide at first timer rollover
		}
	else
		{
		itsTE->TEHideCaret();
		}
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXTEBlinkCaretTask16::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (itsActiveFlag && TimeToPerform(delta, maxSleepTime))
		{		
		if (itsTE->TECaretIsVisible())
			{
			itsTE->TEHideCaret();
			}
		else
			{
			itsTE->TEShowCaret();
			}
		}
}
