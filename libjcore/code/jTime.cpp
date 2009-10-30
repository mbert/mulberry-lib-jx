/******************************************************************************
 jTime.cpp

	Routines to augment time.c

	Copyright © 1995 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jTime.h>
#include <jMath.h>
#include <unistd.h>
#include <jMissingProto.h>
#include <jGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kWarnExpireID = "WarnExpire::jTime";
static const JCharacter* kExpiredID    = "Expired::jTime";

/******************************************************************************
 JGetTimeStamp

	Returns the time stamp from ctime (minus the line feed) as a JString.

 ******************************************************************************/

JString
JGetTimeStamp()
{
	return JConvertToTimeStamp(time(NULL));
}

/******************************************************************************
 JConvertToTimeStamp

	Returns the time stamp from ctime (minus the line feed) as a JString.

 ******************************************************************************/

JString
JConvertToTimeStamp
	(
	const time_t t
	)
{
	JString timeStamp  = ctime(&t);
	const JSize length = timeStamp.GetLength();
	timeStamp.RemoveSubstring(length, length);
	return timeStamp;
}

/******************************************************************************
 JCheckExpirationDate

	Warns user if time is within two weeks of expiration.  Calls exit()
	if time is beyond expiration.

	map must define:  date, name, version, url, email
	*** date must be first!

 ******************************************************************************/

void
JCheckExpirationDate
	(
	const time_t		expireTime,
	const JCharacter*	map[],
	const JSize			size
	)
{
	const time_t t = time(NULL);
	if (t > expireTime)
		{
		map[1] = "";
		const JString msg = JGetString(kExpiredID, map, size);
		(JGetUserNotification())->DisplayMessage(msg);
		exit(0);
		}
	else if (t > expireTime - 14*24*3600)
		{
		JCharacter date[100];
		strftime(date, 100, "%B %e, %Y", localtime(&expireTime));
		map[1] = date;
		const JString msg = JGetString(kWarnExpireID, map, size);
		(JGetUserNotification())->DisplayMessage(msg);
		}
}
