/******************************************************************************
 JString16.h

	Interface for the JString16 class

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JString16
#define _H_JString16

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#include <JIndexRange.h>
#include <string.h>

size_t strlen16(const JCharacter16* ustr);
int strncmp16(const JCharacter16 * s1, const JCharacter16 * s2, size_t n);

class JString;

class JString16
{
	friend istream& operator>>(istream&, JString16&);
	friend ostream& operator<<(ostream&, const JString16&);

public:

	enum
	{
		kPrecisionAsNeeded = -1,
		kDefSigDigitCount  = 16
	};

	enum ExponentDisplay
	{
		kStandardExponent = -10000,
		kForceExponent    = -9999,
		kForceNoExponent  = 0,
		kUseGivenExponent = 1
	};

	enum Base	// sorry, have to use this to make ctors distinct
	{
		kBase10 = 10,
		kBase2  = 2,
		kBase8  = 8,
		kBase16 = 16
	};

public:

	JString16();
	JString16(const JString16& source);
	JString16(const JCharacter16* str);
	JString16(const JCharacter16* str, const JSize length);
	JString16(const JCharacter16* str, const JIndexRange& range);
	JString16(const JFloat number, const JInteger precision = kPrecisionAsNeeded,
			const ExponentDisplay expDisplay = kStandardExponent,
			const JInteger exponent = 0, const JInteger sigDigitCount = 0);
	JString16(const JUInt number, const Base base, const JBoolean pad = kJFalse);

	~JString16();

	const JString16& operator=(const JString16& str);
	const JString16& operator=(const JCharacter16* str);

	JString16& operator+=(const JString16& str);
	JString16& operator+=(const JCharacter16* str);

	operator const JCharacter16*() const;

	void	Set(const JString16& str);
	void	Set(const JCharacter16* str);
	void	Set(const JCharacter16* str, const JSize length);
	void	Set(const JCharacter16* str, const JIndexRange& range);

	JBoolean			ContainsNULL() const;
	const JCharacter16*	GetCString() const;
	JCharacter16*		AllocateCString() const;	// client must call delete [] when finished with it

	JString	ToASCII() const;
	void	FromASCII(const JString& ascii);
	void	FromASCII(const JCharacter* ascii);
	JString	ToUTF8() const;
	void	FromUTF8(const JString& utf8);
	void	FromUTF8(const JCharacter* utf8);

	void	InsertSubstring(const JString16& str, const JIndex insertionIndex);
	void	InsertSubstring(const JCharacter16* str, const JIndex insertionIndex);
	void	InsertSubstring(const JCharacter16* str, const JSize length,
							const JIndex insertionIndex);
	void	InsertCharacter(const JCharacter16 c, const JIndex insertionIndex);

	void	Prepend(const JString16& str);
	void	Prepend(const JCharacter16* str);
	void	Prepend(const JCharacter16* str, const JSize length);
	void	PrependCharacter(const JCharacter16 c);

	void	Append(const JString16& str);
	void	Append(const JCharacter16* str);
	void	Append(const JCharacter16* str, const JSize length);
	void	AppendCharacter(const JCharacter16 c);

	JBoolean	IsEmpty() const;
	JSize		GetLength() const;
	JBoolean	IndexValid(const JIndex index) const;
	JBoolean	RangeValid(const JIndexRange& range) const;

	JCharacter16	GetCharacter(const JIndex index) const;
	void		SetCharacter(const JIndex index, const JCharacter16 c);

	JCharacter16	GetFirstCharacter() const;
	JCharacter16	GetLastCharacter() const;

	JCharacter16	GetCharacterFromEnd(const JIndex index) const;
	void		SetCharacterFromEnd(const JIndex index, const JCharacter16 c);

	void	Clear();
	void	TrimWhitespace();
	void	ToLower();
	void	ToUpper();

	JBoolean	Contains(const JString16& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JCharacter16* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	Contains(const JCharacter16* str, const JSize length,
						 const JBoolean caseSensitive = kJTrue) const;

	JBoolean	LocateSubstring(const JString16& str, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter16* str, JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter16* str, const JSize length,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JString16& str, const JBoolean caseSensitive,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter16* str, const JBoolean caseSensitive,
								JIndex* startIndex) const;
	JBoolean	LocateSubstring(const JCharacter16* str, const JSize length,
								const JBoolean caseSensitive, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JString16& str, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter16* str, JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter16* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JString16& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter16* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateNextSubstring(const JCharacter16* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;

	JBoolean	LocatePrevSubstring(const JString16& str, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter16* str, JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter16* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JString16& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter16* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocatePrevSubstring(const JCharacter16* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;

	JBoolean	LocateLastSubstring(const JString16& str, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter16* str, JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter16* str, const JSize length,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JString16& str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter16* str, const JBoolean caseSensitive,
									JIndex* startIndex) const;
	JBoolean	LocateLastSubstring(const JCharacter16* str, const JSize length,
									const JBoolean caseSensitive, JIndex* startIndex) const;

	JBoolean	BeginsWith(const JString16& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JCharacter16* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	BeginsWith(const JCharacter16* str, const JSize length,
						   const JBoolean caseSensitive = kJTrue) const;

	JBoolean	EndsWith(const JString16& str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JCharacter16* str, const JBoolean caseSensitive = kJTrue) const;
	JBoolean	EndsWith(const JCharacter16* str, const JSize length,
						 const JBoolean caseSensitive = kJTrue) const;

	JString16		GetSubstring(const JIndex firstCharIndex, const JIndex lastCharIndex) const;
	JString16		GetSubstring(const JIndexRange& range) const;	// allows empty range

	void		Extract(const JArray<JIndexRange>& rangeList,
						JPtrArray<JString16>* substringList) const;

	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JString16& str);
	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JCharacter16* str);
	void		ReplaceSubstring(const JIndex firstCharIndex,
								 const JIndex lastCharIndex,
								 const JCharacter16* str, const JSize length);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JString16& str, JIndexRange* newRange);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JCharacter16* str, JIndexRange* newRange);
	void		ReplaceSubstring(const JIndexRange& range,
								 const JCharacter16* str, const JSize length,
								 JIndexRange* newRange);

	void		RemoveSubstring(const JIndex firstCharIndex, const JIndex lastCharIndex);
	void		RemoveSubstring(const JIndexRange& range);

	JBoolean	MatchCase(const JString16& source, const JIndexRange& sourceRange);
	JBoolean	MatchCase(const JCharacter16* source, const JIndexRange& sourceRange);
	JBoolean	MatchCase(const JString16& source, const JIndexRange& sourceRange,
						  const JIndexRange& destRange);
	JBoolean	MatchCase(const JCharacter16* source, const JIndexRange& sourceRange,
						  const JIndexRange& destRange);

	JBoolean	IsFloat() const;
	JBoolean	ConvertToFloat(JFloat* value) const;

	JBoolean	IsInteger(const JSize base = 10) const;
	JBoolean	ConvertToInteger(JInteger* value, const JSize base = 10) const;

	JBoolean	IsUInt(const JSize base = 10) const;
	JBoolean	IsHexValue() const;
	JBoolean	ConvertToUInt(JUInt* value, const JSize base = 10) const;

	void		Read(istream& input, const JSize count);
	void		ReadDelimited(istream& input);
	void		Print(ostream& output) const;

	JSize		GetBlockSize() const;
	void		SetBlockSize(const JSize blockSize);

	static JBoolean	IsFloat(const JCharacter16* str);
	static JBoolean	ConvertToFloat(const JCharacter16* str, JFloat* value);

	static JBoolean	IsInteger(const JCharacter16* str, const JSize base = 10);
	static JBoolean	ConvertToInteger(const JCharacter16* str,
									 JInteger* value, const JSize base = 10);

	static JBoolean	IsUInt(const JCharacter16* str, const JSize base = 10);
	static JBoolean	IsHexValue(const JCharacter16* str);
	static JBoolean	ConvertToUInt(const JCharacter16* str,
								  JUInt* value, const JSize base = 10);

private:

	JCharacter16*	itsString;			// characters
	JSize			itsStringLength;	// number of characters used
	JSize			itsAllocLength;		// number of characters we have space for
	JSize			itsBlockSize;		// size by which to shrink and grow allocation

private:

	void		CopyToPrivateString(const JCharacter16* str);
	void		CopyToPrivateString(const JCharacter16* str, const JSize length);

	JCharacter16	PrivateGetCharacter(const JIndex index) const;
	JCharacter16*	GetCharacterPtr(const JIndex index) const;

	static JBoolean	ConvertToFloat(const JCharacter16* str, const JSize length,
								   JFloat* value);
	static JBoolean	ConvertToInteger(const JCharacter16* str, const JSize length,
									 JInteger* value, const JSize base = 10);
	static JBoolean	ConvertToUInt(const JCharacter16* str, const JSize length,
								  JUInt* value, const JSize base = 10);
	static JBoolean	IsHexValue(const JCharacter16* str, const JSize length);
	static JBoolean	CompleteConversion(const JCharacter16* startPtr, const JSize length,
									   const JCharacter16* convEndPtr);
};


// declarations of global functions for dealing with strings

int JString16Compare(const JCharacter16* s1, const JCharacter16* s2,
				   const JBoolean caseSensitive = kJTrue);
int JString16Compare(const JCharacter16* s1, const JSize length1,
				   const JCharacter16* s2, const JSize length2,
				   const JBoolean caseSensitive = kJTrue);

JBoolean	JCompareMaxN(const JCharacter16* s1, const JCharacter16* s2, const JSize N,
						 const JBoolean caseSensitive = kJTrue);
JBoolean	JCompareMaxN(const JCharacter16* s1, const JSize length1,
						 const JCharacter16* s2, const JSize length2,
						 const JSize N, const JBoolean caseSensitive = kJTrue);

JSize		JCalcMatchLength(const JCharacter16* s1, const JCharacter16* s2,
							 const JBoolean caseSensitive = kJTrue);

JBoolean	JCopyMaxN(const JCharacter16* source, const JIndex maxBytes,
					  JCharacter16* destination);

JBoolean	JMatchCase(const JCharacter16* source, const JIndexRange& sourceRange,
					   JCharacter16* dest, const JIndexRange& destRange);

JBoolean	JIsPrint(const JCharacter16 c);
JBoolean	JIsAlnum(const JCharacter16 c);
JBoolean	JIsAlpha(const JCharacter16 c);
JBoolean	JIsUpper(const JCharacter16 c);
JBoolean	JIsLower(const JCharacter16 c);

JCharacter16	JToUpper(const JCharacter16 c);
JCharacter16	JToLower(const JCharacter16 c);

JBoolean	JGetDiacriticalMap(const JCharacter16** map, const JIndex** markType);
void		JSetDiacriticalMap(const JCharacter16*  map, const JIndex*  markType);


/******************************************************************************
 Cast to JCharacter16*

 ******************************************************************************/

