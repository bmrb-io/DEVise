/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
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
  Revision 1.10  1999/07/13 17:32:44  wenger
  Parent view can now control attribute(s) in child view's mapping;
  cleaned up some of the mapping-related code; better command logging.

  Revision 1.9  1999/06/22 18:30:56  wenger
  Visual filter values for view symbols can now be specified in the parent
  view's mapping.

  Revision 1.8  1999/05/28 16:32:48  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.7  1999/05/26 19:50:56  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.6  1999/05/20 15:17:56  wenger
  Fixed bugs 490 (problem destroying piled parent views) and 491 (problem
  with duplicate elimination and count mappings) exposed by Tim Wilson's
  two-station session.

  Revision 1.5  1999/01/04 15:33:34  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

 */

#ifndef VIEW_SHAPE_H
#define VIEW_SHAPE_H

#include "RectShape.h"


class PileStack;

class FullMapping_ViewShape
: public ViewShape
{
public:
  virtual int NumShapeAttrs();

  virtual Boolean BBIsVariable(GDataAttrOffset *offsets) {
    Boolean result = false;
    if (offsets->_sizeOffset >= 0 ||
        offsets->_shapeAttrOffset[1] >= 0 || 
        offsets->_shapeAttrOffset[2] >= 0) {
      result = true;
    }
    return result;
  }

  virtual void FindBoundingBoxes(void *gdataArray, int numRecs,
            TDataMap *tdMap, Coord &maxWidth, Coord &maxHeight);

  virtual void DrawGDataArray (WindowRep *win, void **gdataArray, 
			       int numSyms, TDataMap *map,
			       ViewGraph *view, int pixelSize,
			       int &recordsProcessed, Boolean timeoutAllowed);
private:
  static void SetTData(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata, ViewGraph *viewsym);

  static PileStack *GetPile(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata);

  static void SetChildValue(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata, ViewGraph *viewsym);

  static void SetFilter(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata, ViewGraph *viewsym);

  static Boolean ShapeAttrToFilterVal(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata, int attrNum, Boolean isDate,
      Coord &value);

  static void SetTitle(TDataMap *map, AttrList *attrList,
      StringStorage *stringTable, char *gdata, ViewGraph *viewsym);

  static void CheckPile(ViewGraph *viewsym, PileStack *&ps, int pixX, int pixY,
      unsigned pixWd, unsigned pixHt);
};
#endif
