/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   Drawable2D.hxx
PURPOSE: Defines abstract class for drawing points and lines on a 2D cartesian
	 surface 
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#include <stdlib.h>
#include "Drawable2D.hxx"

/*Macro for limiting the value of a to a value between b and c*/
#define LIMIT(a,b,c)  (a < b) ? b : (a < c) ? a : (c-1) 

#define SIGN(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)


/*Default constructor*/
Drawable2D::Drawable2D()
{
	siColor = 0;
	siBackgroundColor = 0;
	fX = fY = 0.0;
	iX = iY = 0;
	fScaleX = fScaleY = 1.0;
	iNX = iNY = 128;
	iNPoints = iNX*iNY;
}

Drawable2D::Drawable2D(int i_NX,int i_NY)
{
	siColor = 0;
	siBackgroundColor = 0;
	fX = fY = 0.0;
	iX = iY = 0;
	fScaleX = fScaleY = 1.0;
	iNX = i_NX;
	iNY = i_NY;
	iNPoints = iNX*iNY;
}

/*Set image size*/
void Drawable2D::vSetImageSize(int i_NX,int i_NY)
{
	if (  (i_NX != iNX) || (i_NY != iNY) )
	{
		iNX = i_NX;
		iNY = i_NY;
		iNPoints = iNX*iNY;
	}
}

/*Set size of coordinate image.  Used to set scaling*/
void Drawable2D::vSetCoordSize(Real fXSize, Real fYSize)
{
	fScaleX = (Real)iNX/fXSize;
	fScaleY = (Real)iNY/fYSize; 
}

/*Returns TRUE/FALSE depending upon whether iPX,iPY lies within
  the clipping boundaries */ 
int Drawable2D::bIsWithin(int iPX,int iPY)
{
	if ( iPX < 0 )
		return 0;
	if ( iPX >= iNX )
		return 0;
	if ( iPY < 0 )
		return 0;
	if ( iPY >= iNY )
		return 0;
	return 1; 
}

/*Set the current cursor position*/
void Drawable2D::vSetCursor(Real fNewX,Real fNewY)
{ 
	fX = fNewX;  fY = fNewY; 

	iX = (int)(fX*fScaleX + 0.50);
	iY = (int)(fY*fScaleY + 0.50);

	iX = LIMIT(iX,0,iNX);
	iY = LIMIT(iY,0,iNY);
}

/*Bressenham's algorithm for drawing a line from iPX1,iPY1
  to iPX2,iPY2.  Uses vDrawPoint(iPX,iPY) to render.
  This is taken from Chapter 10 of Graphics Programming in C by
  Roger T. Stevens */
void Drawable2D::vDrawLine(int x1,int y1,int x2,int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int sdx = SIGN(dx);
	int sdy = SIGN(dy); 
	int dxabs = abs(dx);
	int dyabs = abs(dy);
	int x = 0;
	int y = 0;
	int px = x1;
	int py = y1;

	if ( dxabs >= dyabs )
	{
		for(int i=0; i<=dxabs; i++)
		{
			y += dyabs;
			if ( y >= dxabs )
			{
				y -= dxabs;
				py += sdy;
			}
			if ( !bIsWithin(px,py) )
				break;
			vDrawPoint(px,py);
			px += sdx;
		}
	}
	else
	{
		for(int i=0; i<=dyabs; i++)
		{
			x += dxabs;
			if ( x >= dyabs )
			{
				x -= dyabs;
				px += sdx;
			}
			if ( !bIsWithin(px,py) )
				break;
			vDrawPoint(px,py);
			py += sdy;
		}
	}
}

/*Draw a point from the current cursor position to
  point fNewX,fNewY using the current pen color.
  The cursor is set to fNewX,fNewY after drawing */
void Drawable2D::vDrawTo(Real fX2,Real fY2)
{
	int iX2 = (int)(fX2*fScaleX + 0.50);
	int iY2 = (int)(fY2*fScaleY + 0.50);

	vDrawLine(iX,iY,iX2,iY2);

	fX = fX2;
	fY = fY2;
	iX = LIMIT(iX2,0,iNX);
	iY = LIMIT(iY2,0,iNY);
}

#undef LIMIT
