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
  Revision 1.8  1995/12/14 19:11:05  jussi
  Further small fixes.

  Revision 1.7  1995/12/14 18:43:50  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.6  1995/12/14 15:42:46  jussi
  Replaced WinVertical and WinHorizontal with TileLayout.

  Revision 1.5  1995/12/02 21:38:56  jussi
  Added horizontal view layout.

  Revision 1.4  1995/09/22 16:01:09  jussi
  Added copyright message.

  Revision 1.3  1995/09/05 20:08:33  jussi
  Updated header

  Revision 1.2  1995/09/05 20:05:24  jussi
  Added CVS header.
*/

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "Display.h"
#include "ColorMgr.h"
#include "Control.h"
#include "Init.h"
#include "WinClassInfo.h"
#include "MapInterpClassInfo.h"
#include "VisualLinkClassInfo.h"
#include "VisualArg.h"
#include "View.h"
#include "AxisLabelClassInfo.h"
#include "ViewClassInfo.h"
#include "CompositeParser.h"
#include "RecInterp.h"
#include "AttrList.h"
#include "QueryProc.h"
#ifndef NOTAPE
#include "QueryProcTape.h"
#endif
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "CursorClassInfo.h"
#ifndef NOTAPE
#include "TDataTapeInterp.h"
#endif
#include "ParseCat.h"
#include "Command.h"
#include "MapClassInfo.h"

int debug = 0;

char *monthNames[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL",
			 "AUG", "SEP", "OCT", "NOV", "DEC" };

int GetMonth(char *month)
{
  static int monthHint = -1;
  if (monthHint >= 0 && strcmp(monthNames[monthHint],month) == 0){
    return monthHint;
  }

  for(int i = 0; i < 12; i++) {
    if (strcmp(monthNames[i],month) == 0) {
      monthHint = i;
      return i;
    }
  }

  /* not found */
  fprintf(stderr,"unknown month %s\n", month);
  Exit::DoExit(2);

  /* keep compiler happy */
  return 0;
}

/* name of quality attrbutess */
const int NUM_QUAL_ATTRS = 24;
int qualOffset[NUM_QUAL_ATTRS]; /* offset of quality attributes */
char *qualAttrs[] = {
"QUAL_OBS_DATE",
"QUAL_OBS_TIME",
"QUAL_DRY_BULB_TEMP",
"QUAL_WET_BULB_TEMP",
"QUAL_ATMOSIC_PRESS",
"QUAL_ACCUM_RAINFALL",
"QUAL_RAINFALL_RATE",
"QUAL_U_COMPNT_WIND_VELOC",
"QUAL_V_COMPNT_WIND_VELOC",
"QUAL_W_COMPNT_WIND_VELOC",
"QUAL_MAX_WIND_SPEED",
"QUAL_W_COMPNT_WIND_VELOC_SDEV",
"QUAL_SURF_TEMP",
"QUAL_SOIL_TEMP_10CM",
"QUAL_SOIL_TEMP_50CM",
"QUAL_SHORTWAVE_SOLAR_REFL",
"QUAL_NET_RADTN",
"QUAL_INCIDENT_LONGWAVE_RADTN",
"QUAL_DOME_TEMP",
"QUAL_INSTR_TEMP",
"QUAL_TOTAL_INCIDENT_RADTN",
"QUAL_TOTAL_INCIDENT_PAR",
"QUAL_DIFFUSE_INCIDENT_RADTN",
"QUAL_DIFFUSE_INCIDENT_PAR"
};

/* User composite to parse date */
class AmsComposite : public UserComposite{
public:
	AmsComposite(){

		time_t clk;
		(void)time(&clk);
		struct tm *tstr = localtime(&clk);
		timestr= *tstr;
		lasttstr = *tstr;
		lasttime = clk;
		_first =true;
		_init = true;
	}