inline
JString16::operator const JCharacter16*()
	const
{
	return itsString;
}

/******************************************************************************
 ContainsNULL

 ******************************************************************************/

inline JBoolean
JString16::ContainsNULL()
	const
{
	return JConvertToBoolean( itsStringLength != strlen16(itsString) );
}

/******************************************************************************
 GetCString

 ******************************************************************************/

inline const JCharacter16*
JString16::GetCString()
	const
{
	return itsString;
}

/******************************************************************************
 IsEmpty

 ******************************************************************************/

inline JBoolean
JString16::IsEmpty()
	const
{
	return JConvertToBoolean( itsStringLength == 0 );
}

/******************************************************************************
 JString16Empty

 ******************************************************************************/

inline JBoolean
JString16Empty
	(
	const JCharacter16* s
	)
{
	return JConvertToBoolean( s == NULL || s[0] == '\0' );
}

/******************************************************************************
 GetLength

 ******************************************************************************/

inline JSize
JString16::GetLength()
	const
{
	return itsStringLength;
}

/******************************************************************************
 Convert to number

 ******************************************************************************/

inline JBoolean
JString16::ConvertToFloat
	(
	JFloat* value
	)
	const
{
	return JString16::ConvertToFloat(itsString, itsStringLength, value);
}

inline JBoolean
JString16::ConvertToInteger
	(
	JInteger*	value,
	const JSize	origBase
	)
	const
{
	return JString16::ConvertToInteger(itsString, itsStringLength, value, origBase);
}

