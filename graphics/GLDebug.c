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