	/* This is called by the Composite parser to parse composite attributes */
	virtual void Decode(RecInterp *recInterp){
		char dateBuf[20];
		int i;
		AttrInfo *info;
		char *buf;
		char *dt ;
		int *tm;
		char *qualStr, qualCh;
		int *qualPtr;
		time_t *datePtr ;
		time_t time;
		char *errorAttr;
		if (_init){
			/* initialize by caching offsets of all the attributes we need*/
				
			if ((info = recInterp->GetAttrInfo("DATA_QUAL_CODE")) == NULL){
				errorAttr = "DATA_QUAL_CODE";
				goto error;
			}
			qualStrOffset = info->offset;

			if ((info = recInterp->GetAttrInfo("DATE")) == NULL){
				errorAttr = "DATE";
				goto error;
			}
			dateOffset = info->offset;

			if ((info = recInterp->GetAttrInfo("OBS_DATE")) == NULL){
				errorAttr = "OBS_DATE";
				goto error;
			}
			obsDateOffset = info->offset;

			if ((info = recInterp->GetAttrInfo("OBS_TIME")) == NULL){
				errorAttr = "OBS_TIME";
				goto error;
			}
			obsTimeOffset = info->offset;


			for (i=0; i < NUM_QUAL_ATTRS; i++){
				if ((info = recInterp->GetAttrInfo(qualAttrs[i])) == NULL){
					errorAttr = qualAttrs[i];
					goto error;
				}
				qualOffset[i] = info->offset;
			}

			_init = false;
		}

		/*
		printf("AmsComposite parsing %s ",attrInfo->name);
		*/

		buf = (char *)recInterp->GetBuf();

		/* decode date */
		datePtr = (time_t *)(buf+dateOffset);
		dt = buf + obsDateOffset;
		strcpy(dateBuf, dt);
		dt = dateBuf;
		tm = (int *)(buf+obsTimeOffset);
		/*
		printf(" time %d\n", *tm);
		*/

		dt[2] = '\0';
		dt[6] = '\0';
		timestr.tm_year = atoi(&dt[7]);
		timestr.tm_mon = GetMonth(&dt[3]);
		timestr.tm_mday = atoi(&dt[0]);
		timestr.tm_hour = (*tm) /100;
		timestr.tm_min = (*tm) % 100;
		timestr.tm_sec = 0;

		/* see if we can reuse the time value from last call's
		   mktime() call; mktime costs 1 msec per call so we
		   don't want to call it too often */

		if (timestr.tm_year == lasttstr.tm_year
		    && timestr.tm_mon == lasttstr.tm_mon) {
		  int diff = (timestr.tm_mday - lasttstr.tm_mday) * 24 * 3600 +
		             (timestr.tm_hour - lasttstr.tm_hour) * 3600 +
		             (timestr.tm_min  - lasttstr.tm_min) * 60 +
		             (timestr.tm_sec  - lasttstr.tm_sec);
		  time = lasttime + diff;
		} else {
		  time = mktime(&timestr);
		  memcpy(&lasttstr, &timestr, sizeof lasttstr);
		  lasttime = time;
		}

		*datePtr  = time;

		if (_first){
			/*
			printf("Composite: first date = %d\n", time);
			*/
			_first = false;
		}

		/* compute quality attributes */
		qualStr = (buf+qualStrOffset);

		for (i=0; i < NUM_QUAL_ATTRS; i++){
			qualPtr  = (int *)(buf+qualOffset[i]);

			qualCh = qualStr[i+1];
			if (qualCh == 'G' || qualCh == 'C')
				*qualPtr = 3;
			else if (qualCh == '_')
				*qualPtr = 1;
			else *qualPtr = 2;
		}
		return;
	error:
		fprintf(stderr,"Ams composite attr: can't find attr %s\n",
			errorAttr);
		Exit::DoExit(2);
	}
private:
	struct tm timestr;              /* current time */
	struct tm lasttstr;             /* cached value of last time */
	time_t    lasttime;             /* cached value of last time */
	Boolean _first;
	Boolean _init;
	int qualStrOffset; /* offset of quality string */
	int dateOffset; /* offset of date attribute */
	int obsDateOffset; /* offset of observed date */
	int obsTimeOffset; /* offset of observed time */
};

class BaselComposite : public UserComposite{
public:
	BaselComposite(){
		time_t clk;
		(void)time(&clk);
		struct tm *tstr = localtime(&clk);
		lasttstr = *tstr;
		lasttime = clk;
		timestr= *tstr;
		_init = true;
	}

