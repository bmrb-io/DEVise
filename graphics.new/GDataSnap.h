/*
  $Id$

  $Log$*/

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
