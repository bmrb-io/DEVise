/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   BoundaryPointList.h
PURPOSE: Defines a list of pointers to instances of class BoundaryPoint 
DATE:    10/25/1997
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- NEW DATA TYPES --------*/

#ifndef BoundaryPointList_H
	#include  "BoundaryPoint.h"

	#define DATA_TYPE BoundaryPoint
	#define LIST_TYPE BoundaryPointList
	#include "TemplateList.hxx"
	#undef LIST_TYPE
	#undef DATA_TYPE

#define BoundaryPointList_H
#endif

/*-------- END DATA TYPES --------*/
