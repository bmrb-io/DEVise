/*
  $Id$

  $Log$*/

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
