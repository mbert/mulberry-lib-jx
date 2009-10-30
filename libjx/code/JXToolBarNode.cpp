/******************************************************************************
 JXToolBarNode.cpp

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 *****************************************************************************/

#include <JXStdInc.h>
#include "JXToolBarNode.h"

#include <JXTextMenu.h>

#include <JString.h>
#include <JTree.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

******************************************************************************/

JXToolBarNode::JXToolBarNode
	(
	JXTextMenu*	menu,
	const JIndex	index,
	const JBoolean	hasSeparator,
	const JBoolean	checked,
	JTree*			tree,
	JNamedTreeNode* parent,
	const char*	name
	)
	:
	JNamedTreeNode(tree, name, kJFalse),
	itsMenu(menu),
	itsIndex(index),
	itsHasSeparator(hasSeparator),
	itsIsChecked(checked)
{
	assert(parent != NULL);
	parent->Append(this);
}


/******************************************************************************
 Destructor

******************************************************************************/

JXToolBarNode::~JXToolBarNode()
{
}

/******************************************************************************
 ToggleChecked (public)

 ******************************************************************************/

void
JXToolBarNode::ToggleChecked()
{
	itsIsChecked = JNegate(itsIsChecked);

	JTree* tree;
	if (GetTree(&tree))
		{
		tree->JTree::BroadcastChange(this);
		}
}
