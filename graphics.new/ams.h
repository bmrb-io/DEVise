/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1997/03/28 16:10:30  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:16:26  jussi
  Added CVS header.
*/

#ifndef ams_h
#define ams_h
#ifndef SGI
#include <math.h>
#endif


/* missing data values */
const double Missing_SolDn = -1.0;
const double Missing_WindU = -99.0;
const double Missing_WindV = -99.0;
const double Missing_WindW = -99.0;
const double Missing_TempWet = -50.0;
const double Missing_TempDry = -50.0;
const double Missing_Rain = -1.0;

struct AmsData {
	double time;	/* # of minutes since 05/01/87 */
	double solDn; /* solar radiation */
	double windU; /* u component of wind */
	double windV; /* v component of wind */
	double windW; /* w component of wind */
	double tempWet; /* wet bulb temperature */
	double tempDry; /* air (dry bulb) temperature */
	double rain;	/* amount of rain in this time period */
};

/* Convert tempWet to vapor pressure */
inline double VaporPressure(double tempWet){
	return 6.108*pow(10.0,7.5*tempWet/(237.3+tempWet));
};

#endif
