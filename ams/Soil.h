/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.4  1995/09/05 20:23:47  jussi
  Updated CVS header.

  Revision 1.3  1995/09/05 20:20:37  jussi
  Updated CVS header

  Revision 1.2  1995/09/05 20:19:19  jussi
  Added CVS header
*/

#ifndef Soil_h
#define Soil_h
 struct AmsRec {
	char  SITEGRID_ID[12];
	int  STATION_ID;
	char  OBS_DATE[12];
	int  OBS_TIME ;
	float DRY_BULB_TEMP;
	float WET_BULB_TEMP;
	float ATMOSIC_PRESS;
	float  ACCUM_RAINFALL;
	float RAINFALL_RATE;
	float U_COMPNT_WIND_VELOC;
	float V_COMPNT_WIND_VELOC;
	float W_COMPNT_WIND_VELOC;
	float MAX_WIND_SPEED;
	float W_COMPNT_WIND_VELOC_SDEV;
	float SURF_TEMP;
	float SOIL_TEMP_10CM;
	float SOIL_TEMP_50CM;
	float SHORTWAVE_SOLAR_REFL;
	float NET_RADTN;
	float INCIDENT_LONGWAVE_RADTN;
	float DOME_TEMP;
	float INSTR_TEMP;
	float TOTAL_INCIDENT_RADTN;
	float TOTAL_INCIDENT_PAR;
	float DIFFUSE_INCIDENT_RADTN;
	float DIFFUSE_INCIDENT_PAR;
	char DATA_QUAL_CODE[28];
	char LAST_REVISION_DATE[10];
	char FIFE_DATA_CRTFCN_CODE[10];
	long DATE;
	int QUAL_OBS_DATE;
	int QUAL_OBS_TIME;
	int QUAL_DRY_BULB_TEMP;
	int QUAL_WET_BULB_TEMP;
	int QUAL_ATMOSIC_PRESS;
	int QUAL_ACCUM_RAINFALL;
	int QUAL_RAINFALL_RATE ;
	int QUAL_U_COMPNT_WIND_VELOC ;
	int QUAL_V_COMPNT_WIND_VELOC ;
	int QUAL_W_COMPNT_WIND_VELOC ;
	int QUAL_MAX_WIND_SPEED ;
	int QUAL_W_COMPNT_WIND_VELOC_SDEV ;
	int QUAL_SURF_TEMP ;
	int QUAL_SOIL_TEMP_10CM ;
	int QUAL_SOIL_TEMP_50CM ;
	int QUAL_SHORTWAVE_SOLAR_REFL ;
	int QUAL_NET_RADTN ;
	int QUAL_INCIDENT_LONGWAVE_RADTN ;
	int QUAL_DOME_TEMP ;
	int QUAL_INSTR_TEMP ;
	int QUAL_TOTAL_INCIDENT_RADTN ;
	int QUAL_TOTAL_INCIDENT_PAR ;
	int QUAL_DIFFUSE_INCIDENT_RADTN ;
	int QUAL_DIFFUSE_INCIDENT_PAR ;
};

#endif
