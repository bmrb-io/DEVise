/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   PixelMap.h
PURPOSE: Defines a Drawable class consisting of a 2D array of pixels 
DERIVED FROM: Drawable.hxx
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#ifndef PixelMap_H
	#include <stream.h>
	#include "Drawable2D.hxx"

class PixelMap : public Drawable2D 
{
/*MEMBERS*/
protected:
	/*Pointer to array of short integers that
	  represents the iNX by iNY pixels */
	short* psiMap;

protected:
	/*Draw a point at iPX,iPY using the current pen color */
	void vDrawPoint(int iPX,int iPY);

/*METHODS*/
public:
	/*Default constructor*/
	PixelMap();

	/*Constructor with defined size*/
	PixelMap(int i_NX,int i_NY);

	
	/*Set image size*/
	virtual void vSetImageSize(int i_NX,int i_NY);

	/*Set the current pen color */
	virtual void vSetPenColor(short siNewColor);

	/*Returns number of points in pixel map*/
	int iGetNPoints()
		{ return iNPoints; }

	/*Returns pointer to internal pixel map*/
	short *psiGetMap();

	/*Set the pixel map to the background color*/
	void vClear();

	/*Destructor*/
	~PixelMap();


	/*FRIEND FUNCTIONS*/
	/*Output ppm style image of pixel map to output stream*/
	friend void vGetPPMImage(PixelMap& rPixelMap,ostream& rOStream);
};

#define PixelMap_H
#endif

/*-------- END DATA TYPES --------*/
