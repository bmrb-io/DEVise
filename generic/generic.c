/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1996
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
  Revision 1.14  1996/04/19 17:20:11  wenger
  Put the GenClassInfo code back in -- this is needed for tape data;
  started adding the tape-related code back in (it was previously
  deleted for some reason; I'm not yet done adding it back); added
  the 'DEVise parseSchema' command and the first parts of the code
  related to it.

  Revision 1.13  1996/04/16 21:00:14  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.12  1996/04/04 05:16:19  kmurli
  No apparent modification done to generic.c. Only added printf commands to test and later removed them.

  Revision 1.11  1996/03/27 15:27:27  jussi
  Added initialization of dateAttr.

  Revision 1.10  1996/03/26 20:22:34  jussi
  Added tape query processor.

  Revision 1.9  1996/03/23 23:34:16  jussi
  Added MdDdYy to DATE composite parser.

  Revision 1.8  1996/03/23 21:40:22  jussi
  Added support for date fields as they appear in Department of Labor
  statistics.

  Revision 1.7  1996/01/26 19:46:21  jussi
  Added two composite parsers for common date formats.

  Revision 1.6  1995/12/14 18:42:45  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.5  1995/12/14 15:43:12  jussi
  Replaced WinVertical and WinHorizontal with TileLayout.

  Revision 1.4  1995/12/02 21:38:10  jussi
  Added horizontal view layout.

  Revision 1.3  1995/09/22 15:58:07  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:33:08  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

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
#include "QueryProcTape.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "CursorClassInfo.h"
#include "ParseCat.h"

static time_t GetTime(struct tm &now)
{
  static struct tm lasttm;
  static time_t    lasttime = (time_t)-1;

  /* see if we can reuse the time value from last call's
     mktime() call; mktime costs 1 msec per call so we
     don't want to call it too often */
  
  time_t nowtime = 0;

  if (lasttime != (time_t)-1 && now.tm_year == lasttm.tm_year
      && now.tm_mon == lasttm.tm_mon) {
    int diff = (now.tm_mday - lasttm.tm_mday) * 24 * 3600 +
               (now.tm_hour - lasttm.tm_hour) * 3600 +
	       (now.tm_min  - lasttm.tm_min) * 60 +
	       (now.tm_sec  - lasttm.tm_sec);
    nowtime = lasttime + diff;
  } else {
    nowtime = mktime(&now);
    lasttime = nowtime;
    lasttm = now;
  }

  return nowtime;
}

/* User composite function for dates of the form YYMMDD */

class YyMmDdComposite : public UserComposite {
public:

  YyMmDdComposite() {
    _init = false;
    attrOffset = 0;
    dateAttr = 0;
  }

  virtual ~YyMmDdComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "YYMMDD", "DATE" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	attrOffset[i] = info->offset;
	if (!strcmp(primAttrs[i], "DATE"))
	  dateAttr = info;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    static struct tm now;
    int seq = *(int *)(buf + attrOffset[0]);
    now.tm_mday = seq % 100;
    now.tm_mon = (seq / 100) % 100 - 1;
    now.tm_year = seq / 10000;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;

    time_t *datePtr = (time_t *)(buf + attrOffset[1]);
    *datePtr = GetTime(now);

    if (!dateAttr->hasHiVal || *datePtr > dateAttr->hiVal.dateVal) {
      dateAttr->hiVal.dateVal = *datePtr;
      dateAttr->hasHiVal = true;
    }
    if (!dateAttr->hasLoVal || *datePtr < dateAttr->loVal.dateVal) {
      dateAttr->loVal.dateVal = *datePtr;
      dateAttr->hasLoVal = true;
    }
  }

private:
  int       *attrOffset;          /* attribute offsets */
  AttrInfo  *dateAttr;            /* date attribute info */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for dates of the form MMDDYY */

class MmDdYyComposite : public UserComposite {
public:

  MmDdYyComposite() {
    _init = false;
    attrOffset = 0;
    dateAttr = 0;
  }

  virtual ~MmDdYyComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "MONTH", "DAY", "YEAR", "DATE" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	attrOffset[i] = info->offset;
	if (!strcmp(primAttrs[i], "DATE"))
	  dateAttr = info;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    static struct tm now;
    now.tm_mday = *(int *)(buf + attrOffset[1]);
    now.tm_mon = *(int *)(buf + attrOffset[0]) - 1;
    now.tm_year = *(int *)(buf + attrOffset[2]) - 1900;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;

    time_t *datePtr = (time_t *)(buf + attrOffset[3]);
    *datePtr = GetTime(now);

    if (!dateAttr->hasHiVal || *datePtr > dateAttr->hiVal.dateVal) {
      dateAttr->hiVal.dateVal = *datePtr;
      dateAttr->hasHiVal = true;
    }
    if (!dateAttr->hasLoVal || *datePtr < dateAttr->loVal.dateVal) {
      dateAttr->loVal.dateVal = *datePtr;
      dateAttr->hasLoVal = true;
    }
  }

private:
  int       *attrOffset;          /* attribute offsets */
  AttrInfo  *dateAttr;            /* date attribute info */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for OBSDATE field */

class ObsDateComposite : public UserComposite {
public:

  ObsDateComposite() {
    _init = false;
    attrOffset = 0;
    dateAttr = 0;
  }

  virtual ~ObsDateComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "OBS_DAY", "OBS_MON", "OBS_YEAR",
			    "OBS_HOUR", "OBS_MIN", "OBS_SEC",
			    "DATE" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	attrOffset[i] = info->offset;
	if (!strcmp(primAttrs[i], "DATE"))
	  dateAttr = info;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    static struct tm now;
    now.tm_mday = *(int *)(buf + attrOffset[0]);
    now.tm_mon = *(int *)(buf + attrOffset[1]) - 1;
    now.tm_year = *(int *)(buf + attrOffset[2]) - 1900;
    now.tm_hour = *(int *)(buf + attrOffset[3]);
    now.tm_min = *(int *)(buf + attrOffset[4]);
    now.tm_sec = *(int *)(buf + attrOffset[5]);

    time_t *datePtr = (time_t *)(buf + attrOffset[6]);
    *datePtr = GetTime(now);

    if (!dateAttr->hasHiVal || *datePtr > dateAttr->hiVal.dateVal) {
      dateAttr->hiVal.dateVal = *datePtr;
      dateAttr->hasHiVal = true;
    }
    if (!dateAttr->hasLoVal || *datePtr < dateAttr->loVal.dateVal) {
      dateAttr->loVal.dateVal = *datePtr;
      dateAttr->hasLoVal = true;
    }
  }

private:
  int       *attrOffset;          /* attribute offsets */
  AttrInfo  *dateAttr;            /* date attribute info */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for Department of Labor date fields */

class DOLDateComposite : public UserComposite {
public:

  DOLDateComposite() {
    _init = false;
    attrOffset = 0;
    dateAttr = 0;
  }

  virtual ~DOLDateComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "YEAR", "PERIOD", "DATE" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	attrOffset[i] = info->offset;
	if (!strcmp(primAttrs[i], "DATE"))
	  dateAttr = info;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    static struct tm now;
    now.tm_mday = 1;
    now.tm_mon = atoi(buf + attrOffset[1] + 1) - 1;
    now.tm_year = *(int *)(buf + attrOffset[0]) - 1900;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;

    if (now.tm_mon >= 12) {
      // the annual statistic is moved to Dec 15th
      now.tm_mon = 11;
      now.tm_mday = 15;
    }

    time_t *datePtr = (time_t *)(buf + attrOffset[2]);
    *datePtr = GetTime(now);

    if (!dateAttr->hasHiVal || *datePtr > dateAttr->hiVal.dateVal) {
      dateAttr->hiVal.dateVal = *datePtr;
      dateAttr->hasHiVal = true;
    }
    if (!dateAttr->hasLoVal || *datePtr < dateAttr->loVal.dateVal) {
      dateAttr->loVal.dateVal = *datePtr;
      dateAttr->hasLoVal = true;
    }
  }

private:
  int       *attrOffset;          /* attribute offsets */
  AttrInfo  *dateAttr;            /* date attribute info */
  Boolean   _init;                /* true when instance initialized */
};

