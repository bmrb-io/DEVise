/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   ContourScheme.cxx
PURPOSE: Class for assigning contour levels to real numbers
NOTES:	 When applied to a 2D matrix of real numbers, the contour scheme
	 is used to generate 2D contour plots 
DATE:    10/22/1997
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- INCLUDES --------*/

#include <strstream.h>
#undef __P // g++ complains about redefinition in math.h.  RKW Dec. 22, 1997.
#include <math.h>
#include <malloc.h>
#include <ContourScheme.hxx>

#define FIELD_DELIMETER '\t'

/*-------- END INCLUDES --------*/


/*METHODS*/
/*Constructor*/
ContourScheme::ContourScheme(Real f_MinLevel,
	int i_NLevels,
	Real f_ContourMult,
	Boolean b_PlotNegative)
{
	iNLevels = i_NLevels;
	bPlotNegative = b_PlotNegative;
	fContourMult = 2.0;

	vSetContourMult(f_ContourMult);
	vSetMinContourValue(f_MinLevel);
}

/*Stream constructor*/
ContourScheme::ContourScheme(istream& rIStream)
{
	rIStream >> *this;
}

/*Copy constructor*/
ContourScheme::ContourScheme(ContourScheme& rContourScheme)
{
	strstream tStrStream;

	tStrStream << rContourScheme;
	tStrStream >> *this;
}


/*Sets the value corresponding to the minimum contour level*/
void ContourScheme::vSetMinContourValue(Real fNewMinValue)
{ 
	if ( fNewMinValue > 1.0E-06 )
	{
		fMinLevel = fNewMinValue; 
		dlogMinLevel = log(fMinLevel);
	}
	else
		cerr << "Minimum contour must be greater than 1.0E-06" << endl;
}

/*Sets the value of the contour multiplier */
void ContourScheme::vSetContourMult(Real fNewContourMult)
{
	if ( fNewContourMult > 1.001 )
	{
		fContourMult = fNewContourMult;
		dlogContourMult = log(fContourMult);
	}
	else
		cerr <<"Contour multiplier must be greater than 1.001" << endl; 
}


/*Returns contour level corresponding to fValue 
  PARMS:	fValue		Real value that is assigned a contour
  RETURNS:	int		Contour level assigned to fValue 
  A return value of 0 means that there is no contour assigned to fValue
*/
int ContourScheme::iGetContourLevel(Real fValue)
{
	if ( bPlotNegative )
	{
		Boolean bIsPos = (fValue > 0);
		double dAbsValue = bIsPos ? fValue : -fValue; 
		if ( dAbsValue < fMinLevel )
			return 0;
		double dLevel =
			(log(dAbsValue) - dlogMinLevel)/dlogContourMult;
		int iLevel = 1 + (int)dLevel;
		iLevel = (iLevel > iNLevels) ? iNLevels : iLevel;

		return bIsPos ? iLevel : -iLevel;	
	}
	else
	{
		int iLevel = 0;
		if ( fValue >= fMinLevel )
		{
			double dLevel =
				(log(fValue) - dlogMinLevel)/dlogContourMult;
			iLevel = 1 + (int)dLevel;
			iLevel = (iLevel > iNLevels) ? iNLevels : iLevel;
		}
		return iLevel;	
	}
}

/*Writes contents from rContourScheme to rOStream*/
ostream& operator<<(ostream& rOStream,
	ContourScheme& rContourScheme)
{
	rOStream << rContourScheme.fMinLevel << FIELD_DELIMETER;
	rOStream << rContourScheme.iNLevels << FIELD_DELIMETER;
	rOStream << rContourScheme.fContourMult << FIELD_DELIMETER;
	rOStream << rContourScheme.bPlotNegative << endl;

	return rOStream;
}

/*Reads contents from rIStream into rContourScheme*/
istream& operator>>(istream& rIStream,
	ContourScheme& rContourScheme)
{
	Real f_MinLevel,f_ContourMult;

	rIStream >> f_MinLevel 
		>> rContourScheme.iNLevels 
		>> f_ContourMult
		>> rContourScheme.bPlotNegative;

	rContourScheme.vSetContourMult(f_ContourMult);
	rContourScheme.vSetMinContourValue(f_MinLevel);

	return rIStream;
}

/*Return array of contour values*/
Real* ContourScheme::pfGetContourValues()
{
	Real *pfContours = (Real*)calloc( iNLevels,sizeof(Real) );

	Real fValue = fMinLevel;
	pfContours[0] = fValue;

	for(int i=1; i<iNLevels; i++)
	{
		fValue *= fContourMult;
		pfContours[i] = fValue;
	}

	return pfContours; 
}

