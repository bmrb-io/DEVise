/* ************************************************************************ */


/* Header file for the `xvertext 5.0' routines.

   Copyright (c) 1993 Alan Richardson (mppa3@uk.ac.sussex.syma) */


/* ************************************************************************ */

#ifndef _XVERTEXT_INCLUDED_ 
#define _XVERTEXT_INCLUDED_


#define XV_VERSION      5.0
#define XV_COPYRIGHT \
      "xvertext routines Copyright (c) 1993 Alan Richardson"


/* ---------------------------------------------------------------------- */


/* text alignment */

#define NONE             0
#define TLEFT            1
#define TCENTRE          2
#define TRIGHT           3
#define MLEFT            4
#define MCENTRE          5
#define MRIGHT           6
#define BLEFT            7
#define BCENTRE          8
#define BRIGHT           9


/* ---------------------------------------------------------------------- */

/* this shoulf be C++ compliant, thanks to 
     vlp@latina.inesc.pt (Vasco Lopes Paulo) */

/* Note: had to change floats to doubles in function declarations for this
 * to work right with g++.  RKW 13 June, 1997. */

#if defined(__cplusplus) || defined(c_plusplus)

extern "C" {
float   XRotVersion(char*, int);
void    XRotSetMagnification(double);
void    XRotSetBoundingBoxPad(int);
int     XRotDrawString(Display*, XFontStruct*, double,
                       Drawable, GC, int, int, char*);
int     XRotDrawImageString(Display*, XFontStruct*, double,
                            Drawable, GC, int, int, char*);
int     XRotDrawAlignedString(Display*, XFontStruct*, double,
                              Drawable, GC, int, int, char*, int);
int     XRotDrawAlignedImageString(Display*, XFontStruct*, double,
                                   Drawable, GC, int, int, char*, int);
XPoint *XRotTextExtents(Display*, XFontStruct*, double,
			int, int, char*, int);
}

#else

extern float   XRotVersion();
extern void    XRotSetMagnification();
extern void    XRotSetBoundingBoxPad();
extern int     XRotDrawString();
extern int     XRotDrawImageString();
extern int     XRotDrawAlignedString();
extern int     XRotDrawAlignedImageString();
extern XPoint *XRotTextExtents();

#endif /* __cplusplus */

/* ---------------------------------------------------------------------- */


#endif /* _XVERTEXT_INCLUDED_ */



