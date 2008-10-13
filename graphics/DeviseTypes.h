/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
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
  Revision 1.19  2005/12/06 20:03:01  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.18.28.2  2003/04/18 17:07:41  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.18.28.1.2.1  2003/04/18 15:26:03  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.18.28.1  2003/04/17 17:59:17  wenger
  Now compiles with no warnings with gcc 2.95, except for warnings about
  tempname and tmpnam on Linux; updated Linux and Solaris dependencies.

  Revision 1.18  1998/02/26 17:19:13  wenger
  Fixed problems with yesterday's commit.

  Revision 1.17  1998/02/26 00:18:53  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.16  1997/12/23 23:34:54  liping
  The class Range was replaced by Interval

  Revision 1.15  1997/10/10 21:11:51  liping
  Adding the definition of "Range" which is used by TData and BufMgr, etc.

  Revision 1.14  1997/09/23 19:59:25  wenger
  Fix compile warning.

  Revision 1.13  1997/03/28 16:09:11  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.12  1996/12/18 18:52:01  wenger
  Devise requests Tasvir not to use subwindows for its images; sizing of
  Tasvir images now basically works like a RectX, except that the aspect
  ratio is fixed by the image itself; fixed a bug in action of +/- keys
  for RectX shapes; Devise won't request Tasvir images smaller than two
  pixels (causes error in Tasvir).

  Revision 1.11  1996/08/23 16:55:32  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.10  1996/08/05 19:51:36  wenger
  Fixed compile errors caused by some of Kevin's recent changes; changed
  the attrproj stuff to make a .a file instead of a .o; added some more
  TData file writing stuff; misc. cleanup.

  Revision 1.9  1996/08/05 17:28:58  beyer
  Added is_safe() which checks to see if a double is safe value (ie, not
  NaN or Infinity).  Made SetVisualFilter check the new filter for safety.

  Revision 1.8  1996/08/04 23:33:16  beyer
  Added #define for NULL

  Revision 1.7  1996/07/31 19:34:30  wenger
  Added AttrProj member functions for reading entire records (no projection).

  Revision 1.6  1996/07/13 04:59:31  jussi
  Added conditional for Linux.

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

#include <values.h>


typedef double Coord;
typedef char Boolean;

typedef struct
{
    const char 	*AttrName;
    Coord	Low;
    Coord	High;
    Coord	Granularity;
    int		NumRecs;

    Boolean has_left;           // there should be a meaningful left_adjacent
    Boolean has_right;          // there should be a meaningful right_adjacent

    Coord left_adjacent;        // the expected high value of the interval
                                // immediately to the left of this interval
    Coord right_adjacent;       // the expected low value of the interval
                                // immediately to the right of this interval

}Interval;

#include "MinMax.h"

#include "DevStatus.h"

#ifndef NULL
#define NULL 0
#endif


// Make sure the value isn't NaN, INFINITY, etc.
inline bool is_safe(double x)
{
  return x < MAXDOUBLE && x > -MAXDOUBLE;
}


#endif
