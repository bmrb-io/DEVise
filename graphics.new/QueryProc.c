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
#include "DispQueryProc.h"
#include "Control.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "Init.h"
#include "Exit.h"

QueryProc *QueryProc::_queryProc = 0;
int QueryProc::_numQueryProcs = 0;      /* number of query processors */
QPRec QueryProc::_qpArray[MAX_QUERYPROCS];

void QueryProc::Register(char *name, QueryProcGen *gen)
{
  DOASSERT(_numQueryProcs < MAX_QUERYPROCS - 1, "Too many query processors");

  _qpArray[_numQueryProcs].name = name;
  _qpArray[_numQueryProcs++].gen = gen;
}

QueryProc *QueryProc::Instance()
{
  if (!_queryProc) {
    char *qpName = Init::QueryProc();
    if (!strcmp(qpName, "default")) {
      _queryProc = new DispQueryProcFull();
    } else {
      Boolean found = false;
      for(int i = 0; i < _numQueryProcs; i++) {
	if (!strcmp(_qpArray[i].name,qpName)) {
	  _queryProc = (*_qpArray[i].gen)();
	  found = true;
	  break;
	}
      }
      if (!found){
	fprintf(stderr, "Cannot find query processor %s\n", qpName);
	DOASSERT(0, "Program terminates");
      }
    }
  }

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
            if (map->GetTData() == tdata) {
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
              vg->UpdateAutoScale();
        }
    }
    View::DoneViewIterator(index);
}