inline JBoolean
JString16::ConvertToUInt
	(
	JUInt*		value,
	const JSize	origBase
	)
	const
{
	return JString16::ConvertToUInt(itsString, itsStringLength, value, origBase);
}

// static

inline JBoolean
JString16::ConvertToFloat
	(
	const JCharacter16*	str,
	JFloat*				value
	)
{
	return JString16::ConvertToFloat(str, strlen16(str), value);
}

inline JBoolean
JString16::ConvertToInteger
	(
	const JCharacter16*	str,
	JInteger*			value,
	const JSize			origBase
	)
{
	return JString16::ConvertToInteger(str, strlen16(str), value, origBase);
}

inline JBoolean
JString16::ConvertToUInt
	(
	const JCharacter16*	str,
	JUInt*				value,
	const JSize			origBase
	)
{
	return JString16::ConvertToUInt(str, strlen16(str), value, origBase);
}

/******************************************************************************
 Is number

	Returns kJTrue if we can convert ourselves to a number.

 ******************************************************************************/

inline JBoolean
JString16::IsFloat()
	const
{
	JFloat value;
	return ConvertToFloat(&value);
}

inline JBoolean
JString16::IsInteger
	(
	const JSize base
	)
	const
{
	JInteger value;
	return ConvertToInteger(&value, base);
}

