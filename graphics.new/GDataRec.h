#ifndef GDataRec_h
#define GDataRec_h
#include "Color.h"
#include "Pattern.h"
#include "ShapeID.h"

/* full gdata record */
const int MAX_GDATA_ATTRS = 10;
struct GDataRec {
	double x, y;
	Color color;
	double size;
	Pattern  pattern; 
	ShapeID shape; 
	double orientation;
	double shapeAttrs[MAX_GDATA_ATTRS];
};
#endif
