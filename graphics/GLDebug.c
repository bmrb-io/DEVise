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
 */

#include "GLDebug.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

void __glcheckerror(char* a, int j) {
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError())!= GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf(stderr, "%s : %d ", a, j);
		fprintf(stderr, "OpenGL Error detected: %s\n", errString);
	}
}
