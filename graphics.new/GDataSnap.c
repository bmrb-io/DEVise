/* GDataSnap.c: takes snapshot of GData conversion */
#include "DeviseTypes.h"
#include "RectShape.h"
#include "ColorMgr.h"
#include "SimpleSymStrm.h"
#include "SimpleFocus.h"
#include "SimpleView.h"
#include "StreamCached.h"
#include "Selection.h"
#include "VisualArg.h"
#include "Init.h"
#include "GDataSnap.h"
#include "PageRangeMap.h"
#include "ViewWindow.h"

class GDataSnapMapping: public ColorCompressionMapping {
public:
	GDataSnapMapping(){
		double attrs[2];
		attrs[0] = attrs[1] = 1.0;
		Mapping::SetDefaultShape(RectXShapeID, 2, attrs);
		Mapping::SetDefaultColor(BlackColor);
	}

	void Map(Stream *, void *rec, Symbol *symbol){
		int *data = (int *)rec;
		/*
		printf("map %d\n", *data);
		*/
		symbol->x = (*data)+0.5;
		symbol->y = 0.5;
	}
};

/***********************************************************************
Constructor
***********************************************************************/
GDataSnap::GDataSnap(GDataVar *gdata, ViewWindow *viewWin){
	_gdata = gdata;
	_tdata = gdata->GetTData();


	/* Create views */
	_mapping = new GDataSnapMapping();
	_stream = new StreamCached(NULL, _tdata->NumPages());
	_symStrm = new SimpleSymStrm(_stream, _mapping);
	_focus = new SimpleFocus(_symStrm);
	_view = new SimpleView("GDataSnap", 0.0, -0.25, 
			(Coord)_tdata->NumPages(), 1.5, true, false, 1, NULL);
	_view->InsertFocus(_focus);

	_viewWin = viewWin;
	_viewWin->Bind(_view);

	/* insert the pages that we already know to have been mapped */
	_gdata->InitRangeIterator();
	PageRangeMapRec *rec;

	while((rec=_gdata->NextRange()) != NULL){
		int i;
		for (i=rec->tLow; i <= rec->tHigh; i++){
			/*
			printf("inserting %d\n", i);
			*/
			int *ptr = new int;
			*ptr = i;
			_stream->InsertRec(ptr);
		}
		
	}
	_gdata->InsertCallback(this);
}

/*****************************************************************************
Process Callback from GData
******************************************************************************/
void GDataSnap::PageConverted(int /* GDataLow*/ , int /*GDataHigh*/ ,
		int tLow, int tHigh){
	
	int i;
	for (i= tLow; i <= tHigh; i++){
		int *ptr = new int;
		*ptr = i;
		_stream->InsertRec(ptr);
	}
}

