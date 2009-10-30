/******************************************************************************
 JXButtonStates.h

	Interface for JXButtonStates.cc

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXButtonStates
#define _H_JXButtonStates

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <X11/Xlib.h>
#include <jTypes.h>

const JSize kXButtonCount = 5;

enum JXMouseButton
{
	kJXNoButton = -1,		// internal use

	kJXButton1 = Button1,
	kJXButton2 = Button2,
	kJXButton3 = Button3,
	kJXButton4 = Button4,
	kJXButton5 = Button5,

	kJXLeftButton   = kJXButton1,
	kJXMiddleButton = kJXButton2,
	kJXRightButton  = kJXButton3
};

class JXButtonStates
{
public:

	JXButtonStates();
	JXButtonStates(const unsigned int state);

	JBoolean		GetState(const JIndex i) const;
	void			SetState(const JIndex i, const JBoolean pushed);
	JBoolean		AllOff() const;

	unsigned int	GetState() const;
	void			SetState(const unsigned int state);

	void			Clear();

	JBoolean
	button(const JIndex i)
		const
	{
		return GetState(i);
	};

	JBoolean
	left()
		const
	{
		return itsState[0];
	};

	JBoolean
	middle()
		const
	{
		return itsState[1];
	};

	JBoolean
	right()
		const
	{
		return itsState[2];
	};

	void
	ToggleState(const JIndex i)
	{
		SetState(i, JNegate(GetState(i)));
	};

	static JBoolean
	AllOff(const unsigned int state)
	{
		return JConvertToBoolean( (state & 0x1F00) == 0 );
	};

	static JBoolean		GetState(const unsigned int state, const JIndex i);
	static unsigned int	SetState(const unsigned int state,
								 const JIndex i, const JBoolean pushed);

	static unsigned int
	ToggleState(const unsigned int state, const JIndex i)
	{
		return SetState(state, i, JNegate(GetState(state, i)));
	};

private:

	JBoolean itsState[ kXButtonCount ];
};

#endif
