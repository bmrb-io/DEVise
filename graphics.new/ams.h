/* ams.h */
#ifndef ams_h
#define ams_h
#include <math.h>

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
