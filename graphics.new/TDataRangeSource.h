/* TDataRangeSource.h: source of which TData ranges have been converted*/
#ifndef TDataRangeSource_h
#define TDataRangeSource_h
#include "RangeCallback.h"
#include "BufMgr.h"

class TData;
class TDataRangeSource: public RangeSource, private BufMgrCallback{
public:
	TDataRangeSource(BufMgr *mgr, TData *tdata);

	/* info about min/max of ranges */
	virtual void Extent(long &low, long &high);

	/* Iterator */
	virtual void InitRangeSourceIterator();
	virtual Boolean NextRangeSource(long &low, long &high);
	virtual void DoneRangeSourceIterator();

private:
    virtual void Inserted(TData *tdata, RecId low, RecId high);
	virtual void Deleted(TData *tdata, RecId low, RecId high);

	TData *_tdata;
	BufMgr *_mgr;
};
#endif
