/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   ContourScheme.h
PURPOSE: Class for assigning contour levels to real numbers
NOTES:	 When applied to a 2D matrix of real numbers, the contour scheme
	 is used to generate 2D contour plots 
DATE:    10/22/1997
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- INCLUDES --------*/

#include <iostream.h>
#include "Real.h"
#include "Boolean.h"

/*-------- END INCLUDES --------*/



/*-------- NEW DATA TYPES --------*/

#ifndef ContourScheme_H
#define ContourScheme_H


class ContourScheme
{
/*MEMBERS*/
private:
	/*Log value of fMinLevel */
	double dlogMinLevel;

	/*Log value of contour multiplier*/
	double dlogContourMult;

protected:
	/*Absolute value of lowest contour level*/
	Real fMinLevel;

	/*Number of contour levels*/
	int iNLevels;

	/*Multiplicative factor between contour levels */
	Real fContourMult;

	/*Boolean which determines whether or not negative contour
	  levels are to be visualized */
	Boolean bPlotNegative;


/*METHODS*/
public:
	/*Constructor*/
	ContourScheme(Real f_MinLevel,
		int i_NLevels = 4,
		Real f_ContourMult = 2.0,
		Boolean b_PlotNegative = 1);

	/*Stream constructor*/
	ContourScheme(istream& rIStream);

	/*Copy constructor*/
	ContourScheme(ContourScheme& rContourScheme);


	/*Member access functions*/
	int iGetNLevels()
		{ return iNLevels; }

	void vSetNLevels(int iNewNLevels)
		{ iNLevels = iNewNLevels; }

	Real fGetMinContourValue()
		{ return iNLevels; }

	void vSetMinContourValue(Real fNewMinValue);

	Real fGetContourMult()
		{ return fContourMult; }

	void vSetContourMult(Real fNewContourMult);

	/*Return array of contour values*/
	Real* pfGetContourValues();

	Boolean bDoesPlotNegative()
		{ return bPlotNegative; }

	void vSetPlotNegative(Boolean bDoesPlotNegative)
		{ bPlotNegative = bDoesPlotNegative; }


	/*Returns contour level corresponding to fValue 
	  PARMS:	fValue		Real value that is assigned a contour
	  RETURNS:	int		Contour level assigned to fValue 
	  A return value of 0 means that there is no contour assigned to fValue
	*/
	int iGetContourLevel(Real fValue); 

	/*Writes contents from rContourScheme to rOStream*/
	friend ostream& operator<<(ostream& rOStream,
		ContourScheme& rContourScheme);

	/*Reads contents from rIStream into rContourScheme*/
	friend istream& operator>>(istream& rIStream,
		ContourScheme& rContourScheme);
};

#endif
