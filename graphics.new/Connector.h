/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:14:38  jussi
  Added CVS header.
*/

#ifndef Connector_h
#define Connector_h

#include "DeviseTypes.h"
#include "Color.h"
#include "ShapeID.h"
#include "Pattern.h"
#include "ConnectorShape.h"

const unsigned MAX_CONNECTOR_SHAPE_ATTRS = 10;

struct Connector {
public:
  Coord x1,y1,x2,y2;                    /* end points of the connector */
  Color color;
  Pattern pattern;
  ConnectorShapeID cShapeID;            /* ID of connector shape */
  int numShapeAttrs;                    /* # of attributes for shape */
  ShapeAttr shapeAttrs[MAX_CONNECTOR_SHAPE_ATTRS];
};

#endif
