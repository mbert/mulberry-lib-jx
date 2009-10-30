// Container_Instantiations.cpp,v 4.1 2003/06/13 11:05:36 bala Exp
#include "ace/CDR_Base.h"
#include "ace/Unbounded_Queue.h"

// Instantiations of templates with basic data types that can be used
// across  ACE+TAO.
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Node<ACE_INT32>;
template class ACE_Unbounded_Queue<ACE_INT32>;
template class ACE_Unbounded_Queue_Iterator<ACE_INT32>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Node<ACE_INT32>;
#pragma instantiate ACE_Unbounded_Queue<ACE_INT32>;
#pragma instantiate ACE_Unbounded_Queue_Iterator<ACE_INT32>;

#endif /*ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION*/
