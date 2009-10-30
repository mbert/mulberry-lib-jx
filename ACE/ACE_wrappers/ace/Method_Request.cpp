#include "ace/Method_Request.h"

ACE_RCSID (ace,
           Method_Request,
           "Method_Request.cpp,v 4.5 2002/04/10 17:44:15 ossama Exp")

ACE_Method_Request::ACE_Method_Request (unsigned long prio)
  : priority_ (prio)
{
}

ACE_Method_Request::~ACE_Method_Request (void)
{
}

unsigned long
ACE_Method_Request::priority (void) const
{
  return this->priority_;
}

void
ACE_Method_Request::priority (unsigned long prio)
{
  this->priority_ = prio;
}
