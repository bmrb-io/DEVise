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
      assert(attrOffset);

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr,
		  "YyMmDd composite parser: can't find attribute %s\n",
		  primAttrs[i]);
	  Exit::DoExit(2);
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
      assert(attrOffset);

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr,
		  "MmDdYy composite parser: can't find attribute %s\n",
		  primAttrs[i]);
	  Exit::DoExit(2);
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
      assert(attrOffset);

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr,
		  "ObsDate composite parser: can't find attribute %s\n",
		  primAttrs[i]);
	  Exit::DoExit(2);
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
      assert(attrOffset);

      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr,
		  "DOLDate composite parser: can't find attribute %s\n",
		  primAttrs[i]);
	  Exit::DoExit(2);
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
