/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ViewShape class (views as symbols within another view).
 */

/*
  $Id$

  $Log$
  Revision 1.5  1999/01/04 15:33:34  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

 */

#ifndef VIEW_SHAPE_H
#define VIEW_SHAPE_H

#include "RectShape.h"


class FullMapping_ViewShape
: public ViewShape
{
  public :
  virtual int NumShapeAttrs();
  
  virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms, 
			  Coord &width, Coord &height);
  
  virtual void DrawGDataArray (WindowRep *win, void **gdataArray, 
			       int numSyms, TDataMap *map,
			       ViewGraph *view, int pixelSize,
			       int &recordsProcessed, Boolean timeoutAllowed);
};
#endif
