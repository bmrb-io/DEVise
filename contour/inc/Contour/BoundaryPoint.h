/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   BoundaryPoint.h
PURPOSE: Defines a boundary point on a 2D grid of real numbers that is
	 assigned an integer intensity value. 
DATE:    10/28/1997
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#ifndef BoundaryPoint_H

	#include<Real.h>

	#define NBOUNDARY_DIRECTIONS 4

typedef struct
{
	Real fX;		/*X coordinate of boundary point*/
	Real fY;		/*Y coordinate of boundary point*/
	short siIntensity;	/*Integer intensity of boundary point*/
	short siDirection;	/*Direction 0(-y),1(+x),2(y),3(-x) */
	int iPos;		/*Position corresponding to col,row*/
} BoundaryPoint;

#define BoundaryPoint_H
#endif

/*-------- END DATA TYPES --------*/
