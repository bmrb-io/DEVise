/*
  $Id$

  $Log$*/

#include "RangeCallback.h"

void RangeSource::RegisterCallback(RangeCallback *c){
	_callBacks.Append(c);
}
void RangeSource::DeleteCallback(RangeCallback *c){
	_callBacks.Delete(c);
}

/* Used by derived class to inform all callbacks about
range inserted/deleted */
void RangeSource::ReportRangeInserted(long low, long high){
	int index;
	for (index=_callBacks.InitIterator(); _callBacks.More(index);){
		RangeCallback *c = _callBacks.Next(index);
		c->RangeInserted(low, high);
	}
	_callBacks.DoneIterator(index);
}

void RangeSource::ReportRangeDeleted(long low, long high){
	int index;
	for (index=_callBacks.InitIterator(); _callBacks.More(index);) {
		RangeCallback *c = _callBacks.Next(index);
		c->RangeDeleted(low, high);
	}
	_callBacks.DoneIterator(index);
}
