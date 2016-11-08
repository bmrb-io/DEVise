#ifndef IMAGE_INT_H
#define IMAGE_INT_H

#include "Image.h" 


extern int InnerImFatalError(Image *Im, char *s, int copy_reason); 
extern int InnerImNonFatalError(Image *Im, char *s, int copy_reason); 

#define ImFatalError(s) return(InnerImFatalError(Im, s, 1))
#define ImFatalErrorReasonSet(s) return(InnerImFatalError(Im, s, 0))

#define ImNonFatalError(s) return(InnerImNonFatalError(Im, s, 1))
#define ImNonFatalErrorReasonSet(s) return(InnerImNonFatalError(Im, s, 0))

extern void dirOf(char *s, char *ans); 
extern void fileOf(char *s, char *ans); 

#define IM_FREE_PTR(ptr) {if (ptr) free(ptr); (ptr) = 0;}

#endif /* IMAGE_INT_H */

