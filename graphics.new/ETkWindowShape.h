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
  Revision 1.7  1999/05/26 19:50:49  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.6  1999/05/20 15:17:52  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.5  1997/11/18 23:26:50  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.4  1997/08/20 22:10:55  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

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

class ViewGraph;

class FullMapping_ETkWindowShape
: public ETkWindowShape
{
  public:
    
    virtual int NumShapeAttrs();

    virtual Boolean BBIsVariable(GDataAttrOffset *offsets) {
      Boolean result = false;
      if (offsets->_sizeOffset >= 0) {
        result = true;
      }
      return result;
    }
    
    virtual void FindBoundingBoxes(void *gdataArray, int numRecs,
            TDataMap *tdMap, Coord &maxWidth, Coord &maxHeight);
    
    virtual void DrawGDataArray(WindowRep *win, void **gdataArray,
				int numSyms, TDataMap *map,
				ViewGraph *view, int pixelSize,
				int &recordsProcessed, Boolean timeoutAllowed);
};

#endif
