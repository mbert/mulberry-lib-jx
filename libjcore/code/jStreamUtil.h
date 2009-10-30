/******************************************************************************
 jStreamUtil.h

	Interface for jStreamUtil.cc

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jStreamUtil
#define _H_jStreamUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <ace/OS.h>

class JString;

void	JCopyBinaryData(istream& input, ostream& output, const JSize byteCount);

JString	JRead(istream& input, const JSize count);
JString	JReadUntil(istream& input, const JCharacter delimiter,
				   JBoolean* foundDelimiter = NULL);
JString	JReadUntilws(istream& input, JBoolean* foundws = NULL);
JString	JReadLine(istream& input, JBoolean* foundNewLine = NULL);
void	JReadAll(istream& input, JString* str);

JBoolean JReadUntil(istream& input, const JSize delimiterCount,
					const JCharacter* delimiters, JString* str,
					JCharacter* delimiter = NULL);

void JIgnoreUntil(istream& input, const JCharacter delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(istream& input, const JCharacter* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(istream& input, const JSize delimiterCount,
					  const JCharacter* delimiters, JCharacter* delimiter = NULL);

void JIgnoreLine(istream& input, JBoolean* foundNewLine = NULL);

// compensate for lack of features in io stream library

JString		JRead(int input, const JSize count);
JString		JReadUntil(int input, const JCharacter delimiter,
					   JBoolean* foundDelimiter = NULL);
JBoolean	JReadAll(int input, JString* str,
					 const JBoolean closeInput = kJTrue);

JBoolean JReadUntil(int input, const JSize delimiterCount,
					const JCharacter* delimiters, JString* str,
					JCharacter* delimiter = NULL);

void JIgnoreUntil(int input, const JCharacter delimiter,
				  JBoolean* foundDelimiter = NULL);
void JIgnoreUntil(int input, const JCharacter* delimiter,
				  JBoolean* foundDelimiter = NULL);

JBoolean JIgnoreUntil(int input, const JSize delimiterCount,
					  const JCharacter* delimiters, JCharacter* delimiter = NULL);

JBoolean	JConvertToStream(int input, ifstream* input2, JString* tempFullName,
							 const JBoolean closeInput = kJTrue);

// compensate for ANSI's removal of features

JSize JTellg(istream& stream);
void  JSeekg(istream& stream, streampos position);
void  JSeekg(istream& stream, streamoff offset, JIOStreamSeekDir direction);

JSize JTellp(ostream& stream);
void  JSeekp(ostream& stream, streampos position);
void  JSeekp(ostream& stream, streamoff offset, JIOStreamSeekDir direction);

// compensate for Sun CC brain damage

void	JSetState(ios& stream, const int flag);

#endif
