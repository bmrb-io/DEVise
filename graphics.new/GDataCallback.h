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
  Revision 1.2  1995/09/05 22:14:51  jussi
  Added CVS header.
*/

#ifndef GDataCallback_h
#define GDataCallback_h

class GDataCallback {
public:
	/* Report GData and corresponding TData pages converted. */
	virtual void PageConverted(int GDataLow, int GDataHigh,
		int TDataLow, int TDataHigh)=0;
};

DefinePtrDList(GDataCallbackList, GDataCallback *);

#endif
