/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   PixelMap.cxx
PURPOSE: Defines a Drawable class consisting of a 2D array of pixels 
DERIVED FROM: Drawable.hxx
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#define LIMIT(a,b,c)  (a < b) ? b : (a < c) ? a : (c-1) 


#include <malloc.h>
#include <stdlib.h>
#include "PixelMap.hxx"

/*METHODS*/
/*Default constructor*/
PixelMap::PixelMap(): 
	Drawable2D()
{
	psiMap = 0;
}

/*Constructor with defined size*/
PixelMap::PixelMap(int i_NX,int i_NY):
	Drawable2D(i_NX,i_NY)
{
	psiMap = 0;
}

/*Set the current pen color */
void PixelMap::vSetPenColor(short siNewColor)
{ 
	siColor = siNewColor; 
}
	
/*Set image size*/
void PixelMap::vSetImageSize(int i_NX,int i_NY)
{
	if (  (i_NX != iNX) || (i_NY != iNY) )
	{
		iNX = i_NX;
		iNY = i_NY;
		iNPoints = iNX*iNY;
		if ( psiMap != 0 )
			free( psiMap );
		psiMap = 0;
	}
}


/*Returns pointer to internal pixel map*/
short* PixelMap::psiGetMap()
{
	if ( psiMap == 0 )
	{
		psiMap = (short*)malloc( iNPoints*sizeof(short) );
		vClear();
	}

	return psiMap;
}

/* Display a point with color siColor at the
   user coordinates given by fX,fY */
void PixelMap::vDrawPoint(int iPX,int iPY)
{
	int iPos = iPY*iNX + iPX;
	psiGetMap()[iPos] = siColor;
}


/*OUTPUT IMAGE TYPES*/
/*Output ppm style image of pixel map to output stream*/
void vGetPPMImage(PixelMap& rPixelMap,ostream& rOStream)
{
	short *psiMap = rPixelMap.psiGetMap();
	int iNPoints = rPixelMap.iNPoints;

	char black[3];
	char red[3];
	char white[3];

	black[0] = 0; black[1] = 0; black[2] = 0;
	red[0] = 1; red[1] = 0; red[2] = 0;
	white[0] = 1; white[1] = 1; white[2] = 1;

	/*Write header */
	rOStream << "P6\n#ppm intensity map" << endl;
	rOStream << rPixelMap.iNX << '\t' << rPixelMap.iNY << endl;

	/*Maximum range of bytes*/
	rOStream << 1 << endl;

	int iLevel;
	char *pcColor;
	for(int i=0; i<iNPoints; i++)
	{
		iLevel = psiMap[i];
		pcColor = (iLevel > 0) ? black : 
			(iLevel < 0) ? red : white;

		/*Output the intensity level*/
		rOStream.write( pcColor, 3);
		/*
		rOStream.put( pcColor[0] );
		rOStream.put( pcColor[1] );
		rOStream.put( pcColor[2] );
		*/
	}

}

/*Set the pixel map to the background color*/
void PixelMap::vClear()
{
	short *psiMap = psiGetMap();

	for(int i=0; i<iNPoints; i++)
		psiMap[i] = siBackgroundColor;		
}


/*Destructor*/
PixelMap::~PixelMap()
{
	if (psiMap != 0)
		free(psiMap);
}

#undef LIMIT
