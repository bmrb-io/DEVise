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
  static DevStatus Reset(char *daliServer);
};


#endif /* _DaliIfc_h_ */

/*============================================================================*/
