#ifndef _ERROR_H
#define _ERROR_H

#include <sys/varargs.h>

#define FATAL 0
#define NON_FATAL 1

#define ERROR \
	ERRORLine = __LINE__; \
	ERRORFile = __FILE__; \
	ErrorProc

#ifdef __cplusplus
extern "C" {
#endif

extern int ERRORLine;
extern char *ERRORFile;

void ErrorProc(int, ...);
void GraceExit(int);

#ifdef __cplusplus
}
#endif

#endif /* !_ERROR_H */
