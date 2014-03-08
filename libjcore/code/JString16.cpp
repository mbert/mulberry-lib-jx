/******************************************************************************
 JString16.cpp

	This class was not designed to be a base class!  If you need to override it,
	be sure to make the destructor virtual.

	JString16s can contain NULL's, if they are constructed with the
	JString16(const JCharacter16* str, const JSize length) constructor.
	You must remember not to call Clib functions on the const JCharacter16* in
	this case.

	Note that operator== is case sensitive, as one would expect.  To avoid the
	UNIX method of sorting capitalized names separately in front of lowercase
	names, operator< and operator> are not case sensitive.  One should therefore
	not mix == with < and > when comparing strings.

	Since strstream doesn't provide the control we need when converting a number
	to a string, we use the NumConvert and StrUtil modules.  We include them at
	the end of the file so they are completely hidden and JString16 is self-contained.

	BASE CLASS = none

	Copyright � 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JString16.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <JMinMax.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include <strstream>
#include <iomanip>
#include <jErrno.h>
#include <jAssert.h>

const JSize kDefaultBlockSize = 10;

static const JCharacter16* kDiacriticalMap  = NULL;
static const JIndex* kDiacriticalMarkType = NULL;

//static JCharacter16* kToLowerMap = NULL;
//static JCharacter16* kToUpperMap = NULL;

// private routines

static void	double2str16(double doubleVal, int afterDec, int sigDigitCount,
					   int expMin, JCharacter16 *returnStr);

/******************************************************************************
 Constructor

 ******************************************************************************/

JString16::JString16()
	:
	itsBlockSize( kDefaultBlockSize )
{
	itsStringLength = 0;
	itsAllocLength  = itsBlockSize;

	itsString = new JCharacter16 [ itsAllocLength+1 ];
	assert( itsString != NULL );
	itsString [ 0 ] = '\0';
}

JString16::JString16
	(
	const JCharacter16* str
	)
	:
	itsBlockSize( kDefaultBlockSize )
{
	itsAllocLength = 0;
	itsString      = NULL;		// makes delete [] safe inside CopyToPrivateString
	CopyToPrivateString(str);
}

JString16::JString16
	(
	const JCharacter16*	str,
	const JSize			length
	)
	:
	itsBlockSize( kDefaultBlockSize )
{
	itsAllocLength = 0;
	itsString      = NULL;		// makes delete [] safe inside CopyToPrivateString
	JString16 temp;
	CopyToPrivateString(length > 0 ? str : temp.GetCString(), length);		// allow NULL,0
}

JString16::JString16
	(
	const JCharacter16*	str,
	const JIndexRange&	range
	)
	:
	itsBlockSize( kDefaultBlockSize )
{
	itsAllocLength = 0;
	itsString      = NULL;		// makes delete [] safe inside CopyToPrivateString
	CopyToPrivateString(str + range.first-1, range.GetLength());
}

JString16::JString16
	(
	const JFloat			number,
	const JInteger			precision,
	const ExponentDisplay	expDisplay,
	const JInteger			exponent,
	const JInteger			sigDigitCount
	)
	:
	itsBlockSize( kDefaultBlockSize )
{
	assert( precision >= -1 );

	itsAllocLength = 50;

	itsString = new JCharacter16 [ itsAllocLength+1 ];
	assert( itsString != NULL );
	double2str16(number, precision, sigDigitCount,
			   (expDisplay == kUseGivenExponent ? exponent : expDisplay),
			   itsString);

	itsStringLength = strlen16(itsString);
}

JString16::JString16
	(
	const JUInt		number,
	const Base		base,
	const JBoolean	pad
	)
	:
	itsBlockSize( kDefaultBlockSize )
{
	itsAllocLength = 0;
	itsString      = NULL;		// makes delete [] safe inside CopyToPrivateString

	if (number == 0)
		{
		  JString16 temp;
		  temp.FromASCII("0");
		CopyToPrivateString(temp);
		}
	else if (base == 2)
		{
		  JString16 temp;
		CopyToPrivateString(temp);

		JUInt v = number;
		do
			{
			if (v & 0x01)
				{
				PrependCharacter('1');
				}
			else
				{
				PrependCharacter('0');
				}
			v >>= 1;
			}
			while (v != 0);

		while (pad && itsStringLength % 8 > 0)
			{
			PrependCharacter('0');
			}
		}
	else
		{
		std::ostringstream s;
		s << std::setbase(base) << number;
		const std::string s1 = s.str();
		JString16 temp;
		temp.FromASCII(s1.c_str());
		CopyToPrivateString(temp);
		}

	if (base == 16)
		{
		if (pad && itsStringLength % 2 == 1)
			{
			PrependCharacter('0');
			}

		ToUpper();
		JString16 temp;
		temp.FromASCII("0x");
		Prepend(temp);
		}
}

/******************************************************************************
 Copy constructor

	Even though JString16(const JCharacter16*) does the same job, we need to declare
	this copy constructor to avoid having the compiler declare it incorrectly.

 ******************************************************************************/

