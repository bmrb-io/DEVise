/* GDataRangeSource.h: source of which GData ranges have been converted*/
#ifndef GDataRangeSource_h
#define GDataRangeSource_h
#include "RangeCallback.h"
#include "GData.h"

class GDataRangeSource: public RangeSource, private GDataCallback{
public:
	GDataRangeSource(GData *gdata, TData *tdata);

	/* info about min/max of ranges */
	virtual void Extent(long &low, long &high);

	/* Iterator */
	virtual void InitRangeSourceIterator();
	virtual Boolean NextRangeSource(long &low, long &high);
	virtual void DoneRangeSourceIterator();

private:
	/* from GDataCallback */
	virtual void Converted(RecId low, RecId high);

	GData *_gdata;
	TData *_tdata;
};
#endif
