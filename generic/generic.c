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
  Revision 1.52  1998/02/19 23:24:36  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.51  1997/12/12 05:50:15  weaver
  *** empty log message ***

  Revision 1.50  1997/11/24 23:14:06  weaver
  Changes for the new ColorManager.

  Revision 1.49  1997/05/21 22:05:08  andyt
  *** empty log message ***

  Revision 1.48.6.1  1997/05/21 20:39:26  weaver
  Changes for new ColorManager

  Revision 1.48  1997/04/18 17:52:32  wenger
  Argh!  Forgot to remove debug output.

  Revision 1.47  1997/04/17 15:05:11  wenger
  Added ISODate2Composite parser for data with _two_ ISO 8601 dates per
  record (geez, these composite parsers are a pain!).

  Revision 1.46  1997/04/16 16:22:37  wenger
  Added composite parser for ISO 8601 date format (YYYY-MM-DD hh:mm:ss).

  Revision 1.45  1997/03/25 17:58:37  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.44  1997/02/26 16:31:04  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.43  1997/02/03 19:46:31  ssl
  Added new class ViewLensInfo

  Revision 1.42.4.3  1997/03/06 19:44:49  wenger
  Fixed various parsing bugs, improved error messages as a result of
  Miron's problems with the GIS data: warns if strings are too long;
  warns if improper separator/whitespace specification; better warnings
  if records don't parse; better error messages from
  MmDdYyHhMmAmPmComposite parser.

  Revision 1.42.4.2  1997/02/27 21:20:44  jussi
  Added MmDdYyHhMmAmPmComposite parser.

  Revision 1.42.4.1  1997/02/14 23:29:00  wenger
  Fixed off-by-one-hour error in YyDdd_HhMmComposite composite parser;
  fixed another bug in point queries.

  Revision 1.42  1997/01/30 19:48:28  jussi
  Moved state map hash table to StateMap.C. Added init of string table.

  Revision 1.41  1996/12/04 17:53:02  wenger
  Date composite parsers call mktime() for every record to fix nasty
  and subtle DST bugs; also check to make sure they're stuffing the
  date into an attribute that _is_ a date type.

  Revision 1.40  1996/12/02 18:44:21  wenger
  Fixed problems dealing with DST in dates (including all date composite
  parsers); added more error checking to date composite parsers.

  Revision 1.39  1996/11/23 20:44:26  jussi
  Removed references to QueryProcTape.

  Revision 1.38  1996/11/01 19:04:32  kmurli
  Makefile changed to include DQL sources

  Revision 1.37  1996/10/23 14:55:20  wenger
  Renamed LandsendDateDiffComposite composite parser to
  MailorderDateDiffComposite as part of getting rid of the "Land's
  End" name in visible places; schema types changed from "LANDSEND..."
  to "MAILORDER...".

  Revision 1.36  1996/10/04 18:07:35  guangshu
  Some improvement in openning composite.ini.

  Revision 1.35  1996/09/16 21:30:49  wenger
  Added YyDdd_HhMmComposite composite parser for dates of the form YYDDD, HHMM
  (needed for Soil Science).

  Revision 1.34  1996/09/05 23:13:44  kmurli
  Generic.c modified to read composite.ini from the DEVISE_LIB directory. (DEVISE_LIB env variable ) to register the composite parsers for various schema
  CVS ----------------------------------------------------------------------

  Revision 1.33  1996/08/29 21:58:16  guangshu
  Added several DATE type schema.

  Revision 1.32  1996/08/13 20:04:38  jussi
  Added DayOfYearComposite parser function.

  Revision 1.31  1996/07/23 22:58:22  jussi
  Added BIRCHLE0.

  Revision 1.30  1996/07/23 15:36:11  jussi
  Added LandsendDateDiffComposite parser.

  Revision 1.29  1996/07/21 14:18:39  jussi
  Mapping from state names to lat/lon coordinates is now through
  a hash table.

  Revision 1.28  1996/07/21 01:15:57  jussi
  LANDSENDDAILY schema uses the YyMmDd composite parser.

  Revision 1.27  1996/07/09 15:59:42  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

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
#include <errno.h>
#include <time.h>
#include <string.h>
#ifndef SGI
#include <math.h>
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <String.h>
#include <fstream.h>

