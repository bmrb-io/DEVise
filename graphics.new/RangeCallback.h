/*
  $Id$

  $Log$*/

/* callback about range info */

#ifndef RangeCallback_h
#define RangeCallback_h
#include "DeviseTypes.h"
#include "DList.h"

class RangeCallback{
public:
	virtual void RangeInserted(long low, long high)=0;
	virtual void RangeDeleted(long low, long high)=0;
};

DefinePtrDList(RangeCallbackList, RangeCallback *)

/* Source of range information */
class RangeSource {
public:
	void RegisterCallback(RangeCallback *c);
	void DeleteCallback(RangeCallback *c);

	/* info about min/max of ranges */
	virtual void Extent(long &low, long &high) = 0;

	/* Iterator */
	virtual void InitRangeSourceIterator()=0;
	virtual Boolean NextRangeSource(long &low, long &high)=0;
	virtual void DoneRangeSourceIterator()=0;
protected:
	/* Used by derived class to inform all callbacks about
	range inserted/deleted */
	void ReportRangeInserted(long low, long high);
	void ReportRangeDeleted(long low, long high);

private:
	RangeCallbackList _callBacks;
};

#endif
