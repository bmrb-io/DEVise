/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DaliIfc class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1996/08/28 00:19:36  wenger
  Improved use of Dali to correctly free images (use of Dali is now fully
  functional with filenames in data).

  Revision 1.1  1996/08/23 16:55:29  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

 */

#ifndef _DaliIfc_h_
#define _DaliIfc_h_


#include "Xdef.h"
#include "DeviseTypes.h"


class DaliIfc
{
public:
  static DevStatus ShowImage(char *daliServer, Window win, int centerX,
    int centerY, int width, int height, char *filename, int imageLen,
    char *image, int &handle);
  static DevStatus FreeImage(char *daliServer, int handle);
  static DevStatus Reset(char *daliServer);
  static DevStatus Quit(char *daliServer);
};


#endif /* _DaliIfc_h_ */

/*============================================================================*/
