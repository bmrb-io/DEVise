/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Define MIN, MAX, and ABS macros.  Special care taken to define MIN and
  MAX in a way that doesn't cause compile warnings because of multiple
  definitions.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  2003/04/17 17:59:57  wenger
  Now compiles with no warnings with gcc 2.95, except for warnings about
  tempname and tmpnam on Linux; updated Linux and Solaris dependencies.

*/

#ifndef DeviseMinMax_h
#define DeviseMinMax_h

#if defined(LINUX) || defined(SGI)
  #ifdef MIN
    #undef MIN
  #endif
  #ifdef MAX
    #undef MAX
  #endif
  #include <sys/param.h>
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN3
#define MIN3(a,b,c) ((a) < (b) ? MIN(a,c) : MIN(b,c))
#endif

#ifndef MAX3
#define MAX3(a,b,c) ((a) > (b) ? MAX(a,c) : MAX(b,c))
#endif

#ifndef ABS
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#endif  //ABS

#endif