/*
   User composite function for geographical locations that translates
   a state abbreviation to latitude/longitude attributes
*/

class StateLatLonComposite : public UserComposite {
public:

  StateLatLonComposite() {
    _init = false;
    _warning = true;
    attrOffset = 0;
    latAttr = 0;
    lonAttr = 0;
    totalAttr = 0;
  }

  virtual ~StateLatLonComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "State", "TotalAmount",
			    "Latitude", "Longitude", "TotalAmountDegree" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	attrOffset[i] = info->offset;
	if (!strcmp(primAttrs[i], "Latitude"))
	  latAttr = info;
	else if (!strcmp(primAttrs[i], "Longitude"))
	  lonAttr = info;
	else if (!strcmp(primAttrs[i], "TotalAmountDegree"))
	  totalAttr = info;
      }
      _init = true;
    }

    /*
       Translate state name to the latitude/longitude of the state capital
       or some other big city in state
     */

    struct {
      char *state;
      float latitude;
      float longitude;
    } StateLatLon[] = {
      { "AL", +32.354400, -86.284287 },  /* Montgomery */
      { "AK", +61.178368, -149.186416 }, /* Anchorage */
      { "AZ", +33.542550, -112.071399 }, /* Phoenix */
      { "AR", +34.722400, -92.354076 },  /* Little Rock */
      { "CA", +34.112101, -118.411201 }, /* Los Angeles */
      { "CO", +39.768035, -104.872655 }, /* Denver */
      { "CT", +41.765700, -72.683866 },  /* Hartford */
      { "DC", +38.905050, -77.016167 },  /* Washington */
      { "DE", +39.735572, -75.529956 },  /* Wilmington */
      { "FL", +25.775667, -80.210845 },  /* Miami */
      { "GA", +33.762900, -84.422592 },  /* Atlanta */
      { "HI", +21.317250, -157.804233 }, /* Honolulu */
      { "ID", +43.606651, -116.226100 }, /* Boise */
      { "IL", +41.837050, -87.684965 },  /* Chicago */
      { "IN", +39.776400, -86.146196 },  /* Indianapolis */
      { "IA", +41.576738, -93.617405 },  /* Des Moines */
      { "KS", +37.687350, -97.342674 },  /* Wichita */
      { "KY", +38.224750, -85.741156 },  /* Louisville */
      { "LA", +30.065846, -89.931355 },  /* New Orleans */
      { "ME", +43.667134, -70.207166 },  /* Portland */
      { "MD", +39.300800, -76.610616 },  /* Baltimore */
      { "MA", +42.336029, -71.017892 },  /* Boston */
      { "MI", +42.383100, -83.102198 },  /* Detroit */
      { "MN", +44.961850, -93.266849 },  /* Minneapolis */
      { "MO", +39.122312, -94.552009 },  /* Kansas City */
      { "MS", +32.320500, -90.207591 },  /* Jackson */
      { "MT", +46.596522, -112.020381 }, /* Helena */
      { "NE", +41.263900, -96.011745 },  /* Omaha */
      { "NJ", +40.724100, -74.173245 },  /* Newark */
      { "NH", +42.983600, -71.444899 },  /* Manchester */
      { "NM", +35.117218, -106.624636 }, /* Albuquerque */
      { "NV", +36.205750, -115.222799 }, /* Las Vegas */
      { "NY", +40.669800, -73.943849 },  /* New York */
      { "NC", +35.197550, -80.834514 },  /* Charlotte */
      { "ND", +46.805467, -100.767298 }, /* Bismarck */
      { "OH", +39.988933, -82.987381 },  /* Columbus */
      { "OK", +35.467050, -97.513491 },  /* Oklahoma City */
      { "OR", +45.538250, -122.656496 }, /* Portland */
      { "PA", +40.006817, -75.134678 },  /* Philadelphia */
      { "RI", +41.821950, -71.419732 },  /* Providence */
      { "SC", +34.039236, -80.886341 },  /* Columbia */
      { "SD", +44.372982, -100.322483 }, /* Pierre */
      { "TN", +35.105600, -90.006991 },  /* Memphis */
      { "TX", +29.768700, -95.386728 },  /* Houston */
      { "UT", +40.777267, -111.929921 }, /* Salt Lake City */
      { "VT", +44.488093, -73.226177 },  /* Burlington */
      { "VA", +37.531050, -77.474584 },  /* Richmond */
      { "WA", +47.621800, -122.350326 }, /* Seattle */
      { "WV", +38.350550, -81.630439 },  /* Charleston */
      { "WI", +43.079800, -89.387519 },  /* Madison */
      { "WY", +41.145450, -104.792349 }, /* Cheyenne */
      { "PR", +18.408386, -66.064425 },  /* San Juan */
      { 0, 0, 0 }
    };

    char *buf = (char *)recInterp->GetBuf();
    char *state = buf + attrOffset[0];
    int i = 0;
    while(StateLatLon[i].state && strcmp(StateLatLon[i].state, state))
      i++;

    if (_warning && !StateLatLon[i].state) {
      fprintf(stderr,
	      "Warning: Data has unrecognized state codes, including %s\n",
	      state);
      _warning = false;
    }

    float *latPtr = (float *)(buf + attrOffset[2]);
    *latPtr = StateLatLon[i].latitude;
    float *lonPtr = (float *)(buf + attrOffset[3]);
    *lonPtr = StateLatLon[i].longitude;

    float *totalAmount = (float *)(buf + attrOffset[1]);
    float *totalPtr = (float *)(buf + attrOffset[4]);
    *totalPtr = *totalAmount / 1500.0;

    if (!latAttr->hasHiVal || *latPtr > latAttr->hiVal.floatVal) {
      latAttr->hiVal.floatVal = *latPtr;
      latAttr->hasHiVal = true;
    }
    if (!lonAttr->hasLoVal || *lonPtr < lonAttr->loVal.floatVal) {
      lonAttr->loVal.floatVal = *lonPtr;
      lonAttr->hasLoVal = true;
    }
    if (!totalAttr->hasLoVal || *totalPtr < totalAttr->loVal.floatVal) {
      totalAttr->loVal.floatVal = *totalPtr;
      totalAttr->hasLoVal = true;
    }
  }

