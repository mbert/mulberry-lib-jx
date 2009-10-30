/******************************************************************************
 JUserNotification.cpp

	Abstract base class for notifying the user of something.

	IsSilent() indicates whether or not DisplayMessage and ReportError
	should actually display the message.

	Copyright � 1994-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JUserNotification.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JUserNotification::JUserNotification()
{
	itsSilenceFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUserNotification::~JUserNotification()
{
}
