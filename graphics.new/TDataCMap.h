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
  $Id$

  $Log$
  Revision 1.5  1996/01/30 21:15:51  jussi
  Removed references to specific colors.

  Revision 1.4  1995/12/29 22:40:13  jussi
  Cleaned up the code a bit.

  Revision 1.3  1995/12/28 19:52:11  jussi
  Small fixes to remove compiler warnings. Added copyright notice.

  Revision 1.2  1995/09/05 22:15:53  jussi
  Added CVS header.
*/

#ifndef TDataCMap_h
#define TDataCMap_h

#include "Color.h"
#include "Pattern.h"
#include "ConnectorShape.h"
#include "Connector.h"
#include "GDataBin.h"
#include "Exit.h"

class Stream;
class Connector;

class TDataCMap {
public:
  TDataCMap() {
    defaultColor = ForegroundColor;
    defaultPattern = Pattern0;
    _shapeId = ContLineConnectorID;
    _numShapeAttrs = 0;
  }

  /* called by GDataBin to map 2 gdata records to one connection */
  virtual Boolean MapToConnection(void *grec1, void *grec2, Connector *c) {
    GDataBinRec *s1 = (GDataBinRec *)grec1;
    GDataBinRec *s2 = (GDataBinRec *)grec2;
    c->x1 = s1->x;
    c->y1 = s1->y;
    c->x2 = s2->x;
    c->y2 = s2->y;
    c->pattern = defaultPattern;
    c->color = defaultColor;
    c->cShapeID = _shapeId;
    c->numShapeAttrs = _numShapeAttrs;
    for(int i = 0; i < _numShapeAttrs; i++)
      c->shapeAttrs[i] = _shapeAttrs[i];
    return Map(grec1, grec2, c);
  }

  void SetDefaultColor(GlobalColor c) { defaultColor = c; }
  void SetDefaultPattern(Pattern p) { defaultPattern = p; }
  void SetDefaultConnectorShape(ConnectorShapeID shapeId,
				int numAttrs = 0,
				ShapeAttr *shapeAttr = NULL) {
    if (numAttrs > (int)MAX_CONNECTOR_SHAPE_ATTRS) {
      fprintf(stderr, "ConnectorMapping::SetDefaultShape:too many attrs %d\n",
	      numAttrs);
      Exit::DoExit(1);
    }
    _shapeId = shapeId;
    _numShapeAttrs = numAttrs;
    _shapeAttrs = shapeAttr;
  }
  GlobalColor GetDefaultColor() { return defaultColor; }
  Pattern GetDefaultPattern() { return defaultPattern; }

protected:
  virtual Boolean Map(void *, void *, Connector *) { return true; }

private:
  GlobalColor      defaultColor;
  Pattern          defaultPattern;
  ConnectorShapeID _shapeId;
  int              _numShapeAttrs;
  ShapeAttr        *_shapeAttrs;
};

#endif