private:
  int       *attrOffset;          /* attribute offsets */
  AttrInfo  *latAttr;             /* latitude attribute info */
  AttrInfo  *lonAttr;             /* longitude attribute info */
  AttrInfo  *totalAttr;           /* total amount attribute info */
  Boolean   _init;                /* true when instance initialized */
  Boolean   _warning;             /* true when warning should be displayed
				     for unknown state codes */
};

QueryProc *genQueryProcTape()
{
  return new QueryProcTape;
}

main(int argc, char **argv)
{
  Init::DoInit(argc,argv);

  /* Register composite parsers */
  CompositeParser::Register("BEST_STOCK", new YyMmDdComposite);
  CompositeParser::Register("MIT_STOCK", new YyMmDdComposite);
  CompositeParser::Register("ISSM-Trade", new ObsDateComposite);
  CompositeParser::Register("ISSM-Quote", new ObsDateComposite);
  CompositeParser::Register("DOL_DATA", new DOLDateComposite);
  CompositeParser::Register("DOWJONES", new MmDdYyComposite);
  CompositeParser::Register("LANDSEND", new StateLatLonComposite);

#if 0
  /* Register parser for tape */
  RegisterGenClassInfo("tape", new ISSMGenClass());
#endif

  /* Register known query processors */
  QueryProc::Register("Tape", genQueryProcTape);

  /* Register known classes  with control panel */
  ControlPanel::RegisterClass(new TileLayoutInfo);
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo());
  ControlPanel::RegisterClass(new CursorClassInfo());

  /* hack to start control panel so that it'll read the RC files */

  ControlPanel *ctrl = ControlPanel::Instance();

  /* This is a hack to create a display before running Dispatcher.
     Otherwise, we'll get an error */

  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();

  /* keep compiler happy */
  disp = disp;

  /* Start session (possibly restoring an old one */
  ctrl->StartSession();

  Dispatcher::RunNoReturn();
}
