/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   ContourMap.h
PURPOSE: Creates contour maps given a ContourScheme and a 2D matrix of
	 real data. 
DATE:    10/22/07
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- INCLUDES --------*/

#include <stream.h>
#include "ContourScheme.hxx"
#include "Drawable2D.hxx"
#include "PixelMap.hxx"

/*-------- END INCLUDES --------*/



/*-------- NEW DATA TYPES --------*/

#ifndef ContourMap_H

class ContourMap
{
/*MEMBERS*/
private:
	/*Name of temporary file to be used for transfering
	  matrix data through stream input and output*/
	char *pcFilenm;

protected:
	/*Current resolution of the data along the x dimension*/
	int iDataX;

	/*Current resolution of the data along the y dimension*/
	int iDataY;

	/* 2D matrix of real data */
	Real* pfMatrix;

	/*Resolution of the contour map along the x dimension*/
	int iMapX;

	/*Resolution of the contour map along the y dimension*/
	int iMapY;

	/*Contour scheme which, when applied to a 2D matrix of data,
	  generates a series of contours for a given contour map
	  resolution. */
	ContourScheme* ptContourScheme;


/*METHODS*/
public:
	/*Stream constructor
	  Attributes are read from the input stream in the following order:
	  1) The 2D x,y real matrix resolution
	  	FORMAT: iDataX <DELIM> iDataY <RET> 
	  2) Filenm of the binary file of N float values 
		where N = iDataX*iDataY
	     This 2D matrix represents the data to be visualized
	  3) The x,y resolution of the contour map
		FORMAT: iMapX <DELIM> iMapY <RET>
	  4) The contour scheme
		FORMAT: fMinLevel <DELIM>
			iNLevels  <DELIM>
			fContourMult <DELIM>
			bPosNegative <RET>
		where fMinLevel is the absolute value of the minimum
		contour level, iNLevels is the number of contour levels,
		fContourMult is the geometric spacing between consecutive
		contour levels (e.g. 2.0), and bPosNegative is a boolean
		value 0/1  that indicates whether or not negative contours
		are to be displayed. 
	  The above formats are not as strict as documented. Almost any
	  white space character (' ','\t','\n') should be able to replace 
	  either <DELIM> or <RET> 
	*/
	ContourMap( istream& rIStream );



	/*Returns pointer to embedded contour scheme.  This pointer
	  can be used to directly change attributes of the current
	  contour scheme */
	ContourScheme* ptGetContourScheme()
		{ return ptContourScheme; }

	/*Copies to riMapX and riMapY the current x,y resolutions of
	  the contour map */
	void vGetMapResolution(int& riMapX,int& riMapY)
		{ riMapX = iMapX; riMapY = iMapY; }

	/*Sets the current x,y resolutions of the contour map*/
	void vSetMapResolution(int iNewMapX,int iNewMapY);

	/*Copies to riDataX and riDataY the current x,y resolutions
	  of the data to be visualized */
	void vGetDataResolution(int& riDataX,int& riDataY)
		{ riDataX = iDataX;  riDataY = iDataY; }

	/*Returns pointer to current data*/
	float *pfGetMatrixData()
		{ return pfMatrix; }

	/*Sets pointer to new data set.  iNewDataX and iNewDataY are the
	  resolutions of the new data set */
	void vSetMatrixData(float *pfNewMatix,int iNewDataX,int iNewDataY);

	/*Sends all info associated with this instance through the ostream.
	  The data is sent in the following way:
	  1) The 2D x,y real matrix resolution
	  	FORMAT: iDataX <DELIM> iDataY <RET> 
	  2) Filenm of the binary file of N float values 
		where N = iDataX*iDataY
	  3) The x,y resolution of the contour map
		FORMAT: iMapX <DELIM> iMapY <RET>
	  4) The contour scheme
		FORMAT: fMinLevel <DELIM>
			iNLevels  <DELIM>
			fContourMult <DELIM>
			bPosNegative <RET>
	      where <DELIM> is a '\t' and <RET> is a '\n'
	*/
	friend ostream& operator<<(ostream& rOStream,
		ContourMap& rContourMap);

	/*Retrieves all info associated with this instance from an istream.
	  The data is retrived in the same manner as documented in the
	  istream constructor*/
	friend istream& operator>>(istream& rIStream,
		ContourMap& rContourMap);


	/*Draws a iMapX X iMapY map of integer intensity values
	  which are an interpolated intensity image of the data contained 
	  in pfMatrix.  Positive and negative integer values represent the
	  intensity levels assigned to positive and negative matrix
	  values respectively.  The intensity values are the values derived 
	  from applying the current contour scheme to the data contained in 
	  pfMatrix.  The fact that the resolution of the intensity map may be 
	  different than the resolution of the matrix allows for shrinking and 
	  stretching of the image along both dimensions.
	*/
	void vDrawIntensityMap(Drawable2D& rDrawable);

	/*Draws a iMapX X iMapY map of integer intensity values 
	  which represent a contour image of the data contained 
	  in pfMatrix.  Positive and negative integer values represent the
	  intensity levels assigned to positive and negative matrix
	  values respectively.  The intensity values are the values derived 
	  from applying the current contour scheme to the data contained in 
	  pfMatrix.  The fact that the resolution of the intensity map may be 
	  different than the resolution of the matrix allows for shrinking and 
	  stretching of the image along both dimensions.
	*/
	void vDrawContourMap(Drawable2D& rDrawable);


	/*Destructor*/
	~ContourMap();
};

#define ContourMap_H
#endif

/*-------- END DATA TYPES --------*/
