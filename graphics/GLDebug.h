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
  Revision 1.3  1998/04/13 22:24:47  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.2  1998/03/25 14:51:49  wenger
  Added standard headers to all graphics sources.

*/

#ifndef GLDEBUG_h
#define GLDEBUG_h
#include <assert.h>

void __glcheckerror(char *a, int j);

#ifdef DEBUGGL
#define GLCHECKERROR() __glcheckerror(__FILE__, __LINE__)
#else
#define GLCHECKERROR() void(0)
#endif

#if defined(DEBUGGL)
//#define MAKECURRENT() printf("%s: %d MAKECURRENT()\n", __FILE__, __LINE__), makeCurrent()
#define MAKECURRENT() makeCurrent()
static int __glbeginendlayercount=0;
#define glBegin(a) do { \
	__glcheckerror(__FILE__,__LINE__); \
	assert((++__glbeginendlayercount)==1); \
	glBegin(a); \
} while (0)
#define glEnd() do { \
	glEnd(); \
	assert((--__glbeginendlayercount)==0); \
	__glcheckerror(__FILE__,__LINE__); \
} while (0)
#else
#define MAKECURRENT() makeCurrent()
#endif

#ifdef DEBUGGL

#define glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap)\
do {glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glCallLists(n, type, lists)\
do {glCallLists(n, type, lists);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glClear(mask)\
do {glClear(mask);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glClearIndex(c)\
do {glClearIndex(c);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glDeleteLists(list, range)\
do {glDeleteLists(list, range);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glDepthFunc(func)\
do {glDepthFunc(func);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glDisable(cap)\
do {glDisable(cap);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glEnable(cap)\
do {glEnable(cap);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glFlush()\
do {glFlush();\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glGetDoublev(pname, params)\
do {glGetDoublev(pname, params);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glGetFloatv(pname, params)\
do {glGetFloatv(pname, params);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glGetIntegerv(pname, params)\
do {glGetIntegerv(pname, params);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glIndexi(c)\
do {glIndexi(c);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glLineWidth(width)\
do {glLineWidth(width);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glListBase(base)\
do {glListBase(base);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glLoadIdentity()\
do {glLoadIdentity();\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glLogicOp(opcode)\
do {glLogicOp(opcode);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glMatrixMode(mode)\
do {glMatrixMode(mode);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glOrtho(left, right, bottom, top, near_val, far_val)\
do {glOrtho(left, right, bottom, top, near_val, far_val);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glPolygonMode(face, mode)\
do {glPolygonMode(face, mode);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glPopMatrix()\
do {glPopMatrix();\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glPushMatrix()\
do {glPushMatrix();\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glRasterPos2i(x, y)\
do {glRasterPos2i(x, y);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glRasterPos3d(x, y, z)\
do {glRasterPos3d(x, y, z);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glRotatef(angle, x, y, z)\
do {glRotatef(angle, x, y, z);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glScaled(x, y, z)\
do {glScaled(x, y, z);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glScissor(x, y, width, height)\
do {glScissor(x, y, width, height);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glTranslated(x, y, z)\
do {glTranslated(x, y, z);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glTranslatef(x, y, z)\
do {glTranslatef(x, y, z);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#define glViewport(x, y, width, height)\
do {glViewport(x, y, width, height);\
__glcheckerror(__FILE__,__LINE__);} while (0)

#endif
#endif
