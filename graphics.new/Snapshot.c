/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.2  1995/09/05 22:15:36  jussi
  Added CVS header.
*/

#include "Snapshot.h"
#include "TDataMap.h"
#include "TData.h"
#include "TDataRangeSource.h"
#include "GDataRangeSource.h"
#include "ViewRanges.h"
#include "Util.h"

/* Make snapshot windows for TData */
void MakeSnapshotView(TDataMap *map, BufMgr *mgr, ViewWin *parent,
		      int flag, Boolean hor)
{
  char buf[80];

  TData *tdata = map->GetTData();
  /* show TData in mem */

  VisualFilter snapFilter;
  TDataRangeSource *tSource = new TDataRangeSource(mgr, tdata);
  long low, high;
  tSource->Extent(low, high);
  if (hor) {
    snapFilter.flag = VISUAL_LOC;
    snapFilter.xLow = (Coord )low;
    snapFilter.yLow = -1;
    snapFilter.xHigh = (Coord)high;
    snapFilter.yHigh = 1;
  } else {
    /* vertical line */
    snapFilter.flag = VISUAL_LOC;
    snapFilter.xLow = -1;
    snapFilter.yLow = (Coord )low;
    snapFilter.xHigh = 1;
    snapFilter.yHigh = (Coord)high;
  }

  if (flag & DisplayTDataInMem) {
    sprintf(buf,"%s in mem", tdata->GetName());
    ViewRanges *tView = new ViewRanges(CopyString(buf), parent,
				       snapFilter,tSource, hor);
    tView->XAxisDisplayOnOff(false);
    tView->YAxisDisplayOnOff(false);
    tView->SetLabelParam(true,20,"T");
  }
	
  GData *gdata = map->GetGData();
  if (gdata != NULL) {
    if (flag & DisplayGDataInMem) {
      TDataRangeSource *gSource = new TDataRangeSource(mgr, gdata);
      sprintf(buf,"%s in mem", gdata->GetName());
      ViewRanges *gView = new ViewRanges(CopyString(buf), 
					 parent, snapFilter,
					 gSource, hor);
      gView->XAxisDisplayOnOff(false);
      gView->YAxisDisplayOnOff(false);
      gView->SetLabelParam(true, 20, "G");
    }
    
    /* Display Converted GData */
    if (flag & DisplayGDataConverted) {
      GDataRangeSource *gConvSource= new GDataRangeSource(gdata, tdata);
      sprintf(buf,"%s converted",gdata->GetName());
      ViewRanges *gConvView = new ViewRanges(CopyString(buf),
					     parent, snapFilter,
					     gConvSource, hor);
      gConvView->XAxisDisplayOnOff(false);
      gConvView->YAxisDisplayOnOff(false);
      gConvView->SetLabelParam(true,20,"C");
    }
  }
}
