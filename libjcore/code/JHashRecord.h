#ifndef _H_JHashRecord
#define _H_JHashRecord

/******************************************************************************
 JHashRecord.h

	Interface for the JHashRecord class.

	For maximum space efficiency JMMRecord does NOT have a virtual destructor.
	It isn't intended to be a base class, so if you do subclass it be careful.

	Copyright � 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// Putting constants inside a template class sucks, so we use the same trick
// JOrderedSet does to keep the namespace clean
class JHashRecordT
{
public:
	enum State
	{
		kEmpty,
		kDeleted,
		kFull
	};
};


#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>


template <class V>
class JHashRecord
{
public:
	JHashRecord();
	JHashRecord(JHashValue hash, const V& value);
	~JHashRecord(); // Warning: not virtual!

	// Accept defaults
//	JHashRecord(const JHashRecord& source);
//	const JHashRecord& operator=(const JHashRecord& source);

// Get...
	JHashRecordT::State GetState() const;
	JBoolean   IsEmpty() const;
	JBoolean   IsDeleted() const;
	JBoolean   IsFull() const;
	JHashValue GetHashValue() const;
	const V&   GetValue() const;

// Set...
	void Set(const JHashValue hash, const V& value);
	void Set(const V& value);

	void Remove();
	void MarkEmpty();

//	void PrintLayout();

private:

	JHashRecordT::State itsState;
	JHashValue itsHashValue;
	V          itsValue;
};

/******************************************************************************
 GetState

 *****************************************************************************/

template <class V>
inline JHashRecordT::State
JHashRecord<V>::GetState() const
{
	return itsState;
}

/******************************************************************************
 IsEmpty

 *****************************************************************************/

template <class V>
inline JBoolean
JHashRecord<V>::IsEmpty() const
{
	return JConvertToBoolean(itsState == JHashRecordT::kEmpty);
}

/******************************************************************************
 IsDeleted

 *****************************************************************************/

template <class V>
inline JBoolean
JHashRecord<V>::IsDeleted() const
{
	return JConvertToBoolean(itsState == JHashRecordT::kDeleted);
}

/******************************************************************************
 IsFull

 *****************************************************************************/

template <class V>
inline JBoolean
JHashRecord<V>::IsFull() const
{
	return JConvertToBoolean(itsState == JHashRecordT::kFull);
}

/******************************************************************************
 GetHashValue

 *****************************************************************************/

template <class V>
inline JHashValue
JHashRecord<V>::GetHashValue() const
{
	return itsHashValue;
}

/******************************************************************************
 GetValue

 *****************************************************************************/

template <class V>
inline const V&
JHashRecord<V>::GetValue() const
{
	return itsValue;
}

/******************************************************************************
 Remove

	If the state is kFull, sets it to kDeleted (in other words, won't mark a
	kEmpty record kDeleted).

 *****************************************************************************/

template <class V>
inline void
JHashRecord<V>::Remove()
{
	if ( IsFull() )
		{
		itsState = JHashRecordT::kDeleted;
		}
}

/******************************************************************************
 MarkEmpty

	DANGER!  Extreme memory leak hazard!

	Don't use this unless you know what you're doing--this is the single easiest
	way to rip a hash table to shreds, and likely leak memory too.

 *****************************************************************************/

template <class V>
inline void
JHashRecord<V>::MarkEmpty()
{
	itsState = JHashRecordT::kEmpty;
}

#endif
