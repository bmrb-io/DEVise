/*
  $Id$

  $Log$
  Revision 1.5  1996/06/21 19:29:11  jussi
  Cleaned up file and replaced MinMax class with simpler MIN/MAX macros.

  Revision 1.4  1996/05/20 18:44:40  jussi
  Replaced PENTIUM flag with SOLARIS.

  Revision 1.3  1996/04/15 19:32:05  wenger
  Consolidated the two (nearly identical) functions for
  reading/parsing physical schema, and cleaned up the
  ParseCat.c module quite a bit.  As part of this, I added
  a new enum DevStatus (StatusOk, StatusFailed, etc.).

  Revision 1.2  1995/09/05 21:12:39  jussi
  Added/update CVS header.
*/

#ifndef DeviseTypes_h
#define DeviseTypes_h

typedef double Coord;
typedef char Boolean;

#if defined(HPUX) || defined(SUN) || defined(SOLARIS) || defined(LINUX)
inline int trunc(float num) {
  return (int)num;
}
inline int trunc(double num) {
  return (int)num;
}
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MIN3
#define MIN3(a,b,c) ((a) < (b) ? MIN(a,c) : MIN(b,c))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAX3
#define MAX3(a,b,c) ((a) > (b) ? MAX(a,c) : MAX(b,c))
#endif

enum DevStatus {
  StatusInvalid = 0,
  StatusOk = 10000,
  StatusFailed,
  StatusWarn,
  StatusCancel,
  StatusWarnCancel
};

#endif