JString16::JString16
	(
	const JString16& source
	)
	:
	itsBlockSize( source.itsBlockSize )
{
	itsAllocLength = 0;
	itsString      = NULL;		// makes delete [] safe inside CopyToPrivateString

	CopyToPrivateString(source.itsString, source.itsStringLength);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JString16::~JString16()
{
	delete [] itsString;
}

/******************************************************************************
 CopyToPrivateString (private)

	Copy the given string into our private string.

	*** This function assumes that str != itsString

 ******************************************************************************/

void
JString16::CopyToPrivateString
	(
	const JCharacter16*	str,
	const JSize			length
	)
{
	assert( str != itsString );

	if (itsAllocLength < length || itsAllocLength == 0)
		{
		itsAllocLength = length + itsBlockSize;

		// We allocate the new memory first.
		// If new fails, we still have the old string data.

		JCharacter16* newString = new JCharacter16 [ itsAllocLength + 1 ];
		assert( newString != NULL );

		// now it's safe to throw out the old data

		delete [] itsString;
		itsString = newString;
		}

	// copy the characters to the new string

	memcpy(itsString, str, length * sizeof(JCharacter16));
	itsString[ length ] = '\0';

	itsStringLength = length;
}

/******************************************************************************
 InsertSubstring

	Insert the given string into ourselves starting at the specified index.

 ******************************************************************************/

void
JString16::InsertSubstring
	(
	const JCharacter16*	stringToInsert,
	const JSize			insertLength,
	const JIndex		insertionIndex
	)
{
	assert( 0 < insertionIndex && insertionIndex <= itsStringLength + 1 );

	// if the string to insert is empty, we don't need to do anything

	if (insertLength == 0)
		{
		return;
		}

	// If we don't have space for the result, we have to reallocate.

	JCharacter16* insertionPtr = NULL;
	const JUnsignedOffset insertionOffset = insertionIndex - 1;

	if (itsAllocLength < itsStringLength + insertLength)
		{
		itsAllocLength = itsStringLength + insertLength + itsBlockSize;

		// allocate space for the combined string

		JCharacter16* newString = new JCharacter16 [ itsAllocLength + 1 ];
		assert( newString != NULL );

		insertionPtr = newString + insertionOffset;

		// copy our characters in front of the insertion point

		memcpy(newString, itsString, insertionOffset * sizeof(JCharacter16));

		// copy our characters after the insertion point, -including the termination-

		memcpy(insertionPtr + insertLength, itsString + insertionOffset,
			   (itsStringLength - insertionOffset + 1) * sizeof(JCharacter16));

		// throw out our original string and save the new one

		delete [] itsString;
		itsString = newString;
		}

	// Otherwise, just shift characters to make space for the result.

	else
		{
		insertionPtr = itsString + insertionOffset;

		// shift characters after the insertion point, -including the termination-

		memmove(insertionPtr + insertLength, insertionPtr,
				(itsStringLength - insertionOffset + 1) * sizeof(JCharacter16));
		}

	// copy the characters from the string to insert

	memcpy(insertionPtr, stringToInsert, insertLength * sizeof(JCharacter16));

	// update our string length

	itsStringLength += insertLength;
}

/******************************************************************************
 AllocateCString

	This allocates a new pointer, which the caller is responsible
	for deleting via "delete []".

 ******************************************************************************/

JCharacter16*
JString16::AllocateCString()
	const
{
	JCharacter16* str = new JCharacter16 [ itsStringLength + 1 ];
	assert( str != NULL );

	memcpy(str, itsString, (itsStringLength+1) * sizeof(JCharacter16));

	return str;
}

JString
JString16::ToASCII() const
{
  std::ostrstream sout;

	const JCharacter16* p = GetCString();
	while(*p)
	{
		wchar_t wp = *p++;
		if (wp < 0x100)
		{
			// Write 1 to buffer
			unsigned char c = wp;
			sout.put(c);
		}
		else
		{
			sout.put('?');
		}
	}

	sout << std::ends;

	const JCharacter* pout = sout.str();
	JString result(pout);
	sout.freeze(false);

	return result;
}

void
JString16::FromASCII(const JString& ascii)
{
	FromASCII(ascii.GetCString());
}

void
JString16::FromASCII(const JCharacter* ascii)
{
  std::ostrstream sout;

	const JCharacter* p = ascii;

	// Convert each character
	unsigned long charlen = 0;
	wchar_t wc = 0;
	while(*p)
	{
		JCharacter16 c = *p++;
#if __BYTE_ORDER == __BIG_ENDIAN
		sout.put((c & 0xFF00) >> 8);
		sout.put(c & 0x00FF);
#else
		sout.put(c & 0x00FF);
		sout.put((c & 0xFF00) >> 8);
#endif
	}

	sout << std::ends << std::ends;
	
	const JCharacter16* pout = (const JCharacter16*)sout.str();
	Set(pout);	
	sout.freeze(false);
}

JString
JString16::ToUTF8() const
{
  std::ostrstream sout;

	const JCharacter16* p = GetCString();
	while(*p)
	{
		wchar_t wp = *p++;
		if (wp < 0x80)
		{
			// Write 1 to buffer
			unsigned char c = wp;
			sout.put(c);
		}
		else if (wp < 0x800)
		{
			// Write 2 to buffer
			unsigned char c = 0xc0 | (wp >> 6);
			sout.put(c);

			c = 0x80 | (wp & 0x3f);
			sout.put(c);
		}
		else // if (wp < 0x10000)
		{
			// Write 3 to buffer
			unsigned char c = 0xe0 | (wp >> 12);
			sout.put(c);

			c = 0x80 | ((wp >> 6) & 0x3f);
			sout.put(c);

			c = 0x80 | (wp & 0x3f);
			sout.put(c);
		}
	}

	sout << std::ends;

	const JCharacter* pout = sout.str();
	JString result(pout);
	sout.freeze(false);

	return result;
}

void
JString16::FromUTF8(const JString& utf8)
{
	FromUTF8(utf8.GetCString());
}

void
JString16::FromUTF8(const JCharacter* utf8)
{
  std::ostrstream sout;

	const JCharacter* p = utf8;

	// Convert each character
	unsigned long charlen = 0;
	wchar_t wc = 0;
	while(*p)
	{
		unsigned char mask = 0x3f;
		if (charlen == 0)
		{
			// Determine length of utf8 encoded wchar_t
			if ((*p & 0xf0 ) == 0xe0)
			{
				charlen = 3;
				mask = 0x0f;
			}
			else if ((*p & 0xe0 ) == 0xc0)
			{
				charlen = 2;
				mask = 0x1f;
			}
			else
			{
				charlen = 1;
				mask = 0x7f;
			}

			// Reset char
			wc = 0;
		}

		// Convert the byte
		wc <<= 6;
		wc |= (*p & mask);
		
		// Bump ptr
		p++;
		
		// Reduce byte remaining count and write it out if done
		if (!--charlen)
		{
			if ((wc != 0xfffe) && !((wc >= 0xd800) && (wc < 0xe000)))
			{
#if __BYTE_ORDER == __BIG_ENDIAN
				sout.put((wc & 0xFF00) >> 8);
				sout.put(wc & 0x00FF);
#else
				sout.put(wc & 0x00FF);
				sout.put((wc & 0xFF00) >> 8);
#endif
			}
		}
	}

	sout << std::ends << std::ends;
	
	const JCharacter16* pout = (const JCharacter16*)sout.str();
	Set(pout);	
	sout.freeze(false);
}

/******************************************************************************
 GetCharacter

 ******************************************************************************/

JCharacter16
JString16::GetCharacter
	(
	const JIndex index
	)
	const
{
	assert( IndexValid(index) );

	return PrivateGetCharacter(index);
}

/******************************************************************************
 GetCharacterFromEnd

 ******************************************************************************/

JCharacter16
JString16::GetCharacterFromEnd
	(
	const JIndex index
	)
	const
{
	assert( IndexValid(index) );	// avoid wrap of unsigned value

	return GetCharacter(itsStringLength - index + 1);
}

/******************************************************************************
 SetCharacterFromEnd

 ******************************************************************************/

void
JString16::SetCharacterFromEnd
	(
	const JIndex		index,
	const JCharacter16	c
	)
{
	assert( IndexValid(index) );	// avoid wrap of unsigned value

	SetCharacter(itsStringLength - index + 1, c);
}

/******************************************************************************
 SetCharacter

 ******************************************************************************/

void
JString16::SetCharacter
	(
	const JIndex		index,
	const JCharacter16	c
	)
{
	assert( IndexValid(index) );

	itsString [ index - 1 ] = c;
}

/******************************************************************************
 Clear

 ******************************************************************************/

void
JString16::Clear()
{
	// there is nothing to do if we are already empty

	if (itsStringLength == 0)
		{
		return;
		}

	// If we are using too much memory, reallocate.

	if (itsAllocLength > itsBlockSize)
		{
		itsAllocLength = itsBlockSize;

		// throw out the old data

		delete [] itsString;

		// Having just released a block of memory at least as large as the
		// one we are requesting, the system must really be screwed if this
		// call to new doesn't work.

		itsString = new JCharacter16 [ itsAllocLength + 1 ];
		assert( itsString != NULL );
		}

	// clear the string

	itsString [ 0 ] = '\0';
	itsStringLength = 0;
}

/******************************************************************************
 TrimWhitespace

	Trim leading and trailing whitespace from ourselves.

 ******************************************************************************/

void
JString16::TrimWhitespace()
{
	// there is nothing to do if we are already empty

	if (itsStringLength == 0)
		{
		return;
		}

	// if there is no blank space to trim, we can stop now

	if (!isspace(PrivateGetCharacter(1)) &&
		!isspace(PrivateGetCharacter(itsStringLength)))
		{
		return;
		}

	// find last non-blank character

	JIndex lastCharIndex = itsStringLength;
	while (lastCharIndex > 0 && isspace(PrivateGetCharacter(lastCharIndex)))
		{
		lastCharIndex--;
		}

	// if we are only blank space, we can just clear ourselves

	if (lastCharIndex == 0)
		{
		Clear();
		return;
		}

	// find first non-blank character (it does exist since lastCharIndex > 0)

	JIndex firstCharIndex = 1;
	while (isspace(PrivateGetCharacter(firstCharIndex)))
		{
		firstCharIndex++;
		}

	// If we are using too much memory, reallocate.

	const JSize newLength = lastCharIndex - firstCharIndex + 1;

	if (itsAllocLength > newLength + itsBlockSize)
		{
		itsAllocLength = newLength + itsBlockSize;

		// allocate space for the new string + termination

		JCharacter16* newString = new JCharacter16[ itsAllocLength+1 ];
		assert( newString != NULL );

		// copy the non-blank characters to the new string

		memcpy(newString, GetCharacterPtr(firstCharIndex), newLength * sizeof(JCharacter16));

		// throw out our original string and save the new one

		delete [] itsString;
		itsString = newString;
		}

	// Otherwise, just shift the characters.

	else
		{
		memmove(itsString, GetCharacterPtr(firstCharIndex), newLength * sizeof(JCharacter16));
		}

	// terminate

	itsString [ newLength ] = '\0';

	itsStringLength = newLength;
}

/******************************************************************************
 ToLower

	Convert all characters to lower case.

 ******************************************************************************/

void
JString16::ToLower()
{
	for (JIndex i=0; i<itsStringLength; i++)
		{
		itsString[i] = JToLower(itsString[i]);
		}
}

/******************************************************************************
 ToUpper

	Convert all characters to upper case.

 ******************************************************************************/

void
JString16::ToUpper()
{
	for (JIndex i=0; i<itsStringLength; i++)
		{
		itsString[i] = JToUpper(itsString[i]);
		}
}

/******************************************************************************
 LocateSubstring

	Return the index corresponding to the start of the first occurrence of
	the given string in our string.

 ******************************************************************************/

JBoolean
JString16::LocateSubstring
	(
	const JCharacter16*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	*startIndex = 1;
	return LocateNextSubstring(str, strLength, caseSensitive, startIndex);
}

/******************************************************************************
 LocateNextSubstring

	Return the index corresponding to the start of the next occurrence of
	the given string in our string, starting at *startIndex.

	In:  *startIndex is first character to consider
	Out: If function returns kJTrue, *startIndex is location of next occurrence.
	     Otherwise, *startIndex is beyond end of string.

 ******************************************************************************/

JBoolean
JString16::LocateNextSubstring
	(
	const JCharacter16*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	if (IsEmpty())
		{
		*startIndex = 1;
		return kJFalse;
		}
	else if (*startIndex > itsStringLength)
		{
		return kJFalse;
		}

	assert( *startIndex > 0 );

	// if the given string is longer than we are, we can't contain it

	if (itsStringLength - *startIndex + 1 < strLength)
		{
		*startIndex = itsStringLength+1;
		return kJFalse;
		}

	// search forward for a match

	for (JIndex i=*startIndex; i<=itsStringLength - strLength + 1; i++)
		{
		if (JCompareMaxN(GetCharacterPtr(i), itsStringLength-i+1, str, strLength,
						 strLength, caseSensitive))
			{
			*startIndex = i;
			return kJTrue;
			}
		}

	// if we fall through, there was no match

	*startIndex = itsStringLength+1;
	return kJFalse;
}

/******************************************************************************
 LocateLastSubstring

	Return the index corresponding to the start of the last occurrence of
	the given string in our string.

 ******************************************************************************/

JBoolean
JString16::LocateLastSubstring
	(
	const JCharacter16*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	*startIndex = itsStringLength;
	return LocatePrevSubstring(str, strLength, caseSensitive, startIndex);
}

/******************************************************************************
 LocatePrevSubstring

	Return the index corresponding to the start of the previous occurrence of
	the given string in our string, starting at *startIndex.

	In:  *startIndex is first character to consider
	Out: If function returns kJTrue, *startIndex is location of prev occurrence.
	     Otherwise, *startIndex is zero.

 ******************************************************************************/

JBoolean
JString16::LocatePrevSubstring
	(
	const JCharacter16*	str,
	const JSize			strLength,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	if (IsEmpty() || *startIndex == 0)
		{
		*startIndex = 0;
		return kJFalse;
		}

	assert( *startIndex <= itsStringLength );

	// if the given string runs past our end, back up *startIndex

	const JSize spaceAtEnd = itsStringLength - *startIndex + 1;
	if (spaceAtEnd < strLength && itsStringLength >= strLength)
		{
		*startIndex = itsStringLength - strLength + 1;
		}
	else if (spaceAtEnd < strLength)
		{
		*startIndex = 0;
		return kJFalse;
		}

	// search backward for a match

	for (JIndex i=*startIndex; i>=1; i--)
		{
		if (JCompareMaxN(GetCharacterPtr(i), itsStringLength-i+1, str, strLength,
						 strLength, caseSensitive))
			{
			*startIndex = i;
			return kJTrue;
			}
		}

	// if we fall through, there was no match

	*startIndex = 0;
	return kJFalse;
}

/******************************************************************************
 BeginsWith

 ******************************************************************************/

JBoolean
JString16::BeginsWith
	(
	const JCharacter16*	str,
	const JSize			length,
	const JBoolean		caseSensitive
	)
	const
{
	if (length == 0)
		{
		return kJTrue;
		}
	else
		{
		JIndex i = 1;
		return LocatePrevSubstring(str, length, caseSensitive, &i);
		}
}

/******************************************************************************
 EndsWith

 ******************************************************************************/

JBoolean
JString16::EndsWith
	(
	const JCharacter16*	str,
	const JSize			length,
	const JBoolean		caseSensitive
	)
	const
{
	if (length == 0)
		{
		return kJTrue;
		}
	else if (itsStringLength >= length)
		{
		JIndex i = itsStringLength - length + 1;
		return LocateNextSubstring(str, length, caseSensitive, &i);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetSubstring

	Return a string containing the specified substring of our string.

	Not inline because it uses assert.

 ******************************************************************************/

JString16
JString16::GetSubstring
	(
	const JIndex firstCharIndex,
	const JIndex lastCharIndex
	)
	const
{
	assert( IndexValid(firstCharIndex) );
	assert( IndexValid(lastCharIndex) );
	assert( firstCharIndex <= lastCharIndex );

	return JString16(GetCharacterPtr(firstCharIndex), lastCharIndex - firstCharIndex + 1);
}

JString16
JString16::GetSubstring
	(
	const JIndexRange& range
	)
	const
{
	assert( !range.IsNothing() );

	if (range.IsEmpty())
		{
		return JString16();
		}
	else
		{
		return GetSubstring(range.first, range.last);
		}
}

/******************************************************************************
 Extract

	Fills substringList with the text of each range in rangeList.  Empty
	ranges are converted to NULL.

 *****************************************************************************/

void
JString16::Extract
	(
	const JArray<JIndexRange>&	rangeList,
	JPtrArray<JString16>*			substringList
	)
	const
{
	assert( substringList != NULL );

	substringList->CleanOut();

	const JSize count = rangeList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JIndexRange r = rangeList.GetElement(i);
		JString16* s          = NULL;
		if (!r.IsEmpty())
			{
			s = new JString16(itsString, r);
			assert( s != NULL );
			}
		substringList->Append(s);
		}
}

/******************************************************************************
 ReplaceSubstring

	Replace the specified substring with the given string.

 ******************************************************************************/

void
JString16::ReplaceSubstring
	(
	const JIndex		firstCharIndex,
	const JIndex		lastCharIndex,
	const JCharacter16*	str,
	const JSize			strLength
	)
{
	assert( IndexValid(firstCharIndex) );
	assert( IndexValid(lastCharIndex) );
	assert( firstCharIndex <= lastCharIndex );

	const JSize replaceLength = lastCharIndex - firstCharIndex + 1;
	const JSize newLength     = itsStringLength - replaceLength + strLength;

	const JSize len1 = firstCharIndex - 1;
	const JSize len2 = strLength;
	const JSize len3 = itsStringLength - lastCharIndex;

	// If we don't have space, or would use too much space, reallocate.

	if (itsAllocLength < newLength || itsAllocLength > newLength + itsBlockSize)
		{
		itsAllocLength = newLength + itsBlockSize;

		// allocate space for the result

		JCharacter16* newString = new JCharacter16[ itsAllocLength+1 ];
		assert( newString != NULL );

		// place the characters in front and behind

		memcpy(newString, itsString, len1 * sizeof(JCharacter16));
		memcpy(newString + len1 + len2, itsString + lastCharIndex, len3 * sizeof(JCharacter16));

		// throw out the original string and save the new one

		delete [] itsString;
		itsString = newString;
		}

	// Otherwise, shift characters to make space.

	else
		{
		memmove(itsString + len1 + len2, itsString + lastCharIndex, len3 * sizeof(JCharacter16));
		}

	// insert the new characters

	memcpy(itsString + len1, str, len2 * sizeof(JCharacter16));

	// terminate

	itsString[ newLength ] = '\0';

	itsStringLength = newLength;
}

/******************************************************************************
 MatchCase

	Interface to JMatchCase().

 *****************************************************************************/

JBoolean
JString16::MatchCase
	(
	const JCharacter16*	source,
	const JIndexRange&	sourceRange,
	const JIndexRange&	destRange
	)
{
	assert( source != NULL );

	if (IsEmpty() || sourceRange.IsEmpty() || destRange.IsEmpty())
		{
		return kJFalse;
		}

	assert( RangeValid(destRange) );

	return JMatchCase(source, sourceRange, itsString, destRange);
}

/******************************************************************************
 ConvertTo*

	We cannot provide a public interface that takes char* + length because
	strto*() does not provide this interface.  We use it internally as an
	optimization since JString16s know their length.

	If we tried to provide the char* + length interface, we could not stop
	strto*() from gobbling digits beyond length, which would obviously be an
	error.

 ******************************************************************************/

/******************************************************************************
 ConvertToFloat (static private)

	Convert string to a floating point value.  Returns kJTrue if successful.

	This function accepts hex values like 0x2A.

 ******************************************************************************/

JBoolean
JString16::ConvertToFloat
	(
	const JCharacter16*	str,
	const JSize			length,
	JFloat*				value
	)
{
	if (IsHexValue(str, length))
		{
		JUInt v;
		const JBoolean ok = ConvertToUInt(str, length, &v);
		*value = v;
		return ok;
		}

	jclear_errno();
#ifdef _TODO
	char* endPtr;
	*value = strtod(str, &endPtr);
	if (jerrno_is_clear() && CompleteConversion(str, length, endPtr))
		{
		return kJTrue;
		}
	else
#endif
		{
		*value = 0.0;
		return kJFalse;
		}
}

/******************************************************************************
 ConvertToInteger (static private)

	Convert string to a signed integer.  Returns kJTrue if successful.

	base must be between 2 and 36 inclusive.
	If the string begins with 0x or 0X, base is forced to 16.

 ******************************************************************************/

JBoolean
JString16::ConvertToInteger
	(
	const JCharacter16*	str,
	const JSize			length,
	JInteger*			value,
	const JSize			origBase
	)
{
	JSize base = origBase;
	if (IsHexValue(str, length))
		{
		base = 0;	// let strtol notice "0x"
		}

	jclear_errno();
#ifdef _TODO
	char* endPtr;
	*value = strtol(str, &endPtr, base);
	if (jerrno_is_clear() && CompleteConversion(str, length, endPtr))
		{
		return kJTrue;
		}
	else
#endif
		{
		*value = 0;
		return kJFalse;
		}
}

/******************************************************************************
 ConvertToUInt (static private)

	Convert string to an unsigned integer.  Returns kJTrue if successful.

	base must be between 2 and 36 inclusive.
	If the string begins with 0x or 0X, base is forced to 16.

 ******************************************************************************/

JBoolean
JString16::ConvertToUInt
	(
	const JCharacter16*	str,
	const JSize			length,
	JUInt*				value,
	const JSize			origBase
	)
{
	JSize base = origBase;
	if (IsHexValue(str, length))
		{
		base = 0;	// let strtoul notice "0x"
		}
	else
		{
		// We do not let strtoul() wrap negative numbers.

		JIndex i=0;
		while (i < length && isspace(str[i]))
			{
			i++;
			}
		if (i < length && str[i] == '-')
			{
			*value = 0;
			return kJFalse;
			}
		}

	jclear_errno();
#ifdef _TODO
	char* endPtr;
	*value = strtoul(str, &endPtr, base);
	if (jerrno_is_clear() && CompleteConversion(str, length, endPtr))
		{
		return kJTrue;
		}
	else
#endif
		{
		*value = 0;
		return kJFalse;
		}
}

/******************************************************************************
 IsHexValue (static private)

	Returns kJTrue if string begins with whitespace+"0x".

 ******************************************************************************/

JBoolean
JString16::IsHexValue
	(
	const JCharacter16*	str,
	const JSize			length
	)
{
	JIndex i=0;
	while (i < length && isspace(str[i]))
		{
		i++;
		}

	return JI2B(i < length-2 && str[i] == '0' &&
				(str[i+1] == 'x' || str[i+1] == 'X'));
}

/******************************************************************************
 CompleteConversion (static private)

	Returns kJTrue if convEndPtr is beyond the end of the string or the
	remainder is only whitespace.

 ******************************************************************************/

JBoolean
JString16::CompleteConversion
	(
	const JCharacter16*	startPtr,
	const JSize			length,
	const JCharacter16*	convEndPtr
	)
{
	if (convEndPtr == startPtr)		// avoid behavior guaranteed by strto*()
		{
		return kJFalse;
		}

	const JCharacter16* endPtr = startPtr + length;
	while (convEndPtr < endPtr)
		{
		if (!isspace(*convEndPtr))
			{
			return kJFalse;
			}
		convEndPtr++;
		}
	return kJTrue;
}

/******************************************************************************
 Read

	Read the specified number of characters from the stream.
	This replaces the current contents of the string.

 ******************************************************************************/

void
JString16::Read
	(
	istream&	input,
	const JSize	count
	)
{
	if (itsAllocLength < count || itsAllocLength == 0)
		{
		itsAllocLength = count + itsBlockSize;

		// We allocate the new memory first.
		// If new fails, we still have the old string data.

		JCharacter16* newString = new JCharacter16 [ itsAllocLength + 1 ];
		assert( newString != NULL );

		// now it's safe to throw out the old data

		delete [] itsString;
		itsString = newString;
		}

	input.read((char*)itsString, count * sizeof(JCharacter16));
	itsStringLength = input.gcount() / sizeof(JCharacter16);
	itsString[ itsStringLength ] = '\0';
}

/******************************************************************************
 ReadDelimited

	Read a string of quote delimited characters from the stream.  Double
	quote (") and backslash (\) characters that are part of the string must
	be preceded by a backslash.  This replaces the current contents of the
	string.

 ******************************************************************************/

void
JString16::ReadDelimited
	(
	istream& input
	)
{
	// skip white space

	input >> ws;

	// the string must start with a double quote

	JCharacter c;
	input.get(c);
	if (c != '"')
		{
		input.putback(c);
		JSetState(input, ios::failbit);
		return;
		}

	// read until we hit a non-backslashed double quote

	Clear();

	const JSize bufSize = 100;
	JCharacter16 buf[ bufSize ];

	JIndex i = 0;
	while (1)
		{
		input.get(c);
		if (input.fail())
			{
			break;
			}
		else if (input.eof())
			{
			JSetState(input, ios::failbit);
			break;
			}
		else if (c == '\\')
			{
			input.get(c);
			if (input.fail())
				{
				break;
				}
			}
		else if (c == '"')
			{
			break;
			}

		buf[i] = c;
		i++;
		if (i == bufSize)
			{
			Append(buf, bufSize);
			i=0;
			}
		}

	Append(buf, i);
}

/******************************************************************************
 Print

	Display the text in such a way that the user can understand it.
	(i.e. don't display it in quotes, and don't use the internal \")

 ******************************************************************************/

void
JString16::Print
	(
	ostream& output
	)
	const
{
  output.write((const char*)itsString, itsStringLength * sizeof(JCharacter16));
}

/******************************************************************************
 Global functions for JString16 class

 ******************************************************************************/

/******************************************************************************
 Diacritical map

	Tells us how to remove diacritical marks.

 ******************************************************************************/

JBoolean
JGetDiacriticalMap
	(
	const JCharacter16**	map,
	const JIndex**		markType
	)
{
	*map      = kDiacriticalMap;
	*markType = kDiacriticalMarkType;
	return JI2B( kDiacriticalMap != NULL && kDiacriticalMarkType != NULL );
}

void
JSetDiacriticalMap
	(
	const JCharacter16*	map,
	const JIndex*		markType
	)
{
	kDiacriticalMap      = map;
	kDiacriticalMarkType = markType;
/*
	delete kToLowerMap;
	kToLowerMap = NULL;

	delete kToUpperMap;
	kToUpperMap = NULL;

	if (map != NULL && markType != NULL)
		{
		kToLowerMap = new JCharacter16 [ UCHAR_MAX+1 ];
		assert( kToLowerMap != NULL );

		kToUpperMap = new JCharacter16 [ UCHAR_MAX+1 ];
		assert( kToUpperMap != NULL );

		for (int i=0; i<=UCHAR_MAX; i++)
			{
			JBoolean found = kJFalse;
			if (kDiacriticalMarkType[i] == 0)
				{
				kToLowerMap[i] = tolower(i);
				kToUpperMap[i] = toupper(i);
				found          = kJTrue;
				}
			else if (JIsUpper(i))
				{
				kToLowerMap[i] = tolower(kDiacriticalMap[i]);
				kToUpperMap[i] = i;

				for (int j=0; j<=UCHAR_MAX; j++)
					{
					if (kDiacriticalMap[j]      == kToLowerMap[i] &&
						kDiacriticalMarkType[j] == kDiacriticalMarkType[i])
						{
						kToLowerMap[i] = j;
						found          = kJTrue;
						}
					}
				}
			else if (JIsLower(i))
				{
				kToLowerMap[i] = i;
				kToUpperMap[i] = toupper(kDiacriticalMap[i]);

				for (int j=0; j<=UCHAR_MAX; j++)
					{
					if (kDiacriticalMap[j]      == kToUpperMap[i] &&
						kDiacriticalMarkType[j] == kDiacriticalMarkType[i])
						{
						kToUpperMap[i] = j;
						found          = kJTrue;
						}
					}
				}

			if (!found)
				{
				kToLowerMap[i] = i;
				kToUpperMap[i] = i;
				}
			}
		}
*/
}

/******************************************************************************
 JString16Compare

	Returns the same as strcmp(): + if s1>s2, 0 if s1==s2, - if s1<s2

 ******************************************************************************/

inline int
jDiffChars
	(
	JCharacter16		c1,
	JCharacter16		c2,
	const JBoolean	caseSensitive
	)
{
	if (kDiacriticalMap != NULL)	// remove diacritical marks
		{
//		if (c1 < 0 || c2 < 0)
//			{
//			cout << c1 << ' ' << kDiacriticalMap[ (unsigned char) c1 ];
//			cout << c2 << ' ' << kDiacriticalMap[ (unsigned char) c2 ] << endl;
//			}
		c1 = kDiacriticalMap[ (unsigned char) c1 ];
		c2 = kDiacriticalMap[ (unsigned char) c2 ];
		}
	return (caseSensitive ? (c1 - c2) : (tolower(c1) - tolower(c2)));
}

int
JString16Compare
	(
	const JCharacter16*	s1,
	const JCharacter16*	s2,
	const JBoolean		caseSensitive
	)
{
	while (*s1 != '\0' && *s2 != '\0')
		{
		int c = jDiffChars(*s1, *s2, caseSensitive);
		if (c != 0)
			{
			return c;
			}
		s1++;
		s2++;
		}
	return jDiffChars(*s1, *s2, caseSensitive);
/*
	if (caseSensitive)
		{
		return strcoll(s1, s2);
		}
	else
		{
		return strcasecmp(s1, s2);
		}
*/
}

int
JString16Compare
	(
	const JCharacter16*	s1,
	const JSize			length1,
	const JCharacter16*	s2,
	const JSize			length2,
	const JBoolean		caseSensitive
	)
{
	const JCharacter16* end1 = s1 + length1;
	const JCharacter16* end2 = s2 + length2;

	while (s1 < end1 && s2 < end2)
		{
		int c = jDiffChars(*s1, *s2, caseSensitive);
		if (c != 0)
			{
			return c;
			}
		s1++;
		s2++;
		}

	if (s1 == end1 && s2 == end2)
		{
		return 0;
		}
	else if (s1 == end1)
		{
		return -1;
		}
	else
		{
		assert( s2 == end2 );
		return +1;
		}
/*
	int r;
	if (caseSensitive)
		{
		r = strncmp(s1, s2, JMin(length1, length2));
		}
	else
		{
		r = strncasecmp(s1, s2, JMin(length1, length2));
		}

	if (r == 0 && length1 < length2)
		{
		r = -1;
		}
	else if (r == 0 && length1 > length2)
		{
		r = +1;
		}
	return r;
*/
}

/******************************************************************************
 JCompareMaxN

	Returns kJTrue if the first N characters of the two strings are equal.
	(replaces strncmp())

 ******************************************************************************/

JBoolean
JCompareMaxN
	(
	const JCharacter16*	s1,
	const JCharacter16*	s2,
	const JSize			N,
	const JBoolean		caseSensitive
	)
{
	for (JIndex i=0; i<N; i++)
		{
		if (jDiffChars(s1[i], s2[i], caseSensitive) != 0)
			{
			return kJFalse;
			}
		else if (s1[i] == '\0' && s2[i] == '\0')	// same length, shorter than N
			{
			return kJTrue;
			}
		}

	return kJTrue;
}

JBoolean
JCompareMaxN
	(
	const JCharacter16*	s1,
	const JSize			length1,
	const JCharacter16*	s2,
	const JSize			length2,
	const JSize			N,
	const JBoolean		caseSensitive
	)
{
	for (JIndex i=0; i<length1 && i<length2 && i<N; i++)
		{
		if (jDiffChars(s1[i], s2[i], caseSensitive) != 0)
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 JCalcMatchLength

	Calculates the number of characters that match from the beginning
	of the given strings.

	JCalcMatchLength("abc", "abd")          -> 2
	JCalcMatchLength("abc", "xyz")          -> 0
	JCalcMatchLength("abc", "aBd", kJFalse) -> 2

 *****************************************************************************/

JSize
JCalcMatchLength
	(
	const JCharacter16*	s1,
	const JCharacter16*	s2,
	const JBoolean		caseSensitive
	)
{
	JSize i = 0;
	while (jDiffChars(s1[i], s2[i], caseSensitive) == 0 &&
		   s1[i] != '\0')	// kJTrue => s2[i] != '\0'
		{
		i++;
		}
	return i;
}

/******************************************************************************
 JCopyMaxN

	A version of strncpy() without the horrible bug.  In addition, it returns
	kJTrue if the entire string was copied or kJFalse if it wasn't, since the
	strncpy() return value is totally useless.  The name maxBytes should remind
	you that there must also be room for a null terminator.

 *****************************************************************************/

JBoolean
JCopyMaxN
	(
	const JCharacter16*	source,
	const JIndex		maxBytes,
	JCharacter16*			destination
	)
{
	JIndex i;
	for (i=0; i<maxBytes; i++)
		{
		destination[i] = source[i];
		if (destination[i] == '\0')
			{
			return kJTrue;
			}
		}

	destination[maxBytes-1] = '\0';
	return kJFalse;
}

/******************************************************************************
 JMatchCase

	Adjusts the case of destRange in dest to match the case of sourceRange
	in source.  Returns kJTrue if any changes were made.

	If sourceRange and destRange have the same length, we match the
	case of each character individually.

	Otherwise:

	If both first characters are letters, the first letter of destRange is
	adjusted to the same case as the first character of sourceRange.

	If the rest of sourceRange contains at least one alphabetic character
	and all its alphabetic characters have the same case, all the alphabetic
	characters in destRange are coerced to that case.

 *****************************************************************************/

JBoolean
JMatchCase
	(
	const JCharacter16*	source,
	const JIndexRange&	sourceRange,
	JCharacter16*			dest,
	const JIndexRange&	destRange
	)
{
	if (JString16Empty(source) || sourceRange.IsEmpty() ||
		JString16Empty(dest)   || destRange.IsEmpty())
		{
		return kJFalse;
		}

	JBoolean changed      = kJFalse;
	const JSize sourceLen = sourceRange.GetLength();
	const JSize destLen   = destRange.GetLength();

	// if not the same length, match all but first character

	if (destLen > 1 && sourceLen != destLen)
		{
		JBoolean hasUpper = kJFalse;
		JBoolean hasLower = kJFalse;
		for (JIndex i = sourceRange.first-1; i < sourceRange.last; i++)
			{
			if (JIsLower(source[i]))
				{
				hasLower = kJTrue;
				}
			else if (JIsUpper(source[i]))
				{
				hasUpper = kJTrue;
				}

			if (hasLower && hasUpper)
				{
				break;
				}
			}

		// first character is fixed separately below

		if (hasLower && !hasUpper)
			{
			for (JIndex i = destRange.first-1; i < destRange.last; i++)
				{
				dest[i] = JToLower(dest[i]);
				}
			changed = kJTrue;
			}
		else if (hasUpper && !hasLower)
			{
			for (JIndex i = destRange.first-1; i < destRange.last; i++)
				{
				dest[i] = JToUpper(dest[i]);
				}
			changed = kJTrue;
			}
		}

	// if not same length, match first character
	// else, match all characters

	const JIndex endIndex = (sourceLen == destLen ? sourceLen : 1);
	for (JIndex i=0; i<endIndex; i++)
		{
		const JIndex j = sourceRange.first-1+i;
		const JIndex k = destRange.first  -1+i;
		if (JIsLower(source[j]) && JIsUpper(dest[k]))
			{
			dest[k] = JToLower(dest[k]);
			changed = kJTrue;
			}
		else if (JIsUpper(source[j]) && JIsLower(dest[k]))
			{
			dest[k] = JToUpper(dest[k]);
			changed = kJTrue;
			}
		}

	return changed;
}

/******************************************************************************
 Character types

	Replacements for isalpha(), etc. that understand European letters.
	setlocale() makes most of the standard functions work correctly.

 ******************************************************************************/

JBoolean
JIsPrint
	(
	const JCharacter16 c
	)
{
	return JI2B( isprint(c) /* || (c & 0x80) */ );
}

JBoolean
JIsAlnum
	(
	const JCharacter16 c
	)
{
	return JI2B( JIsAlpha(c) || isdigit(c) );
}

JBoolean
JIsAlpha
	(
	const JCharacter16 c
	)
{
//	if (kDiacriticalMap != NULL)
//		{
//		return JI2B( isalpha(kDiacriticalMap[ (unsigned char) c ]) );
//		}
//	else
//		{
		return JI2B( isalpha(c) );
//		}
}

JBoolean
JIsUpper
	(
	const JCharacter16 c
	)
{
//	if (kDiacriticalMap != NULL)
//		{
//		return JI2B( isupper(kDiacriticalMap[ (unsigned char) c ]) );
//		}
//	else
//		{
		return JI2B( isupper(c) );
//		}
}

JBoolean
JIsLower
	(
	const JCharacter16 c
	)
{
//	if (kDiacriticalMap != NULL)
//		{
//		return JI2B( islower(kDiacriticalMap[ (unsigned char) c ]) );
//		}
//	else
//		{
		return JI2B( islower(c) );
//		}
}

/******************************************************************************
 Case conversion

	Replacements for toupper() and tolower() that understand European letters.
	setlocale() makes the standard functions work correctly.

 ******************************************************************************/

JCharacter16
JToUpper
	(
	const JCharacter16 c
	)
{
//	if (kToUpperMap != NULL)
//		{
//		return kToUpperMap[ (unsigned char) c ];
//		}
//	else
//		{
		return toupper(c);
//		}
}

JCharacter16
JToLower
	(
	const JCharacter16 c
	)
{
//	if (kToLowerMap != NULL)
//		{
//		return kToLowerMap[ (unsigned char) c ];
//		}
//	else
//		{
		return tolower(c);
//		}
}

/******************************************************************************
 Stream operators

	The string data is delimited by double quotes:  "this is a string".

	To include double quotes in a string, use \"
	To include a backslash in a string, use \\

	An exception is made if the streams are cin or cout.
	For input, characters are simply read until 'return' is pressed.
	For output, Print() is used.

 ******************************************************************************/

istream&
operator>>
	(
	istream&	input,
	JString16&	aString
	)
{
#ifdef _TODO
	if (&input == &cin)
		{
		// Read characters until return is pressed.
		// Following convention, the return must be left in the stream.

		aString = JReadLine(cin);
		cin.putback('\n');
		aString.TrimWhitespace();
		}
	else
		{
		// Read quote delimited string of characters.

		aString.ReadDelimited(input);
		}
#endif

	// allow chaining

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
	const JString16&	aString
	)
{
#ifdef _TODO
	if (&output == &cout)
		{
		aString.Print(cout);
		return output;
		}
	else if (&output == &cerr)
		{
		aString.Print(cerr);
		return output;
		}

	// write the starting delimiter

	output << '"';

	// write the string data, substituting \" in place of "

	const JSize length = aString.GetLength();

	for (JIndex i=1; i<=length; i++)
		{
		const JCharacter16 c = aString.PrivateGetCharacter(i);

		if (c == '"')
			{
			output << '\\' << '"';
			}
		else if (c == '\\')
			{
			output << '\\' << '\\';
			}
		else
			{
			output << c;
			}
		}

	// write the ending delimiter

	output << '"';
#endif

	// allow chaining

	return output;
}

/*-----------------------------------------------------------------------------
 Routines required for number<->string conversion

 -----------------------------------------------------------------------------*/

// private routines

static void  Round(int start, int B, int D[], int *exp, const int sigDigitCount);
static void  Shift(int* start, int D[], int* exp, const int sigDigitCount);

static void  JAssignString(JCharacter16 s1[], char *s2);
static void  JInsertSubstring(JCharacter16 s1[], short pos1, JCharacter16 *s2, short pos2, short pos3);
static void  JShiftSubstring(JCharacter16 s[], short pos1, short pos2, short shift);
static short JLocateSubstring(JCharacter16 *s1, JCharacter16 *s2);
//static void  JTrimSpaces(char* s);

inline void
JAppendChar(JCharacter16 s1[], JCharacter16 c)
{
	const int len = strlen16(s1);
	s1[len]   = c;
	s1[len+1] = '\0';
}

/*-----------------------------------------------------------------------------
 NumConvert.c (encapsulated by JString16 class)

	Assembled routines for dealing with string<->number conversion.

	Copyright � 1992 John Lindal. All rights reserved.

 -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 double2str16 (adapted from FNUM in FORTRAN)

	Converts doubleVal to a string stored at returnStr.

	afterDec = # of digits after decimal point, doubleVal is rounded appropriately
	           -1 => as many as needed, -2 => truncate, 0 => round up

	sigDigitCount = # of significant digits
	                0 => use as many as possible

	exp = desired exponent, decimal point is shifted appropriately
	      kStandardExponent => use exponent if magnitude >=7
	      kForceExponent    => force use of exponent

 -----------------------------------------------------------------------------*/

void
double2str16
	(
	double	doubleVal,
	int		afterDec,
	int		sigDigitCount,
	int		expMin,
	JCharacter16	returnStr[]
	)
{
int i,j;

	if (sigDigitCount <= 0 || sigDigitCount > JString16::kDefSigDigitCount)
		{
		sigDigitCount = JString16::kDefSigDigitCount;
		}

	JBoolean neg=kJFalse;
	if (doubleVal<0.0)					// can't take log of x<0
		{
		neg       = kJTrue;
		doubleVal = -doubleVal;
		}

	// get exponent (bump it up slightly if exact multiple of 10)

	int exp = (doubleVal == 0.0) ? 0 : JLFloor(log10(doubleVal*1.00001))+1;

	// compute digit values

	int      D[ JString16::kDefSigDigitCount+1 ];
	JBoolean A[ JString16::kDefSigDigitCount+1 ];

	double chewedVal = doubleVal / pow(10.0, exp-1);
	for (i=0; i<=sigDigitCount; i++)
		{
		A[i]      = kJTrue;
		D[i]      = JLFloor(chewedVal) - 10*JLFloor(chewedVal/10.0);
		chewedVal = (chewedVal - D[i]) * 10.0 ;
		}

	// adjust digits that are outside [0,9]

	Round(sigDigitCount, sigDigitCount, D, &exp, sigDigitCount);
	if (D[sigDigitCount] > 4)
		{
		Round(sigDigitCount-1, sigDigitCount, D, &exp, sigDigitCount);
		}
	else
		{
		D[sigDigitCount] = 0;
		}

	// round up if lots of 9's on end (egcs 1.1.2:  0.6, 9.995)

	j=0;
	for (i=sigDigitCount-1; i>=0; i--)
		{
		if (D[i]==9)
			{
			j++;
			}
		else
			{
			break;
			}
		}
	if (j >= 3)
		{
		Round(sigDigitCount-j, sigDigitCount, D, &exp, sigDigitCount);
		}

	// set the exponent

	// There is no point in displaying an exponent until the zeroes start
	// piling up on the end.
	const int expMax = sigDigitCount+1;

	exp--;
	if (expMin == JString16::kStandardExponent)
		{
		expMin = (-expMax<=exp && exp<=expMax) ? 0 : exp;
		}
	else if (expMin == JString16::kForceExponent)
		{
		expMin = exp;
		}
	else if (doubleVal == 0.0)
		{
		expMin = 0;
		}

	// adjust digits to fit specs

	if (afterDec != -1)
		{
		JBoolean truncate = kJFalse;
		if (afterDec == -2)
			{
			afterDec = 0;
			truncate = kJTrue;
			}

		// adjust for spec'd # digits after dp

		int start = 1 + afterDec + (exp-expMin);
		if (start < 1)
			{
			Shift(&start,D,&exp, sigDigitCount);
			}
		else if (start > sigDigitCount)
			{
			start = sigDigitCount;
			}

		// round or truncate

		if (!truncate && D[start]>4)
			{
			Round(start-1,0,D,&exp, sigDigitCount);
			}
		else
			{
			for (i=start; i<=sigDigitCount; i++)
				{
				D[i]=0;
				}
			}
		}

	// strip trailing zeros

	for (i=sigDigitCount; i>=0; i--)
		{
		if (D[i]==0)
			{
			A[i]=kJFalse;
			}
		else
			{
			break;
			}
		}

	// create string of digits

	for (i=0; i<=sigDigitCount; i++)
		{
		if (A[i])
			{
			returnStr[i]='0'+D[i];
			}
		else
			{
			returnStr[i]='\0';
			break;
			}
		}

	if (returnStr[0] == '\0')			// number has ended up as zero
		{
		exp    = 0;
		expMin = 0;
		}
	else if (returnStr[0] == '0' &&
			 returnStr[1] == '\0')		// "-0" is silly
		{
		neg=kJFalse;
		}

	// prepend leading zero's and decimal point

	if (expMin>exp)
		{
		if (expMin-exp>1)
			{
			JCharacter16 temp[] = {'0', 0};
			for (j=0; j<=expMin-exp-2; j++)
				{
				JInsertSubstring(returnStr,0,temp,0,1);
				}
			}
		if (returnStr[0] == '\0')						// 0. is silly
			{
			returnStr[0] = '0';
			returnStr[1] = '\0';
			}
		else
			{
			JCharacter16 temp[] = {'0', '.', 0};
			JInsertSubstring(returnStr,0,temp,0,2);		// prepend decimal point
			}
		}

	// append zero's or insert decimal point

	else
		{
		i=strlen16(returnStr)-1;
		if (exp-expMin>i)
			{
			for (j=1; j<=exp-expMin-i; j++)
				{
				JAppendChar(returnStr, '0');
				}
			}
		else if (exp-expMin<i)
			{
			JCharacter16 temp[] = {'.', 0};
			JInsertSubstring(returnStr,exp-expMin+1,temp,0,1);
			}
		}

	exp=expMin;

	// append decimal point and trailing zeros

	JCharacter16 temp[] = {'.', 0};
	i = JLocateSubstring(returnStr,temp) + 1;
	const int decPt = (i == 0 ? 0 : strlen16(returnStr)-i);
	if (afterDec > decPt)
		{
		if (decPt==0)
			{
			JAppendChar(returnStr, '.');
			}
		for (i=decPt+1; i<=afterDec; i++)
			{
			JAppendChar(returnStr, '0');
			}
		}

	// append exponent

	if (exp != 0)
		{
		JAppendChar(returnStr, 'e');
		if (exp>0)
			{
			JAppendChar(returnStr, '+');
			}
		else
			{
			JAppendChar(returnStr, '-');
			exp=-exp;
			}

		double2str16(exp,0,0,0, returnStr + strlen16(returnStr));
		}

	// prepend negative sign

	if (neg)
		{
		JCharacter16 temp[] = {'-', 0};
		JInsertSubstring(returnStr,0,temp,0,1);
		}
}

/*-----------------------------------------------------------------------------
 Round (from ROUND in FORTRAN)

	Rounds D starting from start.

 -----------------------------------------------------------------------------*/

void
Round
	(
	int			start,
	int			B,
	int			D[],
	int*		exp,
	const int	sigDigitCount
	)
{
int i;

	// clear digits beyond where we will round

	for (i=start+1; i<=sigDigitCount; i++)
		{
		D[i]=0;
		}

	// round up digit # start

	if (start < sigDigitCount)
		{
		D[start]++;
		}

	// move 10 up to next digit

	if (start > 0)
		{
		for (i=start; i>=1; i--)
			{
			if (D[i]>9)
				{
				D[i]=0;
				D[i-1]++;
				}
			}
		}

	// make space for extra digit in front

	if (D[0] > 9)
		{
		for (i=sigDigitCount; i>=2; i--)
			{
			D[i]=D[i-1];
			}
		D[0]=1;
		D[1]=0;
		(*exp)++;
		}
}

/*-----------------------------------------------------------------------------
 Shift (from SHIFT in FORTRAN)

	Shifts digits in D array so that start can be increased to 1.
	This takes care of the situation when doubleVal=0.002 and afterDec=0.

 -----------------------------------------------------------------------------*/

void
Shift
	(
	int*		start,
	int			D[],
	int*		exp,
	const int	sigDigitCount
	)
{
int i;

	// calculate how far to shift each digit

	const int shift = 1 - *start;

	// shift digits

	if (shift < sigDigitCount+1)
		{
		for (i=sigDigitCount; i>=shift; i--)
			{
			D[i]=D[i-shift];
			}

		// set preceding digits to zero

		for (i=0; i<shift; i++)
			{
			D[i]=0;
			}
		}

	// shift too large, just clear all digits

	else
		{
		for (i=0; i<=sigDigitCount; i++)
			{
			D[i]=0;
			}
		}

	(*start)=1;			// shifting digits fixed start
	(*exp)+=shift;		// exponent has changed
}

/*-----------------------------------------------------------------------------
 str2double (from FSTR in FORTRAN)

	Converts the C string numStr to doubleVal.  Returns kJFalse if an error occurs.

	We use recursion to handle the exponent.

 -----------------------------------------------------------------------------*/
/*
JBoolean str2double(char *numStr1,double *doubleVal)
{
int			i,numlen,decPt,decEnd,expMrk,digit;
char		numStr[256],expNumStr[100];
double		exp0,exp1;
JBoolean	neg,valid;

	*doubleVal=0.0; JAssignString(numStr,numStr1);

	JTrimSpaces(numStr);

	numlen=strlen16(numStr);
	if (numlen==0) return kJFalse;	// empty string

	decPt=-1; decEnd=-1; expMrk=-1; neg=kJFalse;
	if (numStr[0]=='+' || numStr[0]=='-') {			// trim sign
		if (numStr[0]=='-') neg=kJTrue;
		JShiftSubstring(numStr,1,-1,-1); numlen--;
		}

	for (i=1;i<=numlen;i++)
		{
		const char c = numStr[i-1];
		if (c == '.')									// found dp
			decPt=i;
//		else if (c == ' ') {							// end of digits
//			if (decPt==-1 && expMrk==-1) decPt=i;		// forced dp
//			if (decPt>-1 && decEnd==-1) decEnd=i;		// end of decimal string
//			}
		else if (c == 'E' || c == 'e' ||
				 c == 'D' || c == 'd')					// found exponent
			{
			if (i==1 || i==numlen) return kJFalse;		// no digits, no exponent string
			expMrk=i;
			if (decEnd==-1)				// forced end of decimal string
				decEnd=i;
			if (decPt==-1) {			// forced dp
				decPt=i; decEnd=i;
				}
			break;						// let recursion handle the exponent
			}
		else if (!isdigit(c))
			return kJFalse;
		}
	if (decPt==-1) decPt=numlen+1;		// forced dp
	if (decEnd==-1) decEnd=numlen+1;	// forced end of decimal string
	if (expMrk==-1) expMrk=numlen+1;	// forced exponent (zero)

	if (decPt==1 && decPt>=numlen) return kJFalse;	// no digits at all

	if (decPt!=1) {						// decode digits preceding dp
		for (i=1;i<=decPt-1;i++) {
			digit=numStr[i-1]-'0';
			if (digit<0 || digit>9) return kJFalse;		// not a number
			*doubleVal+=(digit*pow(10.0,decPt-i-1));	// add in weighted values
			}
		}

	if (decPt<numlen) {					// decode digits after dp
		for (i=decPt+1;i<=decEnd-1;i++) {
			digit=numStr[i-1]-'0';
			if (digit<0 || digit>9) return kJFalse;	// not a number
			*doubleVal+=(digit/pow(10.0,i-decPt));	// add in weighted values
			}
		}

	if (expMrk<numlen && *doubleVal > 0.0) {	// decode exponent
		exp0=log10(*doubleVal);
		expNumStr[0] = '\0';
		JInsertSubstring(expNumStr,0,numStr,expMrk,-1);
		valid=str2double(expNumStr,&exp1);			// recursive
		if (exp0+exp1>FLT_MAX_10_EXP) valid=kJFalse;	// exponent too big
		if (!valid) return kJFalse;
		i=JLFloor(exp1);
		*doubleVal=(*doubleVal)*pow(10.0,i);		// adjust number
		}

	if (neg) *doubleVal=-(*doubleVal);	// adjust sign

	return kJTrue;
}
*/
/*-----------------------------------------------------------------------------
 JString16.StrUtil.c (a stripped version required by NumConvert.c)

	Assembled routines for dealing with string conversion.
	These routines only require vanilla C.

	Copyright � 1992 John Lindal. All rights reserved.

 -----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 JAssignString

	Clears the s1 array and dumps in s2 (including termination).
	Assumes there are enough elements in s1 to hold s2.

 -----------------------------------------------------------------------------*/

void JAssignString(JCharacter16 s1[],JCharacter16 *s2)
{
short i,len2;

	len2=strlen16(s2);
	for (i=0;i<=len2;i++) s1[i]=*(s2+i);
}

/*-----------------------------------------------------------------------------
 JInsertSubstring (augments strcat in string.h)

	Inserts s2 into s1 at the specified postion in s1.
	pos# start at zero.  pos1 = -1 => append.  pos3 = -1 => to end of s2.
	Assumes there is enough space in s1 to hold s1+s2.

 -----------------------------------------------------------------------------*/

void JInsertSubstring(JCharacter16 s1[],short pos1,JCharacter16 *s2,short pos2,short pos3)
{
short i,len1,len2;

	len1=strlen16(s1); len2=strlen16(s2);
	if (pos1==-1 || pos1>len1) pos1=len1;
	if (pos2<0) pos2=0;
	if (pos3==-1 || pos3>len2) pos3=len2;
	JShiftSubstring(s1,pos1,-1,pos3-pos2);
	for (i=pos2;i<pos3;i++) s1[pos1+i-pos2]=*(s2+i);
}

/*-----------------------------------------------------------------------------
 JShiftSubstring

	Shifts characters in s from pos1 to pos2 by shift positions.
	Overwrites old characters.  Gives up if you try to shift past left end of string.
	pos# start at zero.  pos2 = -1 => end.  shift > 0 => shift to right.
	Assumes there is enough space in s to hold shifted results.

 -----------------------------------------------------------------------------*/

void JShiftSubstring(JCharacter16 s[],short pos1,short pos2,short shift)
{
short i,len;

	if (shift==0) return;

	len=strlen16(s);
	if (pos1<0) pos1=0;
	if (pos2==-1 || pos2>len) pos2=len;

	if (pos2<pos1 || pos1+shift<0) return;

	/* pos2=len => termination shifts too */

	if (shift<0)
		for (i=pos1;i<=pos2;i++) s[i+shift]=s[i];
	if (shift>0) {
		for (i=pos2;i>=pos1;i--) s[i+shift]=s[i];
		if (pos2+shift+1>len && pos2<len) s[pos2+shift+1]=0;	/* Terminate string */
		}
}

/*-----------------------------------------------------------------------------
 JLocateSubstring (equivalent to INDEX()-1 in FORTRAN)

	Returns the offset to the 1st occurrence of s2 inside s1.
	-1 => Not found.

 -----------------------------------------------------------------------------*/

short JLocateSubstring(JCharacter16 *s1,JCharacter16 *s2)
{
short i,len1,len2;

	len1=strlen16(s1); len2=strlen16(s2);

	for (i=0;i<=len1-len2;i++)
		if (strncmp16(s1+i,s2,len2)==0) return i;

	return -1;
}

/*-----------------------------------------------------------------------------
 JTrimSpaces

	Trims off leading and trailing spaces from s.

 -----------------------------------------------------------------------------*/
/*
void JTrimSpaces(char* s)
{
short i,j,len;

	len = strlen16(s);
	while (len>0 && s[len-1]==' ') {	// shift termination
		s[len-1]=0; len--;
		}

	j=0;
	while (j<len && s[j]==' ') j++;
	for (i=0;i<len-j;i++) s[i]=s[i+j];

	s[i]=0;			// terminate
}
*/

size_t strlen16(const JCharacter16* str)
{
	size_t	len = -1;
	
#if !__POWERPC__
	
	do
		len++;
	while (*str++);
	
#else
	
	JCharacter16 * p = (JCharacter16 *) str - 1;
	
	do
		len++;
	while (*++p);
	
#endif
	
	return(len);
}

int strncmp16(const JCharacter16 * str1, const JCharacter16 * str2, size_t n)
{
#if !__POWERPC__
	
	const	JCharacter16 * p1 = (JCharacter16 *) str1;
	const	JCharacter16 * p2 = (JCharacter16 *) str2;
				JCharacter16		c1, c2;
	
	n++;
	
	while (--n)
		if ((c1 = *p1++) != (c2 = *p2++))
			return(c1 - c2);
		else if (!c1)
			break;
	
#else
	
	const	JCharacter16 * p1 = (JCharacter16 *) str1 - 1;
	const	JCharacter16 * p2 = (JCharacter16 *) str2 - 1;
				JCharacter16		c1, c2;
	
	n++;
	
	while (--n)
		if ((c1 = *++p1) != (c2 = *++p2))
			return(c1 - c2);
		else if (!c1)
			break;

#endif
	
	return(0);
}

