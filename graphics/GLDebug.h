/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1998/03/25 14:51:49  wenger
  Added standard headers to all graphics sources.

 */

#ifndef GLDEBUG_h
#define GLDEBUG_h
#include <assert.h>

#ifdef DEBUGGL
void __glcheckerror(char* a, int j);
#define GLCHECKERROR() __glcheckerror(__FILE__, __LINE__)
#else
#define GLCHECKERROR() void(0)
#endif

#if defined(DEBUGGL)
//#define MAKECURRENT() printf("%s: %d MAKECURRENT()\n", __FILE__, __LINE__), makeCurrent()
#define MAKECURRENT() makeCurrent()
static int __glbeginendlayercount=0;
#define GLBEGIN(a) do { \
	__glcheckerror(__FILE__,__LINE__); \
	assert((++__glbeginendlayercount)==1); \
	glBegin(a); \
} while (0)
#define GLEND() do { \
	glEnd(); \
	assert((--__glbeginendlayercount)==0); \
	__glcheckerror(__FILE__,__LINE__); \
} while (0)
#define GLMATRIXMODE(a) do { \
	assert(__glbeginendlayercount==0); \
	glMatrixMode(a); \
	__glcheckerror(__FILE__,__LINE__); \
} while (0)
#else
#define MAKECURRENT() makeCurrent()
#define GLBEGIN(a) glBegin(a)
#define GLEND() glEnd()
#define GLMATRIXMODE(a) glMatrixMode(a)
#endif

#endif
