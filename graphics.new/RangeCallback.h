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
  Revision 1.3  1997/03/28 16:10:26  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:15:20  jussi
  Added CVS header.
*/

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
	virtual ~RangeSource() {}

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
