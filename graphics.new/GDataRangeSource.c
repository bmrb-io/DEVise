/* GDataRangeSource.c */
#include "GDataRangeSource.h"
#include "TData.h"
#include "GData.h"

GDataRangeSource::GDataRangeSource(GData *gdata, TData *tdata){
	_gdata = gdata;
	_tdata = tdata;
	_gdata->RegisterCallback(this);
}

/* info about min/max of ranges */
void GDataRangeSource::Extent(long &low, long &high){
	RecId id;
	if (_tdata->HeadID(id)){
		low = (long)id;
		_tdata->LastID(id);
		high = (long)id;
	}
	else {
		low = high = 0;
	}
}

void GDataRangeSource::InitRangeSourceIterator(){
	_gdata->InitConvertedIterator();
}

Boolean GDataRangeSource::NextRangeSource(long &low, long &high){
	RecId left, right;
	Boolean stat = _gdata->NextRange(left, right);
	low = left;
	high = right;
	return stat;
}

void GDataRangeSource::DoneRangeSourceIterator(){
	_gdata->DoneConvertedIterator();
}

/* from GDataCallback */
void GDataRangeSource::Converted(RecId low, RecId high){
	RangeSource::ReportRangeInserted((long)low, (long)high);
}


