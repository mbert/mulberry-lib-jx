// -*- C++ -*-

//=============================================================================
/**
 *  @file    Asynch_Pseudo_Task.h
 *
 *  Asynch_Pseudo_Task.h,v 1.6 2005/02/28 22:11:23 shuston Exp
 *
 *  @author Alexander Libman <alibman@ihug.com.au>
 */
//=============================================================================

#ifndef ACE_ASYNCH_PSEUDO_TASK_H
#define ACE_ASYNCH_PSEUDO_TASK_H

#include /**/ "ace/pre.h"

#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Task.h"


/**
 * @class ACE_Asynch_Pseudo_Task
 *
 */
class ACE_Export ACE_Asynch_Pseudo_Task : public ACE_Task<ACE_NULL_SYNCH>
{
public:
  ACE_Asynch_Pseudo_Task();
  virtual ~ACE_Asynch_Pseudo_Task();

  int start (void);
  int stop (void);

  int register_io_handler (ACE_HANDLE handle,
                           ACE_Event_Handler *handler,
                           ACE_Reactor_Mask mask,
                           int flg_suspend);

  int remove_io_handler (ACE_HANDLE handle);
  int remove_io_handler (ACE_Handle_Set &set);
  int resume_io_handler (ACE_HANDLE handle);
  int suspend_io_handler (ACE_HANDLE handle);

protected:
  virtual int svc (void);

  ACE_Select_Reactor select_reactor_;
  // should be initialized before reactor_
  ACE_Reactor reactor_;
};

#include /**/ "ace/post.h"

#endif /* ACE_ASYNCH_PSEUDO_TASK_H */
