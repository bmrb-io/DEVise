/* ShapeID.h: definition of shape ID and shape attributes */
#ifndef ShapeID_H
#define ShapeID_H

typedef unsigned int ShapeID;

/* ID of predefined shapes */
const ShapeID RectShapeID = 0;
const ShapeID SquareShapeID= 1;
const ShapeID RectXShapeID = 2;
const ShapeID BarShapeID=3;
const ShapeID PixelRectShapeID = 4;
const ShapeID TriangleShapeID= 5;

typedef double ShapeAttr; /* Each attribute of the shape is a double, for now */

#endif
