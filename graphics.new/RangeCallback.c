/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */
/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:15:19  jussi
  Added CVS header.
*/

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