inline JBoolean
JString16::IsUInt
	(
	const JSize base
	)
	const
{
	JUInt value;
	return ConvertToUInt(&value, base);
}

inline JBoolean
JString16::IsHexValue()
	const
{
	return IsHexValue(itsString, itsStringLength);
}

// static

inline JBoolean
JString16::IsFloat
	(
	const JCharacter16* str
	)
{
	JFloat value;
	return ConvertToFloat(str, &value);
}

inline JBoolean
JString16::IsInteger
	(
	const JCharacter16*	str,
	const JSize			base
	)
{
	JInteger value;
	return ConvertToInteger(str, &value, base);
}

inline JBoolean
JString16::IsUInt
	(
	const JCharacter16*	str,
	const JSize			base
	)
{
	JUInt value;
	return ConvertToUInt(str, &value, base);
}

inline JBoolean
JString16::IsHexValue
	(
	const JCharacter16* str
	)
{
	return JString16::IsHexValue(str, strlen16(str));
}

/******************************************************************************
 GetFirstCharacter

 ******************************************************************************/

inline JCharacter16
JString16::GetFirstCharacter()
	const
{
	return GetCharacter(1);
}

/******************************************************************************
 GetLastCharacter

 ******************************************************************************/

inline JCharacter16
JString16::GetLastCharacter()
	const
{
	return GetCharacter(itsStringLength);
}

/******************************************************************************
 LocateSubstring

 ******************************************************************************/

inline JBoolean
JString16::LocateSubstring
	(
	const JString16&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateSubstring
	(
	const JCharacter16*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, strlen16(str), kJTrue, startIndex);
}

inline JBoolean
JString16::LocateSubstring
	(
	const JCharacter16*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateSubstring
	(
	const JString16&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString16::LocateSubstring
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateSubstring(str, strlen16(str), caseSensitive, startIndex);
}

/******************************************************************************
 LocateNextSubstring

 ******************************************************************************/

inline JBoolean
JString16::LocateNextSubstring
	(
	const JString16&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateNextSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateNextSubstring
	(
	const JCharacter16*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, strlen16(str), kJTrue, startIndex);
}

inline JBoolean
JString16::LocateNextSubstring
	(
	const JCharacter16*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateNextSubstring
	(
	const JString16&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateNextSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString16::LocateNextSubstring
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateNextSubstring(str, strlen16(str), caseSensitive, startIndex);
}

/******************************************************************************
 LocatePrevSubstring

 ******************************************************************************/

inline JBoolean
JString16::LocatePrevSubstring
	(
	const JString16&	str,
	JIndex*			startIndex
	)
	const
{
	return LocatePrevSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString16::LocatePrevSubstring
	(
	const JCharacter16*	str,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, strlen16(str), kJTrue, startIndex);
}

inline JBoolean
JString16::LocatePrevSubstring
	(
	const JCharacter16*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString16::LocatePrevSubstring
	(
	const JString16&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocatePrevSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString16::LocatePrevSubstring
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocatePrevSubstring(str, strlen16(str), caseSensitive, startIndex);
}

/******************************************************************************
 LocateLastSubstring

 ******************************************************************************/

inline JBoolean
JString16::LocateLastSubstring
	(
	const JString16&	str,
	JIndex*			startIndex
	)
	const
{
	return LocateLastSubstring(str.itsString, str.itsStringLength, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateLastSubstring
	(
	const JCharacter16*	str,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, strlen16(str), kJTrue, startIndex);
}

inline JBoolean
JString16::LocateLastSubstring
	(
	const JCharacter16*	str,
	const JSize			length,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, length, kJTrue, startIndex);
}

inline JBoolean
JString16::LocateLastSubstring
	(
	const JString16&	str,
	const JBoolean	caseSensitive,
	JIndex*			startIndex
	)
	const
{
	return LocateLastSubstring(str.itsString, str.itsStringLength, caseSensitive, startIndex);
}

inline JBoolean
JString16::LocateLastSubstring
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive,
	JIndex*				startIndex
	)
	const
{
	return LocateLastSubstring(str, strlen16(str), caseSensitive, startIndex);
}

/******************************************************************************
 Contains

 ******************************************************************************/

inline JBoolean
JString16::Contains
	(
	const JString16&	str,
	const JBoolean	caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, caseSensitive, &i);
}

inline JBoolean
JString16::Contains
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, caseSensitive, &i);
}

