/* dispAfsio.c: display tweety
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
#include "QueryProcTape.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "CursorClassInfo.h"
#include "TDataTapeInterp.h"
#include "ParseCat.h"

int debug = 0;

/* name of primary attributes */
char *primAttrs[] = {
  "OBS_DAY",
  "OBS_MON",
  "OBS_YEAR",
  "OBS_HOUR",
  "OBS_MIN",
  "OBS_SEC",
  "DATE"
};

const int numPrimAttrs = sizeof primAttrs / sizeof primAttrs[0];

/* User composite to parse date */
class ISSMComposite : public UserComposite {
public:

  ISSMComposite() {
    _init = false;
  }

  virtual void Decode(RecInterp *recInterp) {
    if (!_init) {
      /* initialize by caching offsets of all the attributes we need */
      for(int i = 0; i < numPrimAttrs; i++) {
	AttrInfo *info;
	if (!(info = recInterp->GetAttrInfo(primAttrs[i]))) {
	  fprintf(stderr, "ISSM composite attr: can't find attr %s\n",
		  primAttrs[i]);
	  Exit::DoExit(2);
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

    /* see if we can reuse the time value from last call's
       mktime() call; mktime costs 1 msec per call so we
       don't want to call it too often */
    
    time_t nowtime = 0;

    if (now.tm_year == lasttm.tm_year
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
    
    time_t *datePtr = (time_t *)(buf + attrOffset[6]);
    *datePtr = nowtime;
  }

private:
  time_t    lasttime;             /* cached value of last time */
  struct tm lasttm;               /* cached value of last time */

  int attrOffset[numPrimAttrs];   /* attribute offset */
  Boolean   _init;                /* true when instance initialized */
};

class ISSMGenClass: public GenClassInfo  {
public:
  virtual ClassInfo *Gen(char *source, char *className, AttrList *attrList,
			 int recSize, char *separators, int numSeparators,
			 Boolean isSeparator, char *commentString){
    if (strcmp(source,"tape") == 0){
      return new TDataTapeInterpClassInfo(className,attrList,
					  recSize, separators, numSeparators,
					  isSeparator, commentString);
    } else {
      fprintf(stderr,"schema source %s not found\n", source);
      Exit::DoExit(1);
    }
  }
};

QueryProc *genQueryProcTape()
{
  return new QueryProcTape;
}

main(int argc, char **argv)
{
  Init::DoInit(argc,argv);

  /* Register composite parser */
  CompositeParser::Register("ISSM-Trade", new ISSMComposite);
  CompositeParser::Register("ISSM-Quote", new ISSMComposite);

  /* Register parser for tape */
  RegisterGenClassInfo("tape", new ISSMGenClass());

  /* Register known query processors */
  QueryProc::Register("Tape", genQueryProcTape);

  /* Register known classes  with control panel */
  ControlPanel::RegisterClass(new WinVerInfo);
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo());
  ControlPanel::RegisterClass(new CursorClassInfo());

  /* hack to start control panel so that it'll read the RC files */
  ControlPanel *ctrl = ControlPanel::Instance();

  /* This is a hack to create a display before running Dispatcher.
     Otherwise, we'll get an error */
  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();
  
  /* Start session (possibly restoring an old one */
  ctrl->StartSession();

  Dispatcher::RunNoReturn();
}
