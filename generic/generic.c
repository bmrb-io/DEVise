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
  Revision 1.26  1996/06/07 19:36:15  wenger
  Further improvements to the composite parser for the fourth set of
  IBM traces.

  Revision 1.25  1996/06/06 21:43:25  wenger
  Added composite attributes for each type of miss in IBM trace 4 composite
  parser.

  Revision 1.24  1996/06/06 16:59:06  wenger
  Added a new composite parser for the fourth set of IBM traces.

  Revision 1.23  1996/05/22 21:06:24  jussi
  ControlPanel::_controlPanel is now set by main program.

  Revision 1.22  1996/05/11 03:28:20  jussi
  Added association of YyMmDd composite parser with the CRSP schema.

  Revision 1.21  1996/05/07 16:48:59  jussi
  Computation of attribute hi/lo values now done in the TData interpreter
  *after* the composite attribute values are computed.

  Revision 1.20  1996/05/06 16:23:03  jussi
  IBMAddressTraceComposite now stores the reference tag in addition
  to storing the color of the tag. Added the association of this
  composite parser with schemas IBMTRACE1 through IBMTRACE3.

  Revision 1.19  1996/05/05 03:06:41  jussi
  Added IBMAddressTraceCompositeParser. Added missing attribute
  high/low value computation for some composite parsers.

  Revision 1.18  1996/04/30 15:53:53  jussi
  Added link between StateLatLon parser and schema MARKETING.

  Revision 1.17  1996/04/23 20:33:14  jussi
  Added LatLonComposite parser which translates integer
  latitude/longitude numbers (in millionths of degrees)
  to floating-point numbers.

  Revision 1.16  1996/04/23 14:03:06  jussi
  Refined StateLatLon composite parser. It now disperses multiple
  records from the same state with some randomness, forming a
  cloud. Also added the Color and TotalAmountDegree fields.

  Revision 1.15  1996/04/22 22:03:24  jussi
  Added StateLatLon composite parser and associated the LANDSEND
  schema with it.

  Revision 1.14  1996/04/19 17:20:11  wenger
  Put the GenClassInfo code back in -- this is needed for tape data;
  started adding the tape-related code back in (it was previously
  deleted for some reason; I'm not yet done adding it back); added
  the 'DEVise parseSchema' command and the first parts of the code
  related to it.

  Revision 1.13  1996/04/16 21:00:14  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.12  1996/04/04 05:16:19  kmurli
  No apparent modification done to generic.c. Only added printf commands
  to test and later removed them.

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
#include <math.h>
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
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for dates of the form MMDDYY */

class MmDdYyComposite : public UserComposite {
public:

  MmDdYyComposite() {
    _init = false;
    attrOffset = 0;
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
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for OBSDATE field */

class ObsDateComposite : public UserComposite {
public:

  ObsDateComposite() {
    _init = false;
    attrOffset = 0;
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
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for Department of Labor date fields */

class DOLDateComposite : public UserComposite {
public:

  DOLDateComposite() {
    _init = false;
    attrOffset = 0;
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
  }

private:
  int       *attrOffset;          /* attribute offsets */
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
  }

  virtual ~StateLatLonComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "State", "TotalAmount",
			    "Latitude", "Longitude", "TotalAmountDegree",
			    "Color" };
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

    // fuzz up the picture a little by creating a cloud

    float length = (rand() % 300) / 100.0;
    float dir = (rand() % 360) / 360.0 * 2 * 3.14;
    *latPtr += length * sin(dir);
    *lonPtr += length * cos(dir);

    float *totalAmount = (float *)(buf + attrOffset[1]);
    float *totalPtr = (float *)(buf + attrOffset[4]);
    *totalPtr = 0.2 + *totalAmount / 8000.0;

    int *colorPtr = (int *)(buf + attrOffset[5]);
    if (*totalAmount < 250)
      *colorPtr = RedColor;
    else if (*totalAmount < 500)
      *colorPtr = GreenColor;
    else if (*totalAmount < 1000)
      *colorPtr = BlueColor;
    else if (*totalAmount < 2500)
      *colorPtr = OrangeColor;
    else if (*totalAmount < 5000)
      *colorPtr = PurpleColor;
    else if (*totalAmount < 10000)
      *colorPtr = LightGrayColor;
    else if (*totalAmount < 20000)
      *colorPtr = AzureColor;
    else if (*totalAmount < 30000)
      *colorPtr = BlackColor;
    else
      *colorPtr = KhakiColor;
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
  Boolean   _warning;             /* true when warning should be displayed
				     for unknown state codes */
};

/*
   User composite function for geographical locations that translates
   integer latitude/longitude numbers, measured in millionths of
   degrees, to floating point numbers.
*/

class LatLonComposite : public UserComposite {
public:

  LatLonComposite() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~LatLonComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "LatitudeInt", "LongitudeInt", "Latitude",
			    "Longitude" };
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
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    int   *latIntPtr = (int *)(buf + attrOffset[0]);
    int   *lonIntPtr = (int *)(buf + attrOffset[1]);
    float *latPtr = (float *)(buf + attrOffset[2]);
    float *lonPtr = (float *)(buf + attrOffset[3]);

    *latPtr = *latIntPtr / 1e6;
    *lonPtr = *lonIntPtr / 1e6;
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/*
   User composite function for address traces on some IBM architecture.
   Each address reference has a 62-bit address part and a 2-bit type
   tag (instruction reference, load reference, or store reference).
   We convert the 6 highest bits of the address to Y, the next 6 bits
   to X, and ignore the remaining bits. A randomizing is performed
   to create a cloud of address references at X,Y. Type tag is mapped
   to a color (red = instruction, green = load, blue = store).
*/

class IBMAddressTraceComposite : public UserComposite {
public:

  IBMAddressTraceComposite() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~IBMAddressTraceComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "Address", "RecNum", "Tag", "X", "Y", "Color" };
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
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    char *address = buf + attrOffset[0];
    int *recPtr = (int *)(buf + attrOffset[1]);
    int *tagPtr = (int *)(buf + attrOffset[2]);
    float *XPtr = (float *)(buf + attrOffset[3]);
    float *YPtr = (float *)(buf + attrOffset[4]);
    int *colorPtr = (int *)(buf + attrOffset[5]);

    // Record number is passed to us via the record interpreter
    *recPtr = recInterp->GetRecPos();

    // The high 32 bits are first, followed by the low 32 bits.
    // Randomize the picture a little by creating a square cloud.

#ifdef GRANULARITY_64
    *YPtr = (address[0] & 0xfc) >> 2;
    *XPtr = ((address[0] & 0x03) << 4) | ((address[1] & 0xf0) >> 4);
    int cloudSize = 4;
#else
    *YPtr = (unsigned char)address[0];
    *XPtr = (unsigned char)address[1];
    int cloudSize = 8;
#endif
    *YPtr += (rand() % (cloudSize * 100)) / 100.0 - cloudSize / 2;
    *XPtr += (rand() % (cloudSize * 100)) / 100.0 - cloudSize / 2;

    // The lowest 2 bits are the tag.

    *tagPtr = address[7] & 0x03;
    *colorPtr = 0;
    if (*tagPtr == 3)                   // instruction reference?
      *colorPtr = 2;                    // make it red
    else if (*tagPtr == 1)              // load reference?
      *colorPtr = 6;                    // make it green
    else if (*tagPtr == 2)              // store reference?
      *colorPtr = 3;                    // make it blue
    else
      fprintf(stderr, "Invalid tag in trace: %d\n", *tagPtr);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};


/*
   Second user composite function for address traces on some IBM
   architecture.  Each address reference has a 64-bit address part
   and a 32-bit reference that tells whether it's an instruction,
   load, or store, and any cache misses that happened.
   We convert the 6 highest bits of the address to Y, the next 6 bits
   to X, and ignore the remaining bits. A randomizing is performed
   to create a cloud of address references at X,Y. Type tag is mapped
   to a color (red = instruction, green = load, blue = store).
*/

class IBMAddressTraceComposite2 : public UserComposite {
public:
  IBMAddressTraceComposite2() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~IBMAddressTraceComposite2() {
    delete attrOffset;
  }

  enum ref_type {
    SIM_NO_MISS=0,
    SIM_ITLB_IR_MISS=1, /* I-ref causes ITLB miss. */
    SIM_DTLB_IR_MISS=2,     /* I-ref causes DTLB miss. */
    SIM_DTLB_DR_MISS=4, /* D-ref causes DTLB miss. */

    SIM_IC_IR_MISS=8,   /* I-ref causes L1-Icache miss. */
    SIM_DC_IR_MISS=16,  /* I-ref causes L1-Dcache miss. */
    SIM_DC_DR_MISS=32,  /* D-ref causes L1-Dcache miss. */

    SIM_L2_IR_MISS=64,  /* I-ref causes L2cache miss. */
    SIM_L2_DR_MISS=128,     /* D-ref causes L2cache miss. */
    SIM_L2_CO_MISS=256, /* L1 Castout causes L2cache miss. */

    SIM_IREF=(1<<16),   /* This is an I-ref. */
    SIM_LREF=(2<<16),   /* This is a data load ref. */
    SIM_SREF=(4<<16)    /* This is a data store ref. */
  };


  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "Address", "Ref", "RecNum", "Tag", "X", "Y",
        "Color", "Misses", "L2miss", "ICmiss", "DCmiss", "ITmiss", "DTmiss",
		"COmiss", "TotalMisses", "HasMisses" };
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
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    unsigned char *address = (unsigned char *) buf + attrOffset[0];
    unsigned char *refP = (unsigned char *) buf + attrOffset[1];
    int *recPtr = (int *)(buf + attrOffset[2]);
    int *tagPtr = (int *)(buf + attrOffset[3]);
    float *XPtr = (float *)(buf + attrOffset[4]);
    float *YPtr = (float *)(buf + attrOffset[5]);
    int *colorPtr = (int *)(buf + attrOffset[6]);
    int *missesP = (int *)(buf + attrOffset[7]);
    int *L2missP = (int *) (buf + attrOffset[8]);
    int *ICmissP = (int *) (buf + attrOffset[9]);
    int *DCmissP = (int *) (buf + attrOffset[10]);
    int *ITmissP = (int *) (buf + attrOffset[11]);
    int *DTmissP = (int *) (buf + attrOffset[12]);
    int *COmissP = (int *) (buf + attrOffset[13]);
    int *totalMissesP = (int *) (buf + attrOffset[14]);
    int *hasMissesP = (int *) (buf + attrOffset[15]);

    // Record number is passed to us via the record interpreter
    *recPtr = recInterp->GetRecPos();

    // The high 32 bits are first, followed by the low 32 bits.
    // Randomize the picture a little by creating a square cloud.

#ifdef GRANULARITY_64
    *YPtr = (address[0] & 0xfc) >> 2;
    *XPtr = ((address[0] & 0x03) << 4) | ((address[1] & 0xf0) >> 4);
    int cloudSize = 4;
#else
    *YPtr = address[0];
    *XPtr = address[1];
    int cloudSize = 8;
#endif
    *YPtr += (rand() % (cloudSize * 100)) / 100.0 - cloudSize / 2;
    *XPtr += (rand() % (cloudSize * 100)) / 100.0 - cloudSize / 2;

    // The the second byte of the reference tells what kind it is.

    *tagPtr = refP[1] & 0x7;
    *colorPtr = 0;
    if (*tagPtr == 1)                   // instruction reference?
    {
      *colorPtr = 2;                    // make it red
    }
    else if (*tagPtr == 2)              // load reference?
    {
      *colorPtr = 6;                    // make it green
    }
    else if (*tagPtr == 4)              // store reference?
    {
      *colorPtr = 3;                    // make it blue
    }
    else
    {
      fprintf(stderr, "Invalid tag in trace: %d (record %d)\n", *tagPtr,
        *recPtr);
    }

    // The third and fourth bytes tell us the cache miss, if any.
    *missesP = refP[2] << 8 | refP[3];

    *L2missP = 0;
    *ICmissP = 0;
    *DCmissP = 0;
    *ITmissP = 0;
    *DTmissP = 0;
    *COmissP = 0;
    *totalMissesP = 0;
    *hasMissesP = 0;

    if (*missesP & SIM_ITLB_IR_MISS)
    {
	*ITmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_DTLB_IR_MISS)
    {
	*DTmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_DTLB_DR_MISS)
    {
	*DTmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_IC_IR_MISS)
    {
	*ICmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_DC_IR_MISS)
    {
	*DCmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_DC_DR_MISS)
    {
	*DCmissP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_L2_IR_MISS)
    {
	*L2missP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_L2_DR_MISS)
    {
	*L2missP = 1;
	(*totalMissesP)++;
    }

    if (*missesP & SIM_L2_CO_MISS)
    {
	*COmissP = 1;
	(*totalMissesP)++;
    }

    if (*totalMissesP > 0) *hasMissesP = 1;

    //printf("%d: %02x %02x  %02x %04x\n", *recPtr, address[0], address[1], *tagPtr, *missesP);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};


QueryProc *genQueryProcTape()
{
  return new QueryProcTape;
}

int main(int argc, char **argv)
{
  Init::DoInit(argc,argv);

  /* Register composite parsers */
  CompositeParser::Register("BEST_STOCK", new YyMmDdComposite);
  CompositeParser::Register("MIT_STOCK", new YyMmDdComposite);
  CompositeParser::Register("CRSP-Schema", new YyMmDdComposite);
  CompositeParser::Register("ISSM-Trade", new ObsDateComposite);
  CompositeParser::Register("ISSM-Quote", new ObsDateComposite);
  CompositeParser::Register("DOL_DATA", new DOLDateComposite);
  CompositeParser::Register("DOWJONES", new MmDdYyComposite);
  CompositeParser::Register("LANDSEND", new StateLatLonComposite);
  CompositeParser::Register("MARKETING", new StateLatLonComposite);
  CompositeParser::Register("CENSUS_PLACES", new LatLonComposite);
  CompositeParser::Register("CENSUS_ZIP", new LatLonComposite);
  CompositeParser::Register("IBMTRACE", new IBMAddressTraceComposite);
  CompositeParser::Register("IBMTRACE1", new IBMAddressTraceComposite);
  CompositeParser::Register("IBMTRACE2", new IBMAddressTraceComposite);
  CompositeParser::Register("IBMTRACE3", new IBMAddressTraceComposite);
  CompositeParser::Register("IBMTRACE.2.all", new IBMAddressTraceComposite2);
  CompositeParser::Register("IBMTRACE.2.ir", new IBMAddressTraceComposite2);
  CompositeParser::Register("IBMTRACE.2.ld", new IBMAddressTraceComposite2);
  CompositeParser::Register("IBMTRACE.2.st", new IBMAddressTraceComposite2);
  CompositeParser::Register("IBMTRACE.2.miss", new IBMAddressTraceComposite2);

  /* Register known query processors */
  QueryProc::Register("Tape", genQueryProcTape);

  /* Register known classes  with control panel */
  ControlPanel::RegisterClass(new TileLayoutInfo);
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo());
  ControlPanel::RegisterClass(new CursorClassInfo());

  /* hack to start control panel so that it'll read the RC files */
  ControlPanel::_controlPanel = GetNewControl();
  ControlPanel *ctrl = ControlPanel::Instance();

  /* This is a hack to create a display before running Dispatcher.
     Otherwise, we'll get an error */

  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();

  /* keep compiler happy */
  disp = disp;

  /* Start session (possibly restoring an old one */
  ctrl->StartSession();

  Dispatcher::RunNoReturn();

  return 1;
}