inline JBoolean
JString16::Contains
	(
	const JCharacter16*	str,
	const JSize			length,
	const JBoolean		caseSensitive
	)
	const
{
	JIndex i;
	return LocateSubstring(str, length, caseSensitive, &i);
}

/******************************************************************************
 BeginsWith

 ******************************************************************************/

inline JBoolean
JString16::BeginsWith
	(
	const JString16&	str,
	const JBoolean	caseSensitive
	)
	const
{
	return BeginsWith(str, str.itsStringLength, caseSensitive);
}

inline JBoolean
JString16::BeginsWith
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return BeginsWith(str, strlen16(str), caseSensitive);
}

/******************************************************************************
 EndsWith

 ******************************************************************************/

inline JBoolean
JString16::EndsWith
	(
	const JString16&	str,
	const JBoolean	caseSensitive
	)
	const
{
	return EndsWith(str, str.itsStringLength, caseSensitive);
}

inline JBoolean
JString16::EndsWith
	(
	const JCharacter16*	str,
	const JBoolean		caseSensitive
	)
	const
{
	return EndsWith(str, strlen16(str), caseSensitive);
}

/******************************************************************************
 Concatenation

 ******************************************************************************/

inline JString16&
JString16::operator+=
	(
	const JString16& str
	)
{
	InsertSubstring(str.itsString, str.itsStringLength, itsStringLength+1);
	return *this;
}

inline JString16&
JString16::operator+=
	(
	const JCharacter16* str
	)
{
	InsertSubstring(str, strlen16(str), itsStringLength+1);
	return *this;
}

/******************************************************************************
 InsertSubstring

 ******************************************************************************/

inline void
JString16::InsertSubstring
	(
	const JString16&	str,
	const JIndex	insertionIndex
	)
{
	InsertSubstring(str.itsString, str.itsStringLength, insertionIndex);
}

inline void
JString16::InsertSubstring
	(
	const JCharacter16*	str,
	const JIndex		insertionIndex
	)
{
	InsertSubstring(str, strlen16(str), insertionIndex);
}

inline void
JString16::InsertCharacter
	(
	const JCharacter16	c,
	const JIndex		insertionIndex
	)
{
	JCharacter16 str[] = { c, '\0' };
	InsertSubstring(str, 1, insertionIndex);
}

/******************************************************************************
 Prepend

 ******************************************************************************/

inline void
JString16::Prepend
	(
	const JString16& str
	)
{
	InsertSubstring(str.itsString, str.itsStringLength, 1);
}

inline void
JString16::Prepend
	(
	const JCharacter16* str
	)
{
	InsertSubstring(str, strlen16(str), 1);
}

inline void
JString16::Prepend
	(
	const JCharacter16*	str,
	const JSize			length
	)
{
	InsertSubstring(str, length, 1);
}

inline void
JString16::PrependCharacter
	(
	const JCharacter16 c
	)
{
	JCharacter16 str[] = { c, '\0' };
	InsertSubstring(str, 1, 1);
}

/******************************************************************************
 Append

 ******************************************************************************/

inline void
JString16::Append
	(
	const JString16& str
	)
{
	InsertSubstring(str.itsString, str.itsStringLength, itsStringLength+1);
}

inline void
JString16::Append
	(
	const JCharacter16* str
	)
{
	InsertSubstring(str, strlen16(str), itsStringLength+1);
}