	/* This is called by the Composite parser to parse composite attributes */
	virtual void Decode(RecInterp *recInterp){
		char dateBuf[20];
		AttrInfo *info;
		char *buf;
		char *dt ;
		int *tm;
		time_t *datePtr ;
		time_t time;
		char *errorAttr;

		if (_init){
			/* initialize by caching offsets of all the attributes we need*/
			if ((info = recInterp->GetAttrInfo("DATE")) == NULL){
				errorAttr = "DATE";
				goto error;
			}
			_dateOffset = info->offset;

			if ((info = recInterp->GetAttrInfo("OBS_DATE")) == NULL){
				errorAttr = "OBS_DATE";
				goto error;
			}
			_obsDateOffset = info->offset;

			if ((info = recInterp->GetAttrInfo("OBS_TIME")) == NULL){
				errorAttr = "OBS_TIME";
				goto error;
			}
			_obsTimeOffset = info->offset;
			_init = false;
		}

		buf = (char *)recInterp->GetBuf();

		/* decode date */
		datePtr = (time_t *)(buf+_dateOffset);
		dt = buf + _obsDateOffset;
		strcpy(dateBuf, dt);
		dt = dateBuf;
		tm = (int *)(buf+_obsTimeOffset);
		/*
		printf(" time %d\n", *tm);
		*/

		dt[2] = '\0';
		dt[6] = '\0';
		timestr.tm_year = atoi(&dt[7]);
		timestr.tm_mon = GetMonth(&dt[3]);
		timestr.tm_mday = atoi(&dt[0]);
		timestr.tm_hour = (*tm) /100;
		timestr.tm_min = (*tm) % 100;
		timestr.tm_sec = 0;

		/* see if we can reuse the time value from last call's
		   mktime() call; mktime costs 1 msec per call so we
		   don't want to call it too often */

		if (timestr.tm_year == lasttstr.tm_year
		    && timestr.tm_mon == lasttstr.tm_mon) {
		  int diff = (timestr.tm_mday - lasttstr.tm_mday) * 24 * 3600 +
		             (timestr.tm_hour - lasttstr.tm_hour) * 3600 +
		             (timestr.tm_min  - lasttstr.tm_min) * 60 +
		             (timestr.tm_sec  - lasttstr.tm_sec);
		  time = lasttime + diff;
		} else {
		  time = mktime(&timestr);
		  memcpy(&lasttstr, &timestr, sizeof lasttstr);
		  lasttime = time;
		}

		*datePtr  = time;
		return;
	error:
		fprintf(stderr,"Basel composite attr: can't find attr %s\n",
			errorAttr);
		Exit::DoExit(2);
	}
private:
	struct tm timestr;              /* current time */
	struct tm lasttstr;             /* cached value of last time */
	time_t    lasttime;             /* cached value of last time */
	Boolean _init;
	int _dateOffset;
	int _obsDateOffset;
	int _obsTimeOffset;
};

#ifndef NOTAPE
class AmsGenClass: public GenClassInfo  {
public:
	virtual ClassInfo *Gen(char *source, char *className, AttrList *attrList,
		int recSize, char *separators, int numSeparators,
		Boolean isSeparator, char *commentString){
		if (strcmp(source,"tape") == 0){
			return new TDataTapeInterpClassInfo(className,attrList,
				recSize, separators, numSeparators,
				isSeparator, commentString);
		}

		fprintf(stderr,"schema source %s not found\n", source);
	        Exit::DoExit(1);

		// keep compiler happy
		return 0;
	}
};


QueryProc *genQueryProcTape()
{
  return new QueryProcTape;
}
#endif

main(int argc, char **argv){

	Init::DoInit(argc,argv);

	/* Register composite parser */
	CompositeParser::Register("Soil", new AmsComposite);

#ifndef NOTAPE
	CompositeParser::Register("TapeSoil", new AmsComposite);
#endif
	CompositeParser::Register("Basel92", new BaselComposite);

#ifndef NOTAPE
	/* Register parser for tape */
	RegisterGenClassInfo("tape",new AmsGenClass());

	/* Register known query processors */
	QueryProc::Register("Tape", genQueryProcTape);
#endif

	/* Register known classes  with control panel */
	ControlPanel::RegisterClass(new TileLayoutInfo);
	ControlPanel::RegisterClass(new ViewXInfo);
	ControlPanel::RegisterClass(new ViewScatterInfo);
	ControlPanel::RegisterClass(new VisualLinkClassInfo());
	ControlPanel::RegisterClass(new CursorClassInfo());
	/* Create compiled mappings for solar radiation and rain */
	ControlPanel::RegisterClass(new MapInfo(MapInfo::SolDnType));
	ControlPanel::RegisterClass(new MapInfo(MapInfo::RainType));
	ControlPanel::RegisterClass(new MapInfo(MapInfo::TempType));

	/* hack to start control panel so that it'll read the RC files */
	ControlPanel *ctrl = ControlPanel::Instance();

	/* This is a hack to create a display before running Dispatcher.
	Otherwise, we'll get an error */
	DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();

	/* Start session (possibly restoring an old one */
	ctrl->StartSession();

	/* Create a command class to read input from keyboard */
	Command *cmd = new Command(QueryProc::Instance());

	/* keep compiler happy */
	disp = disp;
	cmd = cmd;

	Dispatcher::RunNoReturn();
}
