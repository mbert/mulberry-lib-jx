/******************************************************************************
 JNamedTreeNode.cpp

	JTreeNode that includes a name.

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <JCoreStdInc.h>
#include <JNamedTreeNode.h>
#include <JTree.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	tree can be NULL.  By not providing a constructor that takes a parent,
	we allow tree to be NULL, and we allow JNamedTreeList to keep a sorted
	list of nodes.

 ******************************************************************************/

JNamedTreeNode::JNamedTreeNode
	(
	JTree*				tree,
	const JCharacter*	name,
	const JBoolean		isOpenable
	)
	:
	JTreeNode(tree, isOpenable),
	itsName(name)
{
	SetChildCompareFunction(DynamicCastCompareNames, JOrderedSetT::kSortAscending, kJFalse);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNamedTreeNode::~JNamedTreeNode()
{
}

/******************************************************************************
 SetName

 ******************************************************************************/

void
JNamedTreeNode::SetName
	(
	const JCharacter* name
	)
{
	if (name != itsName)
		{
		itsName = name;

		NameChanged();

		JTree* tree;
		if (GetTree(&tree))
			{
			tree->BroadcastChange(this);
			}
		}
}

/******************************************************************************
 NameChanged (virtual protected)

	Derived classes can override this to update themselves.  This provides
	a more efficient way than listening to itsTree (which may not exist!).

 ******************************************************************************/

void
JNamedTreeNode::NameChanged()
{
}

/******************************************************************************
 DynamicCastCompareNames (static protected)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::DynamicCastCompareNames
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNames(dynamic_cast(JNamedTreeNode*, e1),
						dynamic_cast(JNamedTreeNode*, e2));
}

/******************************************************************************
 CompareNames (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::CompareNames
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	JOrderedSetT::CompareResult result =
		JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		result = JCompareStringsCaseSensitive(&(e1->itsName), &(e2->itsName));
		}
	return result;
}

/******************************************************************************
 DynamicCastCompareNamesForIncrSearch (static protected)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::DynamicCastCompareNamesForIncrSearch
	(
	JTreeNode * const & e1,
	JTreeNode * const & e2
	)
{
	return CompareNamesForIncrSearch(dynamic_cast(JNamedTreeNode*, e1),
									 dynamic_cast(JNamedTreeNode*, e2));
}

/******************************************************************************
 CompareNamesForIncrSearch (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
JNamedTreeNode::CompareNamesForIncrSearch
	(
	JNamedTreeNode * const & e1,
	JNamedTreeNode * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsName), &(e2->itsName));
}
