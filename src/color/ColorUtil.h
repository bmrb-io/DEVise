/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
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
 */

//******************************************************************************

#ifndef __CLASS_COLORUTIL
#define __CLASS_COLORUTIL

//******************************************************************************
// Libraries
//******************************************************************************

#include <string>
#include <stl.h>

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
