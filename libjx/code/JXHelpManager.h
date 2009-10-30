/******************************************************************************
 JXHelpManager.h

	Interface for the JXHelpManager class

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHelpManager
#define _H_JXHelpManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDirector.h>

class JString;
class JXWindow;
class JXHelpDirector;
class JXPSPrinter;

class JXHelpManager : public JXDirector
{
public:

	JXHelpManager();

	virtual ~JXHelpManager();

	JBoolean	HasTOC() const;
	void		ShowTOC();
	void		SetTOCSectionName(const JCharacter* name);

	void	RegisterSection(const JCharacter* name);
	void	ShowSection(const JCharacter* name, JXHelpDirector* helpDir = NULL);
	void	ShowURL(const JCharacter* url, JXHelpDirector* helpDir = NULL);
	void	CloseAll();

	void	SearchAllSections();

	void	SaveWindowPrefs(JXWindow* window);

	JFileVersion	GetCurrentSetupVersion() const;
	void			ReadSetup(istream& input);
	void			WriteLatestSetup(ostream& output) const;
	void			WriteSetupV3(ostream& output) const;
	void			WriteSetupV4(ostream& output) const;

	static JBoolean	IsLocalURL(const JString& url);

protected:

	virtual void	DirectorClosed(JXDirector* theDirector);

private:

	struct SectionInfo
	{
		const JCharacter*	name;
		JXHelpDirector*		dir;

		SectionInfo()
			:
			name(NULL), dir(NULL)
		{ };

		SectionInfo(const JCharacter* n)
			:
			name(n), dir(NULL)
		{ };
	};

private:

	JArray<SectionInfo>*	itsSections;
	const JCharacter*		itsTOCSectionName;	// NULL if not registered
	JXHelpDirector*			itsComposeHelpDir;	// NULL if not visible

	JString*				itsDefWindowGeom;
	JXPSPrinter*			itsPrinter;

private:

	JXHelpDirector*	CreateHelpDirector(const JCharacter* text);

	static JOrderedSetT::CompareResult
		CompareSections(const SectionInfo& s1, const SectionInfo& s2);

	// not allowed

	JXHelpManager(const JXHelpManager& source);
	const JXHelpManager& operator=(const JXHelpManager& source);
};


/******************************************************************************
 Table of Contents

 ******************************************************************************/

inline JBoolean
JXHelpManager::HasTOC()
	const
{
	return JI2B(itsTOCSectionName != NULL);
}

inline void
JXHelpManager::SetTOCSectionName
	(
	const JCharacter* name
	)
{
	itsTOCSectionName = name;
}

#endif
