/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ViewGeom class (used for customizing view geometries).
 */

/*
  $Id$

  $Log$
 */

#ifndef _ViewGeom_h_
#define _ViewGeom_h_

#include <sys/types.h>

#include "DevStatus.h"
#include "DList.h"

class ViewWin;
class OldWinInfo;
DefinePtrDList(OldWinList, OldWinInfo *);

class OldWinInfo;

class ViewGeom {
public:
  static ViewGeom* GetViewGeom();

  ViewGeom();

  DevStatus Ungroup();
  DevStatus Group();

  Boolean IsGrouped() { return _isGrouped; }

private:
  DevStatus SplitWindow(ViewWin *window);
  void GetWindowArea(OldWinInfo *info, int &x, int &y, unsigned &w,
      unsigned &h);

  Boolean _isGrouped;
  OldWinList _oldWindows;
};

#endif // _ViewGeom_h_

/*============================================================================*/
