/******************************************************************************
 JXSelectionData.cpp

	This defines the interface for all objects that encapsulate data
	placed in an X selection.  Each object is restricted to work on a
	single X display since the atom id's are different on each one.

	Remember to always provide access to the data so derived classes
	can extend your class by providing additional types.

	BASE CLASS = JBroadcaster

	Copyright � 1996-2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSelectionData.h>
#include <JXDisplay.h>
#include <JXWidget.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	The second form is used for delayed evaluation.  The id must be something
	unique to a particular *class* so each class in the inheritance line
	that implements GetSelectionData() can either do the work or pass it to
	its base class.  In most cases, the class name is sufficient.

 ******************************************************************************/

JXSelectionData::JXSelectionData
	(
	JXDisplay* display
	)
	:
	itsDisplay(display),
	itsDataSource(NULL)
{
	JXSelectionDataX();
}

JXSelectionData::JXSelectionData
	(
	JXWidget*			widget,
	const JCharacter*	id
	)
	:
	itsDisplay(widget->GetDisplay()),
	itsDataSource(widget)
{
	assert( widget != NULL && !JStringEmpty(id) );

	JXSelectionDataX();

	ListenTo(itsDataSource);	// need to know if it is deleted

	itsDataSourceID = new JString(id);
	assert( itsDataSourceID != NULL );
}

// private

void
JXSelectionData::JXSelectionDataX()
{
	itsSelectionName = None;
	itsStartTime     = CurrentTime;
	itsEndTime       = CurrentTime;

	itsTypeList = new JArray<Atom>;
	assert( itsTypeList != NULL );
	itsTypeList->SetCompareFunction(CompareAtoms);

	itsDataSourceID = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSelectionData::~JXSelectionData()
{
	delete itsTypeList;
	delete itsDataSourceID;
}

/******************************************************************************
 GetSelectionManager

 ******************************************************************************/

JXSelectionManager*
JXSelectionData::GetSelectionManager()
	const
{
	return itsDisplay->GetSelectionManager();
}

/******************************************************************************
 GetDNDManager

 ******************************************************************************/

JXDNDManager*
JXSelectionData::GetDNDManager()
	const
{
	return itsDisplay->GetDNDManager();
}

/******************************************************************************
 GetDNDSelectionName

 ******************************************************************************/

Atom
JXSelectionData::GetDNDSelectionName()
	const
{
	return (itsDisplay->GetDNDManager())->GetDNDSelectionName();
}

/******************************************************************************
 SetSelectionInfo

 ******************************************************************************/

void
JXSelectionData::SetSelectionInfo
	(
	const Atom	selectionName,
	const Time	startTime
	)
{
	assert( selectionName != None );

	itsSelectionName = selectionName;
	itsStartTime     = startTime;

	if (selectionName != GetDNDSelectionName())
		{
		JXSelectionManager* selMgr = GetSelectionManager();
		AddType(selMgr->GetTargetsXAtom());
		AddType(selMgr->GetTimeStampXAtom());
		}

	AddTypes(selectionName);
}

/******************************************************************************
 AddType (protected)

	Create the atom and add it to the list.

 ******************************************************************************/

Atom
JXSelectionData::AddType
	(
	const JCharacter* name
	)
{
	const Atom atom = itsDisplay->RegisterXAtom(name);
	AddType(atom);
	return atom;
}

/******************************************************************************
 RemoveType (protected)

	Remove the type from the list.

 ******************************************************************************/

void
JXSelectionData::RemoveType
	(
	const Atom type
	)
{
	JIndex index;
	if (itsTypeList->SearchSorted(type, JOrderedSetT::kAnyMatch, &index))
		{
		itsTypeList->RemoveElement(index);
		}
}

/******************************************************************************
 Resolve

	Asks its data source to set the data.  This is required for DND where
	the data should not be converted until it is needed, allowing the mouse
	drag to begin immediately.

	This cannot be called by JXSelectionManager because it doesn't know
	how the data is being used.

 ******************************************************************************/

void
JXSelectionData::Resolve()
	const
{
	if (itsDataSource != NULL && itsDataSourceID != NULL)
		{
		JXSelectionData* me = const_cast<JXSelectionData*>(this);

		itsDataSource->GetSelectionData(me, *itsDataSourceID);

		delete me->itsDataSourceID;
		me->itsDataSource   = NULL;
		me->itsDataSourceID = NULL;
		}
}

/******************************************************************************
 Convert

	Handles certain types and passes everything else off to ConvertData().

	When adding special types to this function, remember to update
	SetSelectionInfo() to add the new types.

 ******************************************************************************/

JBoolean
JXSelectionData::Convert
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	JXSelectionManager* selMgr = GetSelectionManager();

	// TARGETS

	if (requestType == selMgr->GetTargetsXAtom())
		{
		const JSize atomCount = itsTypeList->GetElementCount();
		assert( atomCount > 0 );

		*returnType   = XA_ATOM;
		*bitsPerBlock = sizeof(Atom)*8;
		*dataLength   = sizeof(Atom)*atomCount;

		*data = new unsigned char [ *dataLength ];
		if (*data == NULL)
			{
			return kJFalse;
			}

		Atom* atomData = reinterpret_cast<Atom*>(*data);
		for (JIndex i=1; i<=atomCount; i++)
			{
			atomData[i-1] = itsTypeList->GetElement(i);
			}

		return kJTrue;
		}

	// TIMESTAMP

	else if (requestType == selMgr->GetTimeStampXAtom())
		{
		*returnType   = XA_INTEGER;
		*bitsPerBlock = sizeof(Time)*8;
		*dataLength   = sizeof(Time);

		*data = new unsigned char [ *dataLength ];
		if (*data == NULL)
			{
			return kJFalse;
			}

		*(reinterpret_cast<Time*>(*data)) = itsStartTime;

		return kJTrue;
		}

	// everything else

	else
		{
		Resolve();
		return ConvertData(requestType, returnType,
						   data, dataLength, bitsPerBlock);
		}
}

/******************************************************************************
 AddTypes (virtual protected)

	Call AddType() for whatever types are appropriate for the given selection.

 ******************************************************************************/

/******************************************************************************
 ConvertData (virtual protected)

	Derived class must convert data to the specified type and return
	kJTrue, or return kJFalse if the conversion cannot be accomplished.

	*returnType must be actual data type.  For example, when "TEXT" is
	requested, one often returns XA_STRING.

	*data must be allocated with "new unsigned char[]" and will be deleted
	by the caller.  *dataLength must be set to the length of *data.

	*bitsPerBlock must be set to the number of bits per element of data.
	e.g.	If data is text, *bitsPerBlock=8.
			If data is an int, *bitsPerBlock=sizeof(int)*8

	Since X performs byte swapping when *bitsPerBlock > 8, mixed data should
	be packed one byte at a time to insure that it can be correctly decoded.

 ******************************************************************************/

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXSelectionData::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsDataSource)
		{
		delete itsDataSourceID;
		itsDataSource   = NULL;
		itsDataSourceID = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}

/******************************************************************************
 CompareAtoms (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXSelectionData::CompareAtoms
	(
	const Atom& atom1,
	const Atom& atom2
	)
{
	if (atom1 < atom2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (atom1 == atom2)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

#define JTemplateType JXSelectionData
#include <JPtrArray.tmpls>
#undef JTemplateType
