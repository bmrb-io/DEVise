/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  This file is just a means of grabbing the right header files for the
  damn varargs stuff according to what architecture we're on.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/08/21 22:17:53  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.1  1998/07/29 14:21:07  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

 */

#ifndef _devise_varargs_h_
#define _devise_varargs_h_

#if defined(OSF)
#   include <sys/param.h>
#   include <stdarg.h>
#elif defined(SGI) || defined(LINUX) || defined(SOLARIS)
#   include <stdarg.h>
#elif defined(SUN)
#   include <stdarg.h>
#else
#   include <sys/varargs.h>
#endif

#endif // _devise_varargs_h_

/*============================================================================*/
