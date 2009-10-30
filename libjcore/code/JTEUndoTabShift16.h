/******************************************************************************
 JTEUndoTabShift16.h

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEUndoTabShift16
#define _H_JTEUndoTabShift16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTEUndoPaste16.h>

class JTEUndoTabShift16 : public JTEUndoPaste16
{
public:

	JTEUndoTabShift16(JTextEditor16* te);

	virtual ~JTEUndoTabShift16();

	void	UpdateEndChar();

	// provides safe downcasting

	virtual JTEUndoTabShift16*			CastToJTEUndoTabShift16();
	virtual const JTEUndoTabShift16*	CastToJTEUndoTabShift16() const;

private:

	// not allowed

	JTEUndoTabShift16(const JTEUndoTabShift16& source);
	const JTEUndoTabShift16& operator=(const JTEUndoTabShift16& source);
};

#endif
