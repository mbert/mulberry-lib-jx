// -*- C++ -*-

//==========================================================================
/**
 *  @file    Pipe.h
 *
 *  Pipe.h,v 4.18 2005/06/16 18:17:48 schmidt Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//==========================================================================


#ifndef ACE_PIPE_H
#define ACE_PIPE_H

#include /**/ "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/Default_Constants.h"

/**
 * @class ACE_Pipe
 *
 * @brief Provides a portable bidirectional "pipe" abstraction.
 *
 * This class is designed to work with select()-based demuxers, such
 * as the ACE_Select_Reactor, which is why it uses sockets on Windows
 * rather than Win32 pipes (which aren't select()'able). 
 */
class ACE_Export ACE_Pipe
{
public:
  // = Initialization and termination.
  /// Default constructor (does nothing...).
  ACE_Pipe (void);

  /// Open the pipe and initialize the handles.
  ACE_Pipe (ACE_HANDLE handles[2]);

  /// Initialize the ACE_Pipe from the @a read and @a write handles.
  ACE_Pipe (ACE_HANDLE read, ACE_HANDLE write);

  /// Default dtor.  It doesn't close the handles for you.
  ~ACE_Pipe (void);

  /// Open the pipe and initialize the handles.
  int open (ACE_HANDLE handles[2]);

  /// Open the pipe, setting the buffer size to the maximum.
  int open (int buffer_size = ACE_DEFAULT_MAX_SOCKET_BUFSIZ);

  /// Close down the pipe HANDLEs;
  int close (void);

  // = Accessors.

  /**
   * This is the "read" side of the pipe.  Note, however, that
   * processes can also write to this handle as well since pipes are
   * bi-directional.
   */
  ACE_HANDLE read_handle (void) const;

  /**
   * This is the "write" side of the pipe.  Note, however, that
   * processes can also read to this handle as well since pipes are
   * bi-directional.
   */
  ACE_HANDLE write_handle (void) const;

  /// Dump the state of the object.
  void dump (void) const;

private:
  ACE_HANDLE handles_[2];
};

#if defined (__ACE_INLINE__)
#include "ace/Pipe.inl"
#endif /* __ACE_INLINE__ */

#include /**/ "ace/post.h"

#endif /* ACE_PIPE_H */
