/*
  $Id$

  $Log$*/

#include "TDataRangeSource.h"
#include "TData.h"
#include "BufMgr.h"

TDataRangeSource::TDataRangeSource(BufMgr *mgr, TData *tdata){
	_mgr = mgr;
	_tdata = tdata;
	_mgr->RegisterCallback(this);
}

/* info about min/max of ranges */
void TDataRangeSource::Extent(long &low, long &high){
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

void TDataRangeSource::InitRangeSourceIterator(){
/*
printf("TDataRangeSource::InitRAngeSourceIterator()\n");
*/
	_mgr->InitTDataInMem(_tdata);
}

Boolean TDataRangeSource::NextRangeSource(long &low, long &high){
	RecId left, right;
	void *buf;
	Boolean stat= _mgr->GetInMemRecs(buf,left, right);
	low = left;
	high = right;
	if (stat)
		_mgr->FreeInMemRecs(buf);

/*
printf("TDataRangeSource::NextRangeSource: %d,%d\n",low, high);
*/
	return stat;

}
void TDataRangeSource::DoneRangeSourceIterator(){
/*
printf("TDataRangeSource::DoneRAngeSourceIterator()\n");
*/
	_mgr->DoneTDataInMem();
}

virtual void TDataRangeSource::Inserted(TData *tdata, RecId low, RecId high){
/*
printf("TDataRangeSource::Inserted(,%d,%d)\n, low, high");
*/
	if (tdata == _tdata){
		ReportRangeInserted((long)low, (long)high);
	}
}

virtual void TDataRangeSource::Deleted(TData *tdata, RecId low, RecId high){
/*
printf("TDataRangeSource::deleted(,%d,%d)\n", low, high);
*/
	if (tdata == _tdata){
		ReportRangeDeleted((long)low, (long)high);
	}
}

