/******************************************************************************
 JXMDIServer.cpp

	Implements CanAcceptMDIRequest() for use with JXApplication.

	BASE CLASS = JMDIServer

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXMDIServer.h>
#include <jXGlobals.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMDIServer::JXMDIServer()
	:
	JMDIServer((JXGetApplication())->GetSignature())
{
	JXSetMDIServer(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXMDIServer::~JXMDIServer()
{
}

/******************************************************************************
 CanAcceptMDIRequest (virtual protected)

 ******************************************************************************/

JBoolean
JXMDIServer::CanAcceptMDIRequest()
{
	JXApplication* app = JXGetApplication();
	return JNegate( app->HasBlockingWindow() || app->IsSuspended() );
}
