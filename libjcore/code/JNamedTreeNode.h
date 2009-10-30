/******************************************************************************
 JNamedTreeNode.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNamedTreeNode
#define _H_JNamedTreeNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTreeNode.h>
#include <JString.h>

class JNamedTreeList;

class JNamedTreeNode : public JTreeNode
{
	friend class JNamedTreeList;

public:

	JNamedTreeNode(JTree* tree, const JCharacter* name,
				   const JBoolean isOpenable = kJTrue);

	virtual	~JNamedTreeNode();

	const JString&	GetName() const;
	void			SetName(const JCharacter* name);

	static JOrderedSetT::CompareResult
		CompareNames(JNamedTreeNode * const & e1, JNamedTreeNode * const & e2);
	static JOrderedSetT::CompareResult
		CompareNamesForIncrSearch(JNamedTreeNode * const & e1, JNamedTreeNode * const & e2);

protected:

	virtual void	NameChanged();

	static JOrderedSetT::CompareResult
		DynamicCastCompareNames(JTreeNode * const & e1, JTreeNode * const & e2);
	static JOrderedSetT::CompareResult
		DynamicCastCompareNamesForIncrSearch(JTreeNode * const & e1, JTreeNode * const & e2);

private:

	JString	itsName;

private:

	// not allowed

	JNamedTreeNode(const JNamedTreeNode& source);
	const JNamedTreeNode& operator=(const JNamedTreeNode& source);
};


/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
JNamedTreeNode::GetName()
	const
{
	return itsName;
}

#endif
