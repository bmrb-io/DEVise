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
  Dummy versions of OpenGL functions so link works on platforms that don't
  have OpenGL.

  Note that functions are in alphabetical order to make it easier to find
  things.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/05/08 14:02:38  wenger
  Got DEVise to compile on Linux again (with dummy OpenGL).

  Revision 1.1  1998/03/11 18:25:39  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.2  1998/01/08 19:44:00  wenger
  Updated copyright to 1998.

 */

#include <stdio.h>
#include <X11/Xlib.h>
#include "GL/gl.h"
#include "GL/glx.h"
#include "GL/glu.h"

extern "C" {

void
glBegin(GLenum mode)
{
}

void
glBitmap(GLsizei width, GLsizei height,
	 GLfloat xorig, GLfloat yorig,
	 GLfloat xmove, GLfloat ymove,
	 const GLubyte *bitmap)
{
}

void
glCallLists(GLsizei n, GLenum type, const GLvoid *lists)
{
}

void
glClear(GLbitfield mask)
{
}

void
glClearIndex(GLfloat c)
{
}

void
glDeleteLists(GLuint list, GLsizei range)
{
}

void
glDepthFunc(GLenum func)
{
}

void
glDisable(GLenum cap)
{
}

void
glDrawPixels( GLsizei width, GLsizei height, GLenum format,
    GLenum type, const GLvoid *pixels )
{
}

void
glEnable(GLenum cap)
{
}

void
glEnd(void)
{
}

void
glFlush(void)
{
}

GLuint
glGenLists(GLsizei range)
{
  return (GLuint) 0;
}

void
glGetDoublev(GLenum pname, GLdouble *params)
{
}

void
glGetFloatv(GLenum pname, GLfloat *params)
{
}

void
glGetIntegerv(GLenum pname, GLint *params)
{
}

void
glIndexi(GLint c)
{
}

void
glLineWidth(GLfloat width)
{
}

void
glListBase(GLuint base)
{
}

void
glLoadIdentity(void)
{
}

void
glLogicOp(GLenum opcode)
{
}

void
glMatrixMode(GLenum mode)
{
}

void
glOrtho(GLdouble left, GLdouble right,
        GLdouble bottom, GLdouble top,
        GLdouble near_val, GLdouble far_val)
{
}

void
glPolygonMode(GLenum face, GLenum mode)
{
}

void
glPopMatrix(void)
{
}

void
glPushMatrix(void)
{
}

void
glRasterPos2i(GLint x, GLint y)
{
}

void
glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
}

void
glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
    GLenum format, GLenum type, GLvoid *pixels)
{
}

void
glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
}

void
glScaled(GLdouble x, GLdouble y, GLdouble z)
{
}

void
glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

void
glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
}

void
glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
}

void
glVertex2d(GLdouble x, GLdouble y)
{
}

void
glVertex2f(GLfloat x, GLfloat y)
{
}

void
glVertex2i(GLint x, GLint y)
{
}

void
glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
}

void
glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

XVisualInfo*
glXChooseVisual(Display *dpy, int screen,
		int *attribList)
{
  fprintf(stderr, "\nDEVise is linked with a dummy OpenGL library.\n");
  fprintf(stderr, "Do not use the '-gl' command line option.\n\n");
  return NULL;
}

GLXContext
glXCreateContext(Display *dpy, XVisualInfo *vis,
		 GLXContext shareList, Bool direct)
{
  return NULL;
}

void
glXDestroyContext(Display *dpy, GLXContext ctx)
{
}

Bool
glXMakeCurrent(Display *dpy, GLXDrawable drawable,
	       GLXContext ctx)
{
  return (Bool) 0;
}

void
glXSwapBuffers(Display *dpy, GLXDrawable drawable)
{
}

void
glXUseXFont(Font font, int first, int count, int list)
{
}

void
glXWaitGL(void)
{
}

void
glXWaitX(void)
{
}

void
gluDeleteQuadric(GLUquadricObj *state)
{
}

GLUquadricObj *
gluNewQuadric(void)
{
  return NULL;
}

void
gluOrtho2D(GLdouble left, GLdouble right,
	   GLdouble bottom, GLdouble top)
{
}

GLint
gluProject( GLdouble objx, GLdouble objy, GLdouble objz,
            const GLdouble modelMatrix[16],
	    const GLdouble projMatrix[16],
	    const GLint viewport[4],
	    GLdouble *winx, GLdouble *winy, GLdouble *winz)
{
  return (GLint) 0;
}

void
gluQuadricDrawStyle(GLUquadricObj *quadObject,
		    GLenum drawStyle)
{
}

void
gluQuadricNormals(GLUquadricObj *quadObject, GLenum normals)
{
}

void
gluSphere(GLUquadricObj *qobj,
	  GLdouble radius, GLint slices, GLint stacks)
{
}

GLint
gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
	     const GLdouble modelMatrix[16],
	     const GLdouble projMatrix[16],
	     const GLint viewport[4],
	     GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
  return (GLint) 0;
}

}

/*============================================================================*/
