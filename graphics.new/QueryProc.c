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
  Revision 1.12  1998/02/03 23:46:38  wenger
  Fixed a problem Hongyu had with getting GData on socket; fixed bugs
  283 and 285 (resulted from problems in color manager merge);
  conditionaled out some debug output.

  Revision 1.11  1997/05/30 15:41:19  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.10  1997/05/28 15:39:26  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.9.8.1  1997/05/20 16:11:12  ssl
  Added layout manager to DEVise

  Revision 1.9  1996/11/23 21:12:06  jussi
  Removed support for multiple query processors.

  Revision 1.8  1996/11/20 16:51:18  jussi
  Replaced AbortAndReexecute() with AbortQuery() and Refresh().

  Revision 1.7  1996/07/02 22:46:23  jussi
  Added debugging statement.

  Revision 1.6  1996/06/27 15:52:44  jussi
  Added functionality which allows TDataAscii and TDataBinary to request
  that views using a given TData be refreshed (existing queries are
  aborted and new queries are issued). Fixed a few bugs in QueryProcFull
  which became visible only when this new functionality was tested.

  Revision 1.5  1996/06/23 20:33:01  jussi
  Cleaned up.

  Revision 1.4  1996/06/12 14:56:24  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.3  1996/01/15 16:56:31  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:15:14  jussi
  Added CVS header.
*/

#include "QueryProc.h"
#include "QueryProcFull.h"
#include "Control.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "Init.h"
#include "Exit.h"

QueryProc *QueryProc::_queryProc = 0;

QueryProc *QueryProc::Instance()
{
    if (!_queryProc)
        _queryProc = new QueryProcFull();
    
    return _queryProc;
}

/* Abort existing queries that use given TData and re-execute the queries */

void QueryProc::RefreshTData(TData *tdata)
{
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    
    int index = View::InitViewIterator();
    while(View::MoreView(index)) {
        View *v = View::NextView(index);
        char *name = v->GetName();
        DOASSERT(name, "Invalid view name");
        ViewGraph *vg = (ViewGraph *)classDir->FindInstance(name);
        DOASSERT(vg, "Cannot find view");

        Boolean usesTData = false;
        int idx = vg->InitMappingIterator();
        while(vg->MoreMapping(idx)) {
            TDataMap *map = vg->NextMapping(idx)->map;
            if (map->GetPhysTData() == tdata ||
	      map->GetLogTData() == tdata) {
                usesTData = true;
                break;
            }
        }
        vg->DoneMappingIterator(idx);
        if (usesTData) {
#ifdef DEBUG
            printf("Reexecuting query for view %s with new data...\n",
                   vg->GetName());
#endif
            vg->AbortQuery();
            vg->Refresh();
            if (vg->GetAutoScale())
              vg->GoHome();
        }
    }
    View::DoneViewIterator(index);
}
