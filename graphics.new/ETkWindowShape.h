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
  $Id$

  $Log$
  Revision 1.3.2.1  1997/08/07 16:56:30  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.3  1997/07/15 14:29:56  wenger
  Moved hashing of strings from TData*Interp classes to MappingInterp
  class; cleaned up a few extra includes of StringStorage.h.

  Revision 1.2  1997/03/28 16:09:44  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _ETKWINDOWSHAPE_H_
#define _ETKWINDOWSHAPE_H_

#include <iostream.h>
#include "RectShape.h"

class FullMapping_ETkWindowShape
: public ETkWindowShape
{
  public:
    
    virtual int NumShapeAttrs();
    
    virtual void MaxSymSize(TDataMap *map, void *gdata, int numSyms,
			    Coord &width, Coord &height);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed);
};

#endif
