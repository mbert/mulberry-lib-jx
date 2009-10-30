// -*- C++ -*-
// config-irix6.5.x-sgic++.h,v 1.4 2005/03/22 11:10:26 jwillemsen Exp

// Use this file for IRIX 6.5.x

#ifndef ACE_CONFIG_IRIX65X_H
#define ACE_CONFIG_IRIX65X_H
#include /**/ "ace/pre.h"

// Include IRIX 6.[234] configuration
#include "ace/config-irix6.x-sgic++.h"

#undef ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION

// Irix 6.5 man pages show that they exist
#undef ACE_LACKS_CONDATTR_PSHARED
#undef ACE_LACKS_MUTEXATTR_PSHARED

#include /**/ "ace/post.h"
#endif /* ACE_CONFIG_IRIX65X_H */
