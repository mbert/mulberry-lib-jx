/******************************************************************************
 jVCSUtil.cpp

	Routines to help with Version Control Systems.

	Copyright © 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jVCSUtil.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

static const JCharacter* kCVSDirName         = "CVS";
static const JCharacter* kSubversionDirName  = ".svn";
static const JCharacter* kSCCSDirName        = "SCCS";

/******************************************************************************
 JIsVCSDirectory

	This actually checks for all known version control directories.

 ******************************************************************************/

JBoolean
JIsVCSDirectory
	(
	const JCharacter* name
	)
{
	return JI2B(strcmp(name, kCVSDirName)        == 0 ||
				strcmp(name, kSubversionDirName) == 0 ||
				strcmp(name, kSCCSDirName)       == 0);
}

/******************************************************************************
 JGetVCSType

	This actually checks for all known version control directories.

 ******************************************************************************/

JVCSType
JGetVCSType
	(
	const JCharacter* path
	)
{
	JString p = path, n;
	if (JFileExists(path))
		{
		JSplitPathAndName(path, &p, &n);
		}

	JString vcsDir = JCombinePathAndName(p, kCVSDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJCVSType;
		}

	vcsDir = JCombinePathAndName(p, kSubversionDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJSVNType;
		}

	vcsDir = JCombinePathAndName(p, kSCCSDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJSCCSType;
		}

	return kJUnknownVCSType;
}

/******************************************************************************
 JEditVCS

 ******************************************************************************/

void
JEditVCS
	(
	const JCharacter* fullName
	)
{
	if (JFileExists(fullName) && !JFileWritable(fullName))
		{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JString cvsDir = JCombinePathAndName(path, kCVSDirName);
		if (JDirectoryExists(cvsDir))
			{
			const JString cmd = "cd " + path + "; cvs edit " + name;
			system(cmd);
			}
		}
}

/******************************************************************************
 JRemoveVCS

 ******************************************************************************/

void
JRemoveVCS
	(
	const JCharacter* fullName
	)
{
	if (JFileExists(fullName))
		{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JString cvsDir = JCombinePathAndName(path, kCVSDirName);
		if (JDirectoryExists(cvsDir))
			{
			const JString cmd = "cd " + path + "; cvs remove -f " + name;
			system(cmd);
			}
		}
}

/******************************************************************************
 JUpdateCVSIgnore

 ******************************************************************************/

void
JUpdateCVSIgnore
	(
	const JCharacter* ignoreFullName
	)
{
	JString path, name;
	JSplitPathAndName(ignoreFullName, &path, &name);
	const JString cvsFile = JCombinePathAndName(path, ".cvsignore");

	JString cvsData;
	JReadFile(cvsFile, &cvsData);

	name += "\n";
	if (!cvsData.Contains(name))
		{
		JEditVCS(cvsFile);

		// append new data

		if (!cvsData.IsEmpty() && !cvsData.EndsWith("\n"))
			{
			cvsData += "\n";
			}
		cvsData += name;

		// write result

		ofstream output(cvsFile);
		cvsData.Print(output);
		}
}