#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "Display.h"
#include "Control.h"
#include "Init.h"
#include "WinClassInfo.h"
#include "MapInterpClassInfo.h"
#include "VisualLinkClassInfo.h"
#include "ViewClassInfo.h"
#include "CompositeParser.h"
#include "RecInterp.h"
#include "CursorClassInfo.h"
#include "StringStorage.h"
#include "DevError.h"
#include "StateMap.h"
#include "Util.h"

static time_t GetTime(struct tm &now)
{
  /* Check input values. */
  {
    char errBuf[1024];

    /* tm_isdst of -1 tells mktime to not adjust the time (we get the
     * time we specified). */
    if (now.tm_isdst != -1) {
      sprintf(errBuf, "Illegal daylight savings flag; set to false");
      reportErrNosys(errBuf);
      now.tm_isdst = -1;
    }

    if (now.tm_mday < 1 || now.tm_mday > 366) {
      sprintf(errBuf, "Illegal day value %d; set to 1", now.tm_mday);
      reportErrNosys(errBuf);
      now.tm_mday = 1;
    }

    if (now.tm_hour < 0 || now.tm_hour > 24) {
      sprintf(errBuf, "Illegal hour value %d; set to 0", now.tm_hour);
      reportErrNosys(errBuf);
      now.tm_hour = 0;
    }

    if (now.tm_mon < 0 || now.tm_mon > 11) {
      sprintf(errBuf, "Illegal month value %d; set to 0 (January)",
	now.tm_mon);
      reportErrNosys(errBuf);
      now.tm_mon = 0;
    }

    if (now.tm_year < 70) {
#if 0
      sprintf(errBuf, "Illegal year value %d; set to (19)70", now.tm_year);
      reportErrNosys(errBuf);
#endif
      now.tm_sec = 0;
      now.tm_min = 0;
      now.tm_hour = 0;
      now.tm_mday = 1;
      now.tm_mon = 0;
      now.tm_year = 70;
    }
  }

  static struct tm lasttm;
  static time_t    lasttime = (time_t)-1;

  /* see if we can reuse the time value from last call's
     mktime() call; mktime costs 1 msec per call so we
     don't want to call it too often */
  
  time_t nowtime = 0;

/* Calling mktime() every time is the easiest way to get DST to work out
 * correctly.  We probably eventually want to restore the code to not
 * always call mktime(), but this is the quickest way to get things to
 * be _correct_, even if slower.  RKW 12/4/96. */
#if 0
  if (lasttime != (time_t)-1 && now.tm_year == lasttm.tm_year
      && now.tm_mon == lasttm.tm_mon) {
#else
  if (false) {
#endif
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

  if (nowtime == -1) {
    reportErrNosys("Requested time cannot be represented");
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
	if (!strcmp(info->name, "DATE") && (info->type != DateAttr)) {
	  reportErrNosys("Type of DATE attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    int seq = *(int *)(buf + attrOffset[0]);
    now.tm_mday = seq % 100;
    now.tm_mon = (seq / 100) % 100 - 1;
    now.tm_year = seq / 10000;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;
    now.tm_isdst = -1;

    time_t *datePtr = (time_t *)(buf + attrOffset[1]);
    *datePtr = GetTime(now);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for timestamps of the form MM-DD-YY HH:MM AM/PM */

class MmDdYyHhMmAmPmComposite : public UserComposite {
public:
  MmDdYyHhMmAmPmComposite() {
    _init = false;
    attrOffset = 0;
  }
    
  virtual ~MmDdYyHhMmAmPmComposite() {
    delete attrOffset;
  }
    
  virtual void Decode(RecInterp *recInterp) {
        
    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */
            
      char *primAttrs[] = { "MMDDYY", "HHMM", "AMPM", "DATE" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");
            
      for(int i = 0; i < numPrimAttrs; i++) {
        AttrInfo *info;
        if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
          fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
          DOASSERT(0, "Cannot find attribute");
        }
        if (!strcmp(info->name, "DATE") && (info->type != DateAttr)) {
          reportErrNosys("Type of DATE attribute is not date");
        }
        attrOffset[i] = info->offset;
      }
      _init = true;
    }
        
    char *buf = (char *)recInterp->GetBuf();
        
    /* decode date */
    time_t *datePtr = (time_t *)(buf + attrOffset[3]);
    char *dash1 = strchr(buf + attrOffset[0], '-');
    char *dash2 = (!dash1 ? (char*)NULL : strchr(dash1 + 1, '-'));
    char *colon = strchr(buf + attrOffset[1], ':');
    char ampm = *(buf + attrOffset[2]);
    if (islower(ampm))
      ampm = toupper(ampm);
    if (!isdigit(buf[attrOffset[0]]) || !dash1 || !isdigit(dash1[1]) ||
        !dash2 || !isdigit(dash2[1]) || !isdigit(buf[attrOffset[1]]) ||
        !colon || !isdigit(colon[1]) || (ampm != 'A' && ampm != 'P')) {
      reportErrNosys("Timestamp not of the form MM-DD-YY HH:MM AM/PM");
      fprintf(stderr, "  values: <%s> <%s> <%s>\n", &buf[attrOffset[0]],
	&buf[attrOffset[1]], &buf[attrOffset[2]]);
      *datePtr = time(0);
    } else {
      struct tm now;
      now.tm_mday = atoi(dash1 + 1);
      now.tm_mon = atoi(buf + attrOffset[0]) - 1;
      now.tm_year = atoi(dash2 + 1);
      now.tm_hour = atoi(buf + attrOffset[1]);
      if (ampm == 'P' && now.tm_hour < 12)
        now.tm_hour += 12;
      if (ampm == 'A' && now.tm_hour == 12)
        now.tm_hour = 0;
      now.tm_min = atoi(colon + 1);
      now.tm_sec = 0;
      now.tm_isdst = -1;
      *datePtr = GetTime(now);
    }
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
	if (!strcmp(info->name, "DATE") && (info->type != DateAttr)) {
	  reportErrNosys("Type of DATE attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    now.tm_mday = *(int *)(buf + attrOffset[1]);
    now.tm_mon = *(int *)(buf + attrOffset[0]) - 1;
    now.tm_year = *(int *)(buf + attrOffset[2]) - 1900;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;
    now.tm_isdst = -1;

    time_t *datePtr = (time_t *)(buf + attrOffset[3]);
    *datePtr = GetTime(now);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for dates of the form DayOfYear, Hour */

class DayOfYearComposite : public UserComposite {
public:

  DayOfYearComposite() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~DayOfYearComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "DayOfYear", "Time", "Date" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	if (!strcmp(info->name, "Date") && (info->type != DateAttr)) {
	  reportErrNosys("Type of Date attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    now.tm_mday = *(int *)(buf + attrOffset[0]);
    now.tm_mon = 0;
    now.tm_year = 1996 - 1900;
    float hour = *(float *)(buf + attrOffset[1]);
    now.tm_hour = (int)hour;
    now.tm_min = 60 * ((int)(hour - (int)hour));
    now.tm_sec = 0;
    now.tm_isdst = -1;

    time_t *datePtr = (time_t *)(buf + attrOffset[2]);
    *datePtr = GetTime(now);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for dates of the form YYDDD, HHMM */
/* YY (19)70 - ?
 * DDD 001 - 366
 * HH 00 - 24 (0000 is midnight at beginning of day; 2400 is end)
 * MM 00 - 59 */

class YyDdd_HhMmComposite : public UserComposite {
public:

  YyDdd_HhMmComposite() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~YyDdd_HhMmComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "YYDDD", "HHMM", "Date" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	if (!strcmp(info->name, "Date") && (info->type != DateAttr)) {
	  reportErrNosys("Type of Date attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();

    /* decode date */
    float yyddd = *(float *)(buf + attrOffset[0]);
    float hhmm = *(float *)(buf + attrOffset[1]);
    // 0.1 are for safety in case of rounding down.
    int year = int((yyddd / 1000.0) + 0.1);
    int day = int((yyddd - 1000.0 * year) + 0.1);
    int hour = int((hhmm / 100.0) + 0.1);
    int minute = int((hhmm - 100.0 * hour) + 0.1);

    struct tm now;
    memset((char *) &now, 0, sizeof(now));
    now.tm_mday = day;
    now.tm_mon = 0;
    now.tm_year = year;
    now.tm_hour = hour;
    now.tm_min = minute;
    now.tm_sec = 0;
    now.tm_isdst = -1;

    time_t *datePtr = (time_t *)(buf + attrOffset[2]);
    *datePtr = GetTime(now);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

void ISODateDecode(char *recBuf, int dateOffset, int timeOffset,
    int timestampOffset)
{
  struct tm now;
  memset(&now, 0, sizeof(now));

  /* Get year, month, day. */
  if (sscanf(recBuf + dateOffset, "%d-%d-%d", &now.tm_year, &now.tm_mon,
      &now.tm_mday) != 3) {
    char errBuf[256];
    sprintf(errBuf, "Improper date string: <%s>", recBuf + dateOffset);
    reportErrNosys(errBuf);

    now.tm_year = 1970;
    now.tm_mon = 1;
    now.tm_mday = 1;
  }
  now.tm_year -= 1900;
  now.tm_mon -= 1;

  /* Get hour, minute, second. */
  if (sscanf(recBuf + timeOffset, "%d:%d:%d", &now.tm_hour, &now.tm_min,
      &now.tm_sec) != 3) {
    /* Try without seconds. */
    if (sscanf(recBuf + timeOffset, "%d:%d", &now.tm_hour,
        &now.tm_min) != 2) {
      char errBuf[256];
      sprintf(errBuf, "Improper time string: <%s>", recBuf + timeOffset);
      reportErrNosys(errBuf);

      now.tm_hour = 0;
      now.tm_min = 0;
      now.tm_sec = 0;
    } else {
      now.tm_sec = 0;
    }
  }

  now.tm_isdst = -1;

  time_t *datePtr = (time_t *)(recBuf + timestampOffset);
  *datePtr = GetTime(now);
}

/*
 * User composite function for ISO 8601 date format.
 * This format is: YYYY-MM-DD hh:mm:ss
 * YYYY is 1970-2037
 * MM is 01-12
 * DD is 01-31
 * hh is 00-24
 * mm is 00-59
 * ss is 00-60 (60 only for leap seconds)
 * (24:00:00 is the only legal value for hours == 24.)
 * (Seconds are optional in this composite parser.)
 */
class ISODateComposite : public UserComposite {
public:
  ISODateComposite() {
    _init = false;
    _attrOffset = 0;
  }

  virtual ~ISODateComposite() {
    delete _attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {
    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "Date", "Time", "Timestamp" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      _attrOffset = new int [numPrimAttrs];
      DOASSERT(_attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  char errBuf[128];
	  sprintf(errBuf, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, errBuf);
	}
	if (!strcmp(info->name, "Timestamp") && (info->type != DateAttr)) {
	  reportErrNosys("Type of Timestamp attribute is not date");
	}
	_attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *) recInterp->GetBuf();

    /* decode timestamp */
    ISODateDecode(buf, _attrOffset[0], _attrOffset[1], _attrOffset[2]);
  }

private:
  int       *_attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/*
 * User composite function for _two_ dates of ISO 8601 date format.
 * This format is: YYYY-MM-DD hh:mm:ss
 * YYYY is 1970-2037
 * MM is 01-12
 * DD is 01-31
 * hh is 00-24
 * mm is 00-59
 * ss is 00-60 (60 only for leap seconds)
 * (24:00:00 is the only legal value for hours == 24.)
 * (Seconds are optional in this composite parser.)
 */
class ISODate2Composite : public UserComposite {
public:
  ISODate2Composite() {
    _init = false;
    _attrOffset = 0;
  }

  virtual ~ISODate2Composite() {
    delete _attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {
    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "Date1", "Time1", "Timestamp1", "Date2",
	"Time2", "Timestamp2" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      _attrOffset = new int [numPrimAttrs];
      DOASSERT(_attrOffset, "Out of memory");

      int i;
      for(i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  char errBuf[128];
	  sprintf(errBuf, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, errBuf);
	  break;
	}
	if ((!strcmp(info->name, "Timestamp1") ||
	    !strcmp(info->name, "Timestamp2")) && (info->type != DateAttr)) {
	  char errBuf[128];
	  sprintf(errBuf, "Type of %s attribute is not date", info->name);
	  reportErrNosys(errBuf);
	}
	_attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *) recInterp->GetBuf();

    /* decode timestamps */
    ISODateDecode(buf, _attrOffset[0], _attrOffset[1], _attrOffset[2]);
    ISODateDecode(buf, _attrOffset[3], _attrOffset[4], _attrOffset[5]);
  }

private:
  int       *_attrOffset;	/* attribute offsets */
  Boolean   _init;		/* true when instance initialized */
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
	if (!strcmp(info->name, "DATE") && (info->type != DateAttr)) {
	  reportErrNosys("Type of DATE attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    now.tm_mday = *(int *)(buf + attrOffset[0]);
    now.tm_mon = *(int *)(buf + attrOffset[1]) - 1;
    now.tm_year = *(int *)(buf + attrOffset[2]) - 1900;
    now.tm_hour = *(int *)(buf + attrOffset[3]);
    now.tm_min = *(int *)(buf + attrOffset[4]);
    now.tm_sec = *(int *)(buf + attrOffset[5]);
    now.tm_isdst = -1;

    time_t *datePtr = (time_t *)(buf + attrOffset[6]);
    *datePtr = GetTime(now);
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
};

/* User composite function for dates computed as the number of days
   from October 1, 1992 (that's when the Lands' End date begins) */

class MailorderDateDiffComposite : public UserComposite {
public:

  MailorderDateDiffComposite() {
    _init = false;
    attrOffset = 0;
  }

  virtual ~MailorderDateDiffComposite() {
    delete attrOffset;
  }

  virtual void Decode(RecInterp *recInterp) {

    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */

      char *primAttrs[] = { "DateDiff", "Date" };
      const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];
      attrOffset = new int [numPrimAttrs];
      DOASSERT(attrOffset, "Out of memory");

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "Cannot find attribute %s\n", primAttrs[i]);
	  DOASSERT(0, "Cannot find attribute");
	}
	if (!strcmp(info->name, "Date") && (info->type != DateAttr)) {
	  reportErrNosys("Type of Date attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    float dateDiff = *(float *)(buf + attrOffset[0]);
    int years = (int)(dateDiff / 360);
    int months = (int)((dateDiff - years * 360) / 30);
    int days = (int)(dateDiff - years * 360 - months * 30);
    float secDiff = 24 * 3600 * (dateDiff - (int)dateDiff);
    int hours = (int)(secDiff / 3600);
    int minutes = (int)((secDiff - hours * 3600) / 60);
    int seconds = (int)(secDiff - hours * 3600 - minutes * 60);
    now.tm_year = 1992 + years - 1900;
    now.tm_mon = 10 + months - 1;
    now.tm_mday = 1 + days;
    if (now.tm_mon > 11) {
      now.tm_year++;
      now.tm_mon -= 12;
    }
    now.tm_hour = hours;
    now.tm_min = minutes;
    now.tm_sec = seconds;
    now.tm_isdst = -1;
    time_t *datePtr = (time_t *)(buf + attrOffset[1]);
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
	if (!strcmp(info->name, "DATE") && (info->type != DateAttr)) {
	  reportErrNosys("Type of Date attribute is not date");
	}
	attrOffset[i] = info->offset;
      }
      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    
    /* decode date */
    struct tm now;
    now.tm_mday = 1;
    now.tm_mon = atoi(buf + attrOffset[1] + 1) - 1;
    now.tm_year = *(int *)(buf + attrOffset[0]) - 1900;
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;
    now.tm_isdst = -1;

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

static void InitStateMap()
{
  /*
     Translate state name to the latitude/longitude of the state capital
     or some other big city in state
  */

  static stateMapRec StateLatLon[] = {
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

  int i = 0;
  while(StateLatLon[i].state) {
    stateMapRec *rec = &StateLatLon[i];
    int code = genStateMap.insert(StateLatLon[i].state, rec);
    DOASSERT(code >= 0, "Invalid hash table code");
    i++;
  }
}

static void MapStateToLatLon(char *state, float &lat, float &lon)
{
  static int warning = 1;

  stateMapRec *rec;
  int code = genStateMap.lookup(state, rec);
  if (code < 0) {
    if (warning)
      fprintf(stderr,
              "Warning: Data has unrecognized state codes, including %s\n",
              state);
    warning = 0;
    lat = lon = 0;
  } else {
    lat = rec->latitude;
    lon = rec->longitude;
  }
}

class StateLatLonComposite : public UserComposite {
public:

  StateLatLonComposite() {
    _init = false;
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

      /* initialize lat/lon hash table */
      InitStateMap();

      _init = true;
    }

    char *buf = (char *)recInterp->GetBuf();
    char *state = buf + attrOffset[0];
    float *latPtr = (float *)(buf + attrOffset[2]);
    float *lonPtr = (float *)(buf + attrOffset[3]);
    MapStateToLatLon(state, *latPtr, *lonPtr);

    // fuzz up the picture a little by creating a cloud

    float cloudRadius = 2.5;
    float length = (rand() % 1000) / (1000 / cloudRadius);
    float dir = (rand() % 360) / 360.0 * 2 * 3.14;
    *latPtr += length * sin(dir);
    *lonPtr += length * cos(dir);

    float *totalAmount = (float *)(buf + attrOffset[1]);
    float *totalPtr = (float *)(buf + attrOffset[4]);
    *totalPtr = 0.2 + *totalAmount / 8000.0;

	// I don't have any idea what this stuff is. CEW 5/14/97
    int *colorPtr = (int *)(buf + attrOffset[5]);

	*colorPtr = 0;	// Probably shouldn't be doing this at all
/*
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
*/
  }

private:
  int       *attrOffset;          /* attribute offsets */
  Boolean   _init;                /* true when instance initialized */
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

int main(int argc, char **argv)
{
  Init::DoInit(argc,argv);
  String env,schemaStr,parserName;	
  char dummy;
  char * schemaChar;
	
 // Reads the composite file from the DEVISE_LIB directory
  
  env = getenv("DEVISE_LIB");
  if (env.chars() == 0 )
	env = ".";
  env += "/composite.ini";
 	
  ifstream fin(env);
  if(!fin) {
	fprintf(stderr, "Cannot open %s for read: %s\n", 
			env.chars(), strerror(errno));
	exit(1);
  }
  fprintf(stderr, "\nReading composite file %s\n", env.chars());

  while(fin){
	
	fin >> dummy;
	fin.putback(dummy);
	// Ignore comments
	if (dummy == '#'){
		readline(fin,schemaStr); // Read line and ignore
	 	continue;
	}	
	fin >> schemaStr >> parserName;

	schemaChar = strdup(schemaStr.chars());
	
	// Register the corresponding composite attribs with their parsers.
	if (parserName.matches("YyMmDdComposite"))
  		CompositeParser::Register(schemaChar ,new YyMmDdComposite);
  	else if (parserName.matches("ObsDateComposite"))
  		CompositeParser::Register(schemaChar,new ObsDateComposite);
  	else if (parserName.matches("DOLDateComposite"))
  		CompositeParser::Register(schemaChar,new DOLDateComposite);
  	else if (parserName.matches("MmDdYyComposite"))
  		CompositeParser::Register(schemaChar,new MmDdYyComposite);
  	else if (parserName.matches("LatLonComposite"))
  		CompositeParser::Register(schemaChar,new LatLonComposite);
  	else if (parserName.matches("DayOfYearComposite"))
  		CompositeParser::Register(schemaChar,new DayOfYearComposite);
  	else if (parserName.matches("YyDdd_HhMmComposite"))
  		CompositeParser::Register(schemaChar,new YyDdd_HhMmComposite);
  	else if (parserName.matches("StateLatLonComposite"))
  		CompositeParser::Register(schemaChar,new StateLatLonComposite);
  	else if (parserName.matches("IBMAddressTraceComposite2"))
  		CompositeParser::Register(schemaChar,new IBMAddressTraceComposite2);
  	else if (parserName.matches("IBMAddressTraceComposite"))
  		CompositeParser::Register(schemaChar,new IBMAddressTraceComposite);
  	else if (parserName.matches("MailorderDateDiffComposite"))
  		CompositeParser::Register(schemaChar,new MailorderDateDiffComposite);
  	else if (parserName.matches("MmDdYyHhMmAmPmComposite"))
  		CompositeParser::Register(schemaChar,new MmDdYyHhMmAmPmComposite);
  	else if (parserName.matches("ISODateComposite"))
  		CompositeParser::Register(schemaChar,new ISODateComposite);
  	else if (parserName.matches("ISODate2Composite"))
  		CompositeParser::Register(schemaChar,new ISODate2Composite);
   }
  /* Register known classes  with control panel */
  ControlPanel::RegisterClass(new TileLayoutInfo);
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new ViewLensInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo());
  ControlPanel::RegisterClass(new CursorClassInfo());

  /* Hack to start control panel so that it'll read the RC files */
  ControlPanel::_controlPanel = GetNewControl();
  ControlPanel *ctrl = ControlPanel::Instance();

  /* This is a hack to create a display before running Dispatcher.
     Otherwise, we'll get an error */

  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();

  /* Keep compiler happy */
  disp = disp;

  /* Disable buffering of stdout (makes debugging easier) */
  (void)setbuf(stdout, NULL);

  /* Populate string storage space from init file */
  StringStorage::PopulateFromInitFile();

  /* Start session (possibly restoring an old one */
  ctrl->StartSession();

  Dispatcher::RunNoReturn();

  return 1;
}
