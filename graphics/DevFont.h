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
  Declaration of DevFont (Devise font) class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DevFont_h_
#define _DevFont_h_


#include <sys/types.h>

#include "DeviseTypes.h"


class WindowRep;


class DevFont {
public:
  DevFont();
  ~DevFont();

  void Set(int family, float pointSize, Boolean bold, Boolean italic);
  void Get(int &family, float &pointSize, Boolean &bold, Boolean &italic);

  void SetWinFont(WindowRep *win);

private:
  int _family;
  float _pointSize;
  Boolean _bold;
  Boolean _italic;
};


#endif /* _DevFont_h_ */

/*============================================================================*/
