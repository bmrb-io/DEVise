/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DrawTimer class (timer for drawing symbols).  Used to
  break out and allow user control if it takes too long to draw a "chunk"
  of symbols.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1997/08/15 16:06:47  wenger
  Oops!  Forgot to cvs add these files earlier.

 */

#ifndef _DrawTimer_h_
#define _DrawTimer_h_

#include <sys/types.h>

#include "DeviseTypes.h"

class DrawTimer {
public:
  static void Set();
  static void Cancel();
  static Boolean Expired();

private:
  static Boolean _active;
  static double _expTime;
};


#endif /* _DrawTimer_h_ */

/*============================================================================*/