inline void
JString16::Append
	(
	const JCharacter16*	str,
	const JSize			length
	)
{
	InsertSubstring(str, length, itsStringLength+1);
}

inline void
JString16::AppendCharacter
	(
	const JCharacter16 c
	)
{
	JCharacter16 str[] = { c, '\0' };
	InsertSubstring(str, 1, itsStringLength+1);
}

/******************************************************************************
 ReplaceSubstring

 ******************************************************************************/

inline void
JString16::ReplaceSubstring
	(
	const JIndex	firstCharIndex,
	const JIndex	lastCharIndex,
	const JString16&	str
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, str.itsString, str.itsStringLength);
}

inline void
JString16::ReplaceSubstring
	(
	const JIndex		firstCharIndex,
	const JIndex		lastCharIndex,
	const JCharacter16*	str
	)
{
	ReplaceSubstring(firstCharIndex, lastCharIndex, str, strlen16(str));
}

inline void
JString16::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JCharacter16*	str,
	const JSize			length,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range.first, range.last, str, length);
	if (newRange != NULL)
		{
		newRange->SetFirstAndLength(range.first, length);
		}
}

inline void
JString16::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JString16&		str,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range, str.itsString, str.itsStringLength, newRange);
}

inline void
JString16::ReplaceSubstring
	(
	const JIndexRange&	range,
	const JCharacter16*	str,
	JIndexRange*		newRange
	)
{
	ReplaceSubstring(range, str, strlen16(str), newRange);
}

/******************************************************************************
 RemoveSubstring

 ******************************************************************************/

inline void
JString16::RemoveSubstring
	(
	const JIndex firstCharIndex,
	const JIndex lastCharIndex
	)
{
	JString16 temp;
	ReplaceSubstring(firstCharIndex, lastCharIndex, temp);
}

inline void
JString16::RemoveSubstring
	(
	const JIndexRange& range
	)
{
	if (!range.IsEmpty())
		{
		JString16 temp;
		ReplaceSubstring(range.first, range.last, temp);
		}
}

/******************************************************************************
 MatchCase

 ******************************************************************************/

inline JBoolean
JString16::MatchCase
	(
	const JString16&		source,
	const JIndexRange&	sourceRange
	)
{
	return MatchCase(source.itsString, sourceRange, JIndexRange(1, itsStringLength));
}

inline JBoolean
JString16::MatchCase
	(
	const JCharacter16*	source,
	const JIndexRange&	sourceRange
	)
{
	return MatchCase(source, sourceRange, JIndexRange(1, itsStringLength));
}

inline JBoolean
JString16::MatchCase
	(
	const JString16&		source,
	const JIndexRange&	sourceRange,
	const JIndexRange&	destRange
	)
{
	return MatchCase(source.itsString, sourceRange, destRange);
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline JBoolean
JString16::IndexValid
	(
	const JIndex index
	)
	const
{
	return JI2B( 1 <= index && index <= itsStringLength );
}

/******************************************************************************
 RangeValid

 ******************************************************************************/

inline JBoolean
JString16::RangeValid
	(
	const JIndexRange& range
	)
	const
{
	return JI2B(IndexValid(range.first) &&
				(range.IsEmpty() || IndexValid(range.last)));
}

/******************************************************************************
 Block size

 ******************************************************************************/

inline JSize
JString16::GetBlockSize()
	const
{
	return itsBlockSize;
}

inline void
JString16::SetBlockSize
	(
	const JSize blockSize
	)
{
	itsBlockSize = blockSize;
}

/******************************************************************************
 Addition

	We provide enough types so no constructors are called.

 ******************************************************************************/

inline JString16
operator+
	(
	const JString16& s1,
	const JString16& s2
	)
{
	JString16 sum = s1;
	sum += s2;
	return sum;
}

inline JString16
operator+
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	JString16 sum = s;
	sum += str;
	return sum;
}

inline JString16
operator+
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	JString16 sum = str;
	sum += s;
	return sum;
}

/******************************************************************************
 Equality (case-sensitive)

	We provide three types so no constructors are called.
	We don't need access to the private data because we have a conversion operator.

	*** We have to use the version of JString16Compare() that takes lengths
		because JString16s can contain NULLs.

 ******************************************************************************/

// operator==

inline int
operator==
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJTrue) == 0);
}

inline int
operator==
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJTrue) == 0);
}

