/* RectShape.h: a rectangle shape. */
#ifndef RectShape_h
#define RectShape_h
#include "DeviseTypes.h"
#include "Transform.h"
#include "Geom.h"
#include "WindowRep.h"
#include "Shape.h"
#include "Exit.h"

class RectShape: public Shape {

	/* Return the bounding box */
#ifdef OLD_CODE
	virtual void SetBoundingBox(Symbol *symbol){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		BoundingBox(symbol,width,height);
	}
	
	/* draw the rectangle */
	virtual void Draw(WindowRep *win,Symbol *symbol) {
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		DoDraw(win,symbol,width,height);
	}

	virtual Boolean Pick(Symbol *symbol, Coord xlow, Coord ylow,
		Coord xhigh, Coord yhigh, int button){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		return DoPick(symbol,width,height,xlow,ylow,xhigh, yhigh, button);
	}


protected:

	/* Set the bounding box of the rectangular symbol. 
	Width and height are passed in, not gotten from the rectangle.*/
	void BoundingBox(Symbol *symbol, Coord width, Coord height) {
		/* set up the transformer */

		if (symbol->orientation == 0.0 && symbol->size == 1.0){
			/* no rotation and scaling, easy case */
			Coord halfWidth = width/2;
			Coord halfHeight = height/2;
			symbol->xLow= symbol->x-halfWidth; 
			symbol->yLow = symbol->y - halfHeight; 
			symbol->width= width; 
			symbol->height =height; 
		}
		else {
			Coord x1,y1,x2,y2,x3,y3,x4,y4;
			Transform2D transform;
			UpdateTransform(symbol,transform);
			EndPoints(transform,symbol->x,symbol->y,
				width,height,x1,y1,x2,y2,x3,y3, x4,y4);
			symbol->xLow = MinMax::min(x1,x2,x3,x4);
			symbol->yLow = MinMax::min(y1,y2,y3,y4);
			symbol->width= MinMax::max(x1,x2,x3,x4)- symbol->xLow;
			symbol->height = MinMax::max(y1,y2,y3,y4)- symbol->yLow;
		};
	};

	/* do drawing of rectangle */
	void DoDraw(WindowRep *win,Symbol *symbol, Coord width, Coord height){
#ifdef DEBUG_RECT
		printf("Rect::Draw\n");
#endif

		win->SetFgColor(symbol->color);
		win->SetPattern(symbol->pattern);

		if (symbol->orientation == 0.0)
			win->FillRect(symbol->x-width/2.0, symbol->y-height/2.0,
				width,height);
		else {
			Transform2D transform;
			UpdateTransform(symbol,transform);
			Point points[4];
			EndPoints(transform,symbol->x,symbol->y,width,height,
					points[0].x, points[0].y,
					points[1].x, points[1].y,
					points[2].x, points[2].y,
					points[3].x, points[3].y);
#ifdef DEBUG_RECT
		printf("Rece:: now drawing rectangle (%f,%f),(%f,%f),(%f,%f),(%f,%f)\n",
				points[0].x,points[0].y,points[1].x,points[1].y,
				points[2].x,points[2].y, points[3].x, points[3].y);
#endif
			win->FillPoly(points,4);
		}
	}

	/* Return TRUE if picked inside the rectangle */
	Boolean DoPick(Symbol *symbol, Coord width, Coord height,
		Coord xlow, Coord ylow, Coord xhigh, Coord yhigh, int /*button*/){
		Coord halfWidth = width/2.0, halfHeight = height/2.0;
		if (symbol->orientation == 0.0 && symbol->size == 1.0 ){
			return Geom::RectRectIntersect(xlow,ylow,xhigh,yhigh,
				symbol->x-halfWidth,
				symbol->y-halfHeight, symbol->x+halfWidth,symbol->y+halfHeight);
		}
		else {
			fprintf(stderr,"Rect::Pick(): orientation <> 0.0\n");
			Exit::DoExit(1);
			/*
			Transform2D transform;
			UpdateTransform(symbol,transform);

			Coord tx,ty;
			transform.InverseTransform(x,y,tx,ty);
			return Geom::PointRectIntersect(tx,ty,symbol->x-halfWidth,
				symbol->y-halfHeight, symbol->x+halfWidth,symbol->y+halfHeight);
			*/
		}
	}

private:

	void UpdateTransform(Symbol *symbol,Transform2D &transform) {
		transform.MakeIdentity();
		transform.Translate(-symbol->x,-symbol->y);
		transform.Scale(symbol->size,symbol->size);
		transform.Rotate(symbol->orientation);
		transform.Translate(symbol->x,symbol->y);
	};

