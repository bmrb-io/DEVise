/* Shape.h */
#ifndef Shape_h
#define Shape_h
#include <stdio.h>
#include "DeviseTypes.h"

class WindowRep;
class TDataMap;

class Shape {
public:
	/* Return # of shape attributes needed for this shape */
	virtual int NumShapeAttrs() { return 0; }

	/* draw gdata. width == width of rectangle if size of
	gdata is smaller than 1 pixel. Used to enhance the drawing
	if the density of points is small.*/
	virtual void DrawGDataArray(WindowRep *, void **, int, TDataMap *,
		Coord xPerPixel, Coord yPerPixel, int width){};

	/* Find bounding box for gdata. By default, set both to 0.0  */
	virtual void BoundingBoxGData(TDataMap *, void **, int, Coord &width,
		Coord &height){ width = 0.0; height = 0.0; };

#ifdef OLD_CODE
	/* do pick inside */
	virtual Boolean Pick(Symbol *symbol,Coord xlow,Coord ylow,
		Coord xhigh, Coord yhigh, int button)=0;
#endif

};

#endif
