/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 21:12:33  jussi
  Added/update CVS header.
*/

#include "Cursor.h"
#include "View.h"

DeviseCursor::DeviseCursor(char *name, VisualFlag flag, Color color){
	_name = name;
	_visFlag = flag;
	_src = NULL;
	_dst = NULL;
	_color = color;
}
DeviseCursor::~DeviseCursor(){
	if (_src != NULL)
		_src->DeleteViewCallback(this);
	if (_dst != NULL)
		_dst->DeleteCursor(this);
}

/* Set source view. Changing this view's visual filter
changes in the cursor's position. Can also set to NULL */
void DeviseCursor::SetSource(View *view){
	if (_src != NULL){
		_src->DeleteViewCallback(this);
	}

	_src = view;
	if (_src != NULL){
		_src->InsertViewCallback(this);
		if (_dst != NULL)
		_dst->DrawCursors();
	}
}

/* Set destination: this is where the cursor will be drawn.
Can also set to NULL. */
void DeviseCursor::SetDst(View *view){
	if (_dst != NULL){
		_dst->DeleteCursor(this);
	}

	_dst = view;
	if (_dst != NULL){
		_dst->AppendCursor(this);
		if (_src != NULL)
			_dst->DrawCursors();
	}
}

/* Get current visual filter. return TRUE if it exists. */
Boolean DeviseCursor::GetVisualFilter(VisualFilter *&filter, Color &color){

	if (_src == NULL){
		return false;
	}

	_src->GetVisualFilter(_filter);
	_filter.flag = _visFlag;
	filter = &_filter;
	color = _color;
	return true;
}

void DeviseCursor::FilterChanged(View *view, VisualFilter &filter,
		int flushed) {
	if (view == _src){
		_dst->DrawCursors();
	}
}

void DeviseCursor::ViewCreated(View *view){ }

void DeviseCursor::ViewDestroyed(View *view){
	if (view == _src){
		_src = NULL;
	}
	if (view == _dst){
		_dst = NULL;
	}
}

void DeviseCursor::MoveSourceX(Coord x) {
	if (_src != NULL){
		VisualFilter filter;
		_src->GetVisualFilter(filter);
		Coord width = (filter.xHigh - filter.xLow);
		Coord newXLow = x - width/2.0;
		Coord xMin;
		if (_src->GetXMin(xMin) && newXLow < xMin)
			newXLow = xMin;
		filter.xLow = newXLow; filter.xHigh = newXLow + width;
		_src->SetVisualFilter(filter);
	}
}