	/* figure out the 4 end points of the rectangle */
	void EndPoints(Transform2D &transform,
			Coord x,Coord y,Coord width,Coord height, 
			Coord &x1,Coord &y1,Coord &x2,Coord &y2,
			Coord &x3,Coord &y3,Coord &x4,Coord &y4){
		Coord halfWidth = width/2.0, halfHeight = height/2.0;
		transform.Transform(x-halfWidth,y-halfHeight,x1,y1);
		transform.Transform(x+halfWidth,y-halfHeight,x2,y2);
		transform.Transform(x+halfWidth,y+halfHeight,x3,y3);
		transform.Transform(x-halfWidth,y+halfHeight,x4,y4);
	};
#endif

};

class SquareShape: public RectShape {
#ifdef OLD_CODE
	/* Return the bounding box */
	virtual void SetBoundingBox(Symbol *symbol){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= width;
		RectShape::BoundingBox(symbol,width,height);
	}
	
	/* draw the rectangle */
	virtual void Draw(WindowRep *win,Symbol *symbol) {
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= width;
		RectShape::DoDraw(win,symbol,width,height);
	}

	virtual Boolean Pick(Symbol *symbol,Coord xlow, Coord ylow, 
		Coord xhigh, Coord yhigh, int button){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= width;
		return RectShape::DoPick(symbol,width,height,xlow,ylow,xhigh,
			yhigh,button);
	}
#endif
};

class PixelRectShape: public Shape {
public:
#ifdef OLD_CODE
	/* Return the bounding box */
	virtual void SetBoundingBox(Symbol *symbol){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		symbol->xLow = symbol->x;
		symbol->yLow = symbol->y;
		symbol->width = 0.0;
		symbol->height = 0.0;
	}
	
	/* draw the rectangle */
	virtual void Draw(WindowRep *win,Symbol *symbol) {
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		DoDraw(win,symbol,width,height);
	}

	virtual Boolean Pick(Symbol *symbol, Coord xlow, Coord ylow, 
		Coord xhigh, Coord yhigh, int button){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		return DoPick(symbol,width,height,xlow,ylow,xhigh,yhigh,button);
	}


protected:

	/* do drawing of rectangle */
	void DoDraw(WindowRep *win,Symbol *symbol, Coord width, Coord height){

#ifdef DEBUG_RECT
		printf("Rect::Draw\n");
#endif
		Transform2D transform;
		Coord tx,ty;
		win->Transform(symbol->x, symbol->y,tx,ty);
		UpdateTransform(tx,ty,symbol->size, symbol->orientation,transform);

		Point points[4];
		EndPoints(transform,tx,ty,width,height,
				points[0].x, points[0].y,
				points[1].x, points[1].y,
				points[2].x, points[2].y,
				points[3].x, points[3].y);
#ifdef DEBUG_RECT
		printf("Rect:: now drawing rectangle (%f,%f),(%f,%f),(%f,%f),(%f,%f)\n",
				points[0].x,points[0].y,points[1].x,points[1].y,
				points[2].x,points[2].y, points[3].x, points[3].y);
#endif
		win->SetFgColor(symbol->color);
		win->SetPattern(symbol->pattern);
		win->FillPixelPoly(points,4);
	}

	/* Return TRUE if picked inside the rectangle */
	Boolean DoPick(Symbol * /*symbol*/, Coord /* width*/, Coord /*height*/,
			Coord /*xlow*/, Coord /*ylow*/,Coord /*xhigh*/, Coord /*yhigh*/, 
			int /*button*/){
		return false;
	}

private:

	void UpdateTransform(Coord x, Coord y, Coord size,
		Coord orientation,Transform2D &transform) {
		transform.MakeIdentity();
		transform.Translate(-x,-y);
		transform.Scale(size,size);
		transform.Rotate(orientation);
		transform.Translate(x,y);
	};

	/* figure out the 4 end points of the rectangle */
	void EndPoints(Transform2D &transform,
			Coord x,Coord y,Coord width,Coord height, 
			Coord &x1,Coord &y1,Coord &x2,Coord &y2,
			Coord &x3,Coord &y3,Coord &x4,Coord &y4){
		Coord halfWidth = width/2.0, halfHeight = height/2.0;
		transform.Transform(x-halfWidth,y-halfHeight,x1,y1);
		transform.Transform(x+halfWidth,y-halfHeight,x2,y2);
		transform.Transform(x+halfWidth,y+halfHeight,x3,y3);
		transform.Transform(x-halfWidth,y+halfHeight,x4,y4);
	};
#endif

};

