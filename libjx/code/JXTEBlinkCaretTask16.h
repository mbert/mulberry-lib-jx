/******************************************************************************
 JXTEBlinkCaretTask16.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTEBlinkCaretTask16
#define _H_JXTEBlinkCaretTask16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class JXTEBase16;

class JXTEBlinkCaretTask16 : public JXIdleTask
{
public:

	JXTEBlinkCaretTask16(JXTEBase16* te);

	virtual ~JXTEBlinkCaretTask16();

	void			ShouldBlink(const JBoolean blink);
	void			Reset();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXTEBase16*	itsTE;			// owns us
	JBoolean	itsActiveFlag;

private:

	// not allowed

	JXTEBlinkCaretTask16(const JXTEBlinkCaretTask16& source);
	const JXTEBlinkCaretTask16& operator=(const JXTEBlinkCaretTask16& source);
};


/******************************************************************************
 ShouldBlink

 ******************************************************************************/

inline void
JXTEBlinkCaretTask16::ShouldBlink
	(
	const JBoolean blink
	)
{
	itsActiveFlag = blink;
	Reset();
}

#endif