inline int
operator==
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJTrue) == 0);
}

// operator!=

inline int
operator!=
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJTrue) != 0);
}

inline int
operator!=
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJTrue) != 0);
}

inline int
operator!=
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJTrue) != 0);
}

/******************************************************************************
 Comparison (case-insensitive)

	We provide three types so no constructors are called.

	*** We have to use the version of JString16Compare() that takes lengths
		because JString16s can contain NULLs.

 ******************************************************************************/

// operator<

inline int
operator<
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) < 0);
}

inline int
operator<
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) < 0);
}

inline int
operator<
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) < 0);
}

// operator<=

inline int
operator<=
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) <= 0);
}

inline int
operator<=
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) <= 0);
}

// operator>

inline int
operator>
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) > 0);
}

inline int
operator>
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) > 0);
}

inline int
operator>
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) > 0);
}

// operator>=

inline int
operator>=
	(
	const JString16& s1,
	const JString16& s2
	)
{
	return (JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JString16&		s,
	const JCharacter16*	str
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) >= 0);
}

inline int
operator>=
	(
	const JCharacter16*	str,
	const JString16&		s
	)
{
	return (JString16Compare(s, s.GetLength(), str, strlen16(str), kJFalse) >= 0);
}

/******************************************************************************
 JString16Compare

	*** We have to use the version of JString16Compare() that takes lengths
		because JString16s can contain NULLs.

 ******************************************************************************/

inline int
JString16Compare
	(
	const JString16&	s1,
	const JString16&	s2,
	const JBoolean	caseSensitive
	)
{
	return JString16Compare(s1, s1.GetLength(), s2, s2.GetLength(), caseSensitive);
}

inline int
JString16Compare
	(
	const JString16&		s1,
	const JCharacter16*	s2,
	const JBoolean		caseSensitive
	)
{
	return JString16Compare(s1, s1.GetLength(), s2, strlen16(s2), caseSensitive);
}

inline int
JString16Compare
	(
	const JCharacter16*	s1,
	const JString16&		s2,
	const JBoolean		caseSensitive
	)
{
	return JString16Compare(s1, strlen16(s1), s2, s2.GetLength(), caseSensitive);
}

/******************************************************************************
 CopyToPrivateString (private)

	Copy the given string into our private string.

 ******************************************************************************/

inline void
JString16::CopyToPrivateString
	(
	const JCharacter16* str
	)
{
	CopyToPrivateString(str, strlen16(str));
}

/******************************************************************************
 PrivateGetCharacter (private)

	*** This routine does no bounds checking!

 ******************************************************************************/

inline JCharacter16
JString16::PrivateGetCharacter
	(
	const JIndex index
	)
	const
{
	return itsString [ index - 1 ];
}

/******************************************************************************
 GetCharacterPtr (private)

	*** This routine does no bounds checking!

	Return a pointer to the character at the specified index.

 ******************************************************************************/

inline JCharacter16*
JString16::GetCharacterPtr
	(
	const JIndex index
	)
	const
{
	return itsString + index - 1;
}

/******************************************************************************
 Set

 ******************************************************************************/

inline void
JString16::Set
	(
	const JString16& str
	)
{
	if (this->itsString != str.itsString)
		{
		CopyToPrivateString(str.itsString, str.itsStringLength);
		}
}

inline void
JString16::Set
	(
	const JCharacter16* str
	)
{
	if (this->itsString != str)
		{
		CopyToPrivateString(str);
		}
}

inline void
JString16::Set
	(
	const JCharacter16*	str,
	const JSize			length
	)
{
	if (this->itsString != str)
		{
		CopyToPrivateString(str, length);
		}
}

inline void
JString16::Set
	(
	const JCharacter16*	str,
	const JIndexRange&	range
	)
{
	if (this->itsString != str)
		{
		if (range.IsNothing())
			{
			Clear();
			}
		else
			{
			CopyToPrivateString(str + range.first-1, range.GetLength());
			}
		}
}

/******************************************************************************
 Assignment operator

	We do not copy itsBlockSize because we assume the client has set them
	appropriately.

 ******************************************************************************/

inline const JString16&
JString16::operator=
	(
	const JString16& source
	)
{
	Set(source);
	return *this;
}

inline const JString16&
JString16::operator=
	(
	const JCharacter16* str
	)
{
	Set(str);
	return *this;
}

#endif
