#include "TDataMapDispatch.h"
#include "TData.h"
#include "GData.h"

TDataMapDispatch::TDataMapDispatch(char *name, TData *tdata, 
	char *gdataName, int gdataRecSize,
	VisualFlag dynamicArgs, int dynamicAttrs, int maxGDataPages,
	VisualFlag *dimensionInfo, int numDimensions, Boolean createGData):
	TDataMap(name, tdata,gdataName, gdataRecSize, dynamicArgs, 
		dynamicAttrs,maxGDataPages, dimensionInfo, numDimensions, createGData){
	Dispatcher::Current()->Register(this);
}

TDataMapDispatch::~TDataMapDispatch(){
	Dispatcher::Current()->Unregister(this);
}

void TDataMapDispatch::Cleanup(){
	GData *gdata= GetGData();
	if (gdata != NULL)
		delete GetGData();
	/*
	delete GetTData();
	*/
}
