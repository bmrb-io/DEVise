/*
  $Id$

  $Log$*/

#include "BufMgr.h"

void BufMgr::RegisterCallback(BufMgrCallback *c){
	_callbacks.Append(c);
}

void BufMgr::ReportInserted(TData *tdata, RecId low, RecId high){
	int index;
	for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
		BufMgrCallback *c = _callbacks.Next(index);
		c->Inserted(tdata,low, high);
	}
	_callbacks.DoneIterator(index);

}
void BufMgr::ReportDeleted(TData *tdata, RecId low, RecId high){
	int index;
	for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
		BufMgrCallback *c = _callbacks.Next(index);
		c->Deleted(tdata,low, high);
	}
	_callbacks.DoneIterator(index);
}