/* RectXShape: rect shape whose height always looks the same in
porportion width, despite zooming or scaling.
Note: orientation and scale not yet implemented. */
class RectXShape: public Shape {
public:
#ifdef OLD_CODE
	/* set bounding box of symbol */
	virtual void SetBoundingBox(Symbol *symbol){
		Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
		Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
		if (symbol->orientation != 0.0){
			fprintf(stderr,"RectXShape: orientation <> 0 not implemented\n");
			Exit::DoExit(1);
		}

		Coord halfWidth = width/2.0;
		Coord halfHeight = height/2.0;
		symbol->xLow = symbol->x - halfWidth;
		symbol->yLow = symbol->y - halfHeight;
		symbol->width = width;
		symbol->height = height;
	};
	virtual void SetArrayBoundingBox(Symbol **symbols, int numSyms,
		int startIndex = 0,  int incr = 1){
		int i;
		for (i= startIndex; i < numSyms; i += incr){
			Symbol *symbol = symbols[i];
			Coord width = (symbol->numShapeAttr >= 1 ? symbol->shapeAttrs[0]: 1.0);
			Coord height= (symbol->numShapeAttr >= 2 ? symbol->shapeAttrs[1]: 1.0);
			if (symbol->orientation != 0.0){
				fprintf(stderr,"RectXShape: orientation <> 0 not implemented\n");
				Exit::DoExit(1);
			}

			Coord halfWidth = width/2.0;
			Coord halfHeight = height/2.0;
			symbol->xLow = symbol->x - halfWidth;
			symbol->yLow = symbol->y - halfHeight;
			symbol->width = width;
			symbol->height = height;
		}
	};


	/* Draw the shape for the symbol */
	virtual void Draw(WindowRep *win, Symbol *symbol){
		/*
		printf("RectXShape: %f,%f\n", symbol->x, symbol->y);
		*/
#ifdef RECTX_PIXEL
		win->SetFgColor(symbol->color);
		win->DrawPixel(symbol->x, symbol->y);
#else
		Coord tx,ty;
		win->Transform(symbol->x,symbol->y,tx,ty);

		/* figure out # of pixels for width of symbol */
		Coord width;
		Coord x1,y1,x2,y2;
		win->Transform(0.0,0.0,x1,y1);
		win->Transform(symbol->width,0.0,x2,y2);
		width = x2-x1;
		if (width < 0.0) width = -width;
		Coord height = symbol->height/symbol->width*width;

		/*
		printf("RectX: %f,%f draw at %f,%f, %f, %f\n", symbol->x, 
			symbol->y,tx,ty, width,height);
		*/

		/* draw the shape */
		win->SetFgColor(symbol->color);
		win->SetPattern(symbol->pattern);
		win->FillPixelRect(tx,ty,width,height);
#endif
	};


	/* do pick inside */
	virtual Boolean Pick(Symbol *symbol,Coord xlow,Coord ylow,
		Coord xhigh, Coord yhigh, int /* button*/){
		return Geom::RectRectIntersect(xlow,ylow,xhigh,yhigh,
			symbol->xLow,symbol->yLow,symbol->xLow+symbol->width,
			symbol->yLow+symbol->height);
	};
	Coord _x[WINDOWREP_BATCH_SIZE];
	Coord _y[WINDOWREP_BATCH_SIZE];
#endif
};


/* BarShape: a shape drawn from (x,0) to (x,y), 0th attribute = width */
class BarShape: public Shape {
public:
#ifdef OLD_CODE
	/* set bounding box of symbol */
	virtual void SetBoundingBox(Symbol *symbol){
		Coord width;
		if (symbol->numShapeAttr ==0)
			width = 5.0;
		else width = symbol->shapeAttrs[0];

		symbol->xLow = symbol->x - width/2;
		symbol->width  = width;
		if (symbol->y > 0.0){
			symbol->yLow = 0.0;
			symbol->height = symbol->y;
		}
		else {
			symbol->yLow = symbol->y;
			symbol->height = -symbol->y;
		}
	};

	/* Draw the shape for the symbol */
	virtual void Draw(WindowRep *win, Symbol *symbol){
		win->SetFgColor(symbol->color);
		win->SetPattern(symbol->pattern);
		win->FillRect(symbol->xLow,symbol->yLow, symbol->width,symbol->height);
	};

	/* do pick inside */
	virtual Boolean Pick(Symbol *symbol,Coord xlow,Coord ylow,
		Coord xhigh, Coord yhigh, int /* button*/){
		return Geom::RectRectIntersect(xlow,ylow,xhigh,yhigh,
			symbol->xLow,symbol->yLow,
			symbol->xLow +symbol->width,symbol->yLow +symbol->height);
	};
#endif
};

#endif
