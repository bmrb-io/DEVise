/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   Drawable2D.h
PURPOSE: Defines abstract class for drawing points and lines on a 2D cartesian
	 surface 
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#ifndef Drawable2D_H
	#include "Real.h"

class Drawable2D
{
protected:
	/*Current background color*/
	short siBackgroundColor;

	/*Current pen color*/
	short siColor;

	/*Current cursor position along x dimension*/
	Real fX;

	/*Current cursor position along y dimension*/
	Real fY;

	/*Size of pixel map along x dimension*/
	int iNX;

	/*Size of pixel map along y dimension*/
	int iNY;

	/*Current integer pixel value along x dimension */
	int iX;

	/*Current integer pixel value along y dimension */
	int iY;

	/*Scaling factor along x dimension*/
	Real fScaleX;

	/*Scaling factor along y dimension*/
	Real fScaleY;

	/*Total number of points in pixel map*/
	int iNPoints;

protected:
	/*Draw a point at iPX, iPY using the current pen color.  This is
	  implemented by ALL derived classes */
	virtual void vDrawPoint(int iPX,int iPY)=0;

	/*Bressenham's algorithm for drawing a line from iPX1,iPY1
	  to iPX2,iPY2.  Uses vDrawPoint(iPX,iPY) to render. 
  	  Performs clipping at boundaries */
	virtual void vDrawLine(int iPX1,int iPY1,int iPX2,int iPY2);

	/*Returns TRUE/FALSE depending upon whether iPX,iPY lies within
	  the clipping boundaries */ 
	int bIsWithin(int iPX,int iPY);

public:
	/*Default constructor*/
	Drawable2D();

	/*Constructor with defined size*/
	Drawable2D(int i_NX,int i_NY);


	/*Get the current pen color*/
	short siGetPenColor()
		{ return siColor; }

	/*Set the current pen color */
	virtual void vSetPenColor(short siNewColor)
		{ siColor = siNewColor; }

	/*Get the current background pen color*/
	short siGetBackgroundPenColor()
		{ return siBackgroundColor; }

	/*Set the current pen background color */
	virtual void vSetBackgroundPenColor(short siNewColor)
		{ siBackgroundColor = siNewColor; }

	/*Get the current coordinate cursor position*/
	void vGetCursor(Real& rfX,Real& rfY)
		{ rfX = fX;  rfY = fY; }

	/*Get the current pixel cursor position*/
	void vGetCursor(int& riX,int& riY)
		{ riX = iX;  riY = iY; }

	/*Set the current cursor position*/
	virtual void vSetCursor(Real fNewX,Real fNewY);

	/*Get image size*/
	void vGetImageSize(int& riNX,int& riNY)
		{ riNX = iNX;  riNY = iNY; }

	/*Set image size*/
	virtual void vSetImageSize(int i_NX,int i_NY);

	/*Set size of coordinate image.  Used to set scaling*/
	virtual void vSetCoordSize(Real fXSize, Real fYSize);

	/*Draw a point at the current cursor position using
	  the current pen color */
	virtual void vDrawPoint()
		{ vDrawPoint(iX,iY); }

	/*Draw a point from the current cursor position to
	  point fNewX,fNewY using the current pen color.
   	  The cursor is set to fNewX,fNewY after drawing.
	  Uses vDrawLine(int iPX1,int iPY1,int iPX2,int iPY2) to render. */
	virtual void vDrawTo(Real fX,Real fY);

	/* Set the canvas to the background color.  This is implemented
	   by ALL derived classes */
	virtual void vClear()=0;
};

#define Drawable2D_H
#endif

/*-------- END DATA TYPES --------*/
