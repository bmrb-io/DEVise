/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:16:20  jussi
  Added CVS header.
*/

#include "ViewScatter.h"
#include "TDataMap.h"

ViewScatter::ViewScatter(char *name, 
			 VisualFilter &initFilter, QueryProc *qp,  
			 Color fg,Color bg, 
			 AxisLabel *xAxis, AxisLabel *yAxis,
			 Action *action) : 
	ViewGraph(name,initFilter, xAxis, yAxis, fg, bg, action)
{
  _queryProc = qp;
  _map = NULL;
}

void ViewScatter::InsertMapping(TDataMap *map)
{
  if (_map != NULL){
    fprintf(stderr,"ViewScatter: cna't handle > 1 mapping\n");
    return;
  }
  _map = map;
  ViewGraph::InsertMapping(map);
}


void ViewScatter::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
  _queryFilter = filter;

  /* find amount of X and Y per pixel */

  VisualFilter filter;
  GetVisualFilter(filter);

  int scrnX, scrnY;
  unsigned int scrnWidth, scrnHeight;
  Geometry(scrnX,scrnY,scrnWidth,scrnHeight);

  _xPerPixel = (filter.xHigh - filter.xLow + 1) / scrnWidth;
  _yPerPixel= (filter.yHigh - filter.yLow + 1) / scrnHeight;
  
  _queryProc->BatchQuery(_map, _queryFilter, this, NULL, timestamp);
}

void ViewScatter::DerivedAbortQuery()
{
  _queryProc->AbortQuery(_map, this);
}

/* Query data ready to be returned. Do initialization here.*/
void ViewScatter::QueryInit(void *userData)
{
}

struct GDataTemp {
  Coord x, y;
};

void ViewScatter::ReturnGData(TDataMap *mapping, RecId recId,
			      void *gdata, int numGData)
{
  int gRecSize = mapping->GDataRecordSize();
  
  /*
     printf("ViewScatter %d recs buf start 0x%x, end 0x%x\n", numGData,
     gdata, ((char *)gdata)+numGData*gRecSize);
  */

  char *ptr = (char *)gdata;
  int recIndex = 0;
  VisualFilter *filter = GetVisualFilter();
  Coord maxWidth, maxHeight;
  mapping->MaxBoundingBox(maxWidth, maxHeight);
  for(int i = 0; i < numGData; i++) {
    GDataTemp *gdata = (GDataTemp *)ptr;
    if (gdata->x < (filter->xLow-maxWidth) || 
	gdata->x > (filter->xHigh+maxWidth) || 
	gdata->y < (filter->yLow-maxHeight) || 
	gdata->y > (filter->yHigh+maxHeight)) {
      ptr += gRecSize;
      continue;
    }
    
    _recs[recIndex++] = ptr;
    if (recIndex == WINDOWREP_BATCH_SIZE) {
      mapping->DrawGDataArray(GetWindowRep(), _recs, recIndex,
			      _xPerPixel, _yPerPixel);
      recIndex = 0;
    }
    ptr += gRecSize;
  }

  if (recIndex > 0)
    mapping->DrawGDataArray(GetWindowRep(), _recs, recIndex,
			    _xPerPixel, _yPerPixel);
}

/* Done with query */
void ViewScatter::QueryDone(int bytes,void *userData)
{
  ReportQueryDone(bytes);
}

