/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001-2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
// Internal utilities for the color manager.
 */

/*
  $Id$

  $Log$
  Revision 1.1.10.1  2003/12/19 18:12:50  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.1.28.1  2003/12/17 00:18:40  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.1.26.2  2003/12/16 16:09:00  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.1.26.1  2003/04/18 16:11:00  wenger
  Got things to compile and link with gcc 3.2.2 (with lots of warnings),
  but some code is commented out; also may need fixes to be backwards-
  compatible with older gcc versions.

  Revision 1.1  2001/06/12 15:29:52  wenger
  Implemented a choice of modulus (default) or truncate color modes.

 */

//******************************************************************************

#ifndef __CLASS_COLORUTIL
#define __CLASS_COLORUTIL

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>

#include "Color.h"

//******************************************************************************
// Utility functions.
//******************************************************************************

inline int PColorID2Index(PColorID pcid, int size)
{
	int index;
	if (pcid == nullPColorID) {
	    index = 0;
	} else {
	    index = (int)pcid;
	}

    switch (GetColorMode()) {
	case ColorModeModulus:
		if (index < 0) index = -index;
		index %= size;
	    break;

	case ColorModeTruncate:
		if (index < 0) {
		    index = 0;
		} else if (index >= size) {
		    index = size - 1;
		}
	    break;

	case ColorModeInvalid:
	default:
		cerr << "Invalid color mode: " << (int)GetColorMode() << endl;
		index = 0;
	    break;
	}

	return index;
}

//******************************************************************************
#endif
