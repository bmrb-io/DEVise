/*
   $Id$

   $Log$*/

#ifndef BINCONV_H
#define BINCONV_H

#include <string.h>

#define SUN4

#ifdef SUN4	                // Sun 4 (Sparc based) machine
#   undef  SWAPBYTES		/* don't swap byte order on ints and reals */
#   define NEED_WORD_ALIGNED	/* data must be aligned on proper word     */
				/*   boundaries				   */
#   define CONVREALS		/* don't convert real (floating point)     */
				/*   numbers 				   */
#   undef  CONVEBCDIC		/* don't convert characters from ASCII     */
				/*   to EBCDIC				   */
#endif

#ifdef MIPS	                // MIPS machine (DECstation)
#   define SWAPBYTES
#   define NEED_WORD_ALIGNED
#   define CONVREALS
#   undef CONVEBCDIC
#endif

#ifdef SUN3	                // Sun 3 (68000 based) machine
#   undef SWAPBYTES
#   undef NEED_WORD_ALIGNED
#   undef CONVREALS
#   undef CONVEBCDIC
#endif

#ifdef VAX                      // VAX
#   define SWAPBYTES
#   undef NEED_WORD_ALIGNED
#   define CONVREALS
#   undef  CONVEBCDIC
#endif

#ifdef IBM370                   // IBM 370
#   undef  SWAPBYTES
#   undef NEED_WORD_ALIGNED
#   define CONVREALS
#   define CONVEBCDIC
#endif

class BinaryConversion {
public:
  static void text(char *var, int len) {
#ifdef CONVEBCDIC
    for(int i = 0; i < len; i++)
      ascii2ebcdic(var + i);
#endif
  }

  static void nullTerm(char *var, int len) {
    var[len - 1] = 0;
    for(int i = len - 2; i >= 0; i--) {
      if (var[i] != ' ')
	return;
      var[i] = 0;
    }
  }

  static void twobyte(char *var, int len) {
#ifdef SWAPBYTES
    for(int i = 0; i < len; i++)
      swap2(var + i * 2);
#endif
  }

  static void fourbyte(char *var, int len) {
#ifdef SWAPBYTES
    for(int i = 0; i < len; i++)
      swap4(var + i * 4);
#endif
  }

  static void real(char *var, int len) {
    fourbyte(var, len);
    for(int i = 0; i < len / 4; i++)
      if (*(int *)(var + i * 4) == 0x100000)
	*(float *)(var + i * 4) = 0.0; 
#if 0
#ifdef CONVREALS
    for(int i = 0; i < len; i++) {
      float *f = (float *)(var + i * 4);
      *f = convreal(*(int *)f);
    }
#endif
#endif
  }

  static void CRSPDate(int index, int &day, int &month, int &year);

protected:
  char ascii2ebcdic(char c) {
    return c;
  }

  void swap2(char *x) {
    char c = *x; *x = *(x + 1); *(x + 1) = c;
  }

  void swap4(char *x) {
    char c = *x; *x = *(x + 3); *(x + 3) = c;
    c = *(x + 1); *(x + 1) = *(x + 2); *(x + 2) = c;
  }

  float convreal(unsigned long num);    // convert IEEE FP number
};

#endif
