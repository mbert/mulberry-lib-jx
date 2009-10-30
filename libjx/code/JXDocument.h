/******************************************************************************
 JXDocument.h

	Interface for the JXDocument class

	Copyright � 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDocument
#define _H_JXDocument

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JXDocumentManager.h>	// need definition of SafetySaveReason

class JXFileDocument;

class JXDocument : public JXWindowDirector
{
public:

	JXDocument(JXDirector* supervisor);

	virtual ~JXDocument();

	virtual JBoolean	Close();
	virtual JBoolean	Deactivate();

	virtual JBoolean	GetMenuIcon(const JXImage** icon) const;
	virtual JBoolean	NeedsSave() const;
	virtual JBoolean	NeedDocument(JXDocument* doc) const;

	void	RevertToSaved();

	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason) = 0;

	// provides safe downcasting

	virtual JXFileDocument*			CastToJXFileDocument();
	virtual const JXFileDocument*	CastToJXFileDocument() const;

protected:

	virtual JBoolean	OKToClose()      = 0;
	virtual JBoolean	OKToRevert()     = 0;
	virtual JBoolean	CanRevert()      = 0;
	virtual void		DiscardChanges() = 0;
	virtual JBoolean	OKToDeactivate();

private:

	// not allowed

	JXDocument(const JXDocument& source);
	const JXDocument& operator=(const JXDocument& source);
};

#endif
