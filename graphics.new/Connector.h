/*
  $Id$

  $Log$*/

#ifndef Connector_h
#define Connector_h
#include "DeviseTypes.h"
#include "Color.h"
#include "ShapeID.h"
#include "Pattern.h"
#include "ConnectorShape.h"

const unsigned MAX_CONNECTOR_SHAPE_ATTRS=10;
struct Connector {
public:
	Coord x1,y1,x2,y2; /* end points of the connector */
	Color color;
	Pattern pattern;
	ConnectorShapeID cShapeID; /* ID of connector shape */
	int numShapeAttrs;	/* # of attributes for shape */
	ShapeAttr shapeAttrs[MAX_CONNECTOR_SHAPE_ATTRS];
};

#endif
