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
  Revision 1.2  1995/09/05 22:15:56  jussi
  Added CVS header.
*/

#include "TDataMapDispatch.h"
#include "TData.h"
#include "GData.h"

TDataMapDispatch::TDataMapDispatch(char *name, TData *tdata, 
				   char *gdataName, int gdataRecSize,
				   VisualFlag dynamicArgs, int dynamicAttrs,
				   int maxGDataPages,
				   VisualFlag *dimensionInfo,
				   int numDimensions, Boolean createGData)
     : TDataMap(name, tdata,gdataName, gdataRecSize, dynamicArgs, 
		dynamicAttrs,maxGDataPages, dimensionInfo, numDimensions,
		createGData)
{
  Dispatcher::Current()->Register(this);
}

TDataMapDispatch::~TDataMapDispatch()
{
  Dispatcher::Current()->Unregister(this);
}

void TDataMapDispatch::Cleanup()
{
  GData *gdata = GetGData();
  delete gdata;
}
