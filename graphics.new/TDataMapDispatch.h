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
  Revision 1.3  1995/11/25  01:20:17  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.2  1995/09/05 22:15:57  jussi
  Added CVS header.
*/

#ifndef TDataMapDispatch_h
#define TDataMapDispatch_h

#include "TDataMap.h"
#include "Dispatcher.h"

class TDataMapDispatch: public TDataMap, public DispatcherCallback {
public:
	
  TDataMapDispatch(char *name, TData *tdata, char *gdataName,
		   int gdataRecSize, VisualFlag dynamicArgs,
		   int dynamicAttrs, int maxGDataPages,
		   VisualFlag *dimensionsInfo, int numDimensions,
		   Boolean createGData = true);
  virtual ~TDataMapDispatch();

  /* convert from Tdata to Gdata */
  virtual void ConvertToGData(RecId recId,void *buf, void **tRecs,int numRecs,
			      void *gdataBuf) {}

#if 0
  virtual void UpdateBoundingBox(int pageNum, void **gdataArray,
				 int numRecs) {}
#endif

  virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms) {}

  char *DispatchedName() { return "TDataMapDispatch"; }
  virtual void Cleanup();
};

#endif
