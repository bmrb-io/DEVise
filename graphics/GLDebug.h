#ifndef GLDEBUG_h
#define GLDEBUG_h
#include <assert.h>

#ifdef DEBUG
void __glcheckerror(char* a, int j);
#define GLCHECKERROR() __glcheckerror(__FILE__, __LINE__)
#else
#define GLCHECKERROR() void(0)
#endif

#if defined(DEBUG)
#define MAKECURRENT() printf("%s: %d MAKECURRENT()\n", __FILE__, __LINE__), makeCurrent()
static int __glbeginendlayercount=0;
#define GLBEGIN(a) assert((++__glbeginendlayercount)==1), glBegin(a)
#define GLEND() assert((--__glbeginendlayercount)==0), glEnd()
#define GLMATRIXMODE(a) assert(__glbeginendlayercount==0), glMatrixMode(a)
#else
#define MAKECURRENT() makeCurrent()
#define GLBEGIN(a) glBegin(a)
#define GLEND() glEnd()
#define GLMATRIXMODE(a) glMatrixMode(a)
#endif


#endif
