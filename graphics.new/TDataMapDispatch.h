/*
  $Id$

  $Log$*/

#ifndef TDataMapDispatch_h
#define TDataMapDispatch_h

#include "TDataMap.h"
#include "Dispatcher.h"

class TDataMapDispatch: public TDataMap, public DispatcherCallback {

public:
	
	TDataMapDispatch(char *name, TData *tdata, char *gdataName,
		int gdataRecSize, VisualFlag dynamicArgs, int dynamicAttrs, 
		int maxGDataPages, VisualFlag *dimensionsInfo, int numDimensions,
		Boolean createGData = true);
	virtual ~TDataMapDispatch();

	/* convert from Tdata to Gdata */
	virtual void ConvertToGData(RecId recId,void *buf, void **tRecs,int numRecs,
		void *gdataBuf){};

	/*
	virtual void UpdateBoundingBox(int pageNum,void **gdataArray,int numRecs){};
	*/

	virtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms,
		Coord xPerPixel, Coord yPerPixel){};

	char *DispatchedName(){
		return "TDataMapDispatch";
	}
	virtual void Cleanup();
};

#endif
