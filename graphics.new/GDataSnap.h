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
  Revision 1.2  1995/09/05 22:14:57  jussi
  Added CVS header.
*/

/* responsible for taking a snapeshot of GData */

#ifndef GDataSnap_h
#define GDataSnap_h
#include "GDataVar.h"

class View;
class StreamCached;
class SymbolStream;
class Focus;
class ViewWindow;

class GDataSnap: private GDataCallback{
public:
	GDataSnap(GDataVar *gdata, ViewWindow *viewWin);

private:
	/* from GDataCallback */
	void GDataSnap::PageConverted(int /* GDataLow*/ , int /*GDataHigh*/ ,
		int tLow, int tHigh);


StreamCached *_stream;
Mapping *_mapping;
SymbolStream *_symStrm;
Focus *_focus;
View *_view;
GDataVar *_gdata;
TData *_tdata;
Boolean _initialized;
ViewWindow *_viewWin;
};
#endif
