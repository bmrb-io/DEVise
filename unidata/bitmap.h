// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Some standard bitmap macros
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef  _BITMAP_H
#define  _BITMAP_H

#include <limits.h>

#define    BIT(n)      (1 << (n))
#define  NOBIT(n)    (~(1 << (n)))

    // Assumes prototypes of (char *bm, int n)
#define  SET_BIT(bm,n)    (bm)[(n) / CHAR_BIT] |=   BIT((n) % CHAR_BIT)
#define  CLR_BIT(bm,n)    (bm)[(n) / CHAR_BIT] &= NOBIT((n) % CHAR_BIT)
#define  IS_SET(bm,n)     (bm)[(n) / CHAR_BIT] &    BIT((n) % CHAR_BIT)
#define  IS_CLR(bm,n)   (((bm)[(n) / CHAR_BIT] &    BIT((n) % CHAR_BIT)) == 0)

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif // _BITMAP_H
