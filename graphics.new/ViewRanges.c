#include "ViewRanges.h"

ViewRanges::ViewRanges(char *name,  ViewWin *parent, VisualFilter &initFilter,
		RangeSource *source, Boolean horizontal)
		: View(name, NULL, initFilter, BlackColor, WhiteColor,
			NULL,NULL, false){
	
	_source = source;
	_source->RegisterCallback(this);
	_horizontal = horizontal;
	AppendToParent(parent);
}

void ViewRanges::DerivedStartQuery(VisualFilter &filter, int timestamp){
/*
printf("ViewRanges::DerivedStartQuery (%f,%f),(%f,%f)\n",
	filter.xLow, filter.yLow, filter.xHigh, filter.yHigh);
*/

	WindowRep *winRep = GetWindowRep();
	VisualFilter *viewFilter = GetVisualFilter();
	winRep->SetFgColor(BlackColor);
	long low, high;
	_source->InitRangeSourceIterator();
	long min, max;
	_source->Extent(min, max);
	while( _source->NextRangeSource(low,high)){
		if (_horizontal){
			winRep->Line(low,0.0, high, 0.0, 2);
		}
		else{
			/*
			winRep->Line(0.0, max-low+min,0.0,  max-high+min, 2);
			*/
			winRep->FillRect(viewFilter->xLow, max-high+min, 
				viewFilter->xHigh- viewFilter->xLow+1, high-low+1);
		}
	}
	_source->DoneRangeSourceIterator();

	ReportQueryDone(0);
}

void ViewRanges::DerivedAbortQuery(){}


void ViewRanges::RangeInserted(long low, long high){
	WindowRep *winRep = GetWindowRep();
	winRep->SetFgColor(BlackColor);
	VisualFilter *viewFilter = GetVisualFilter();
	if (_horizontal)
		winRep->Line(low,0.0, high,0.0, 2);
	else {
		long min, max;
		_source->Extent(min, max);
		/*
		winRep->Line(0.0, max-low+min,0.0, max-high+min, 2);
		*/
		winRep->FillRect(viewFilter->xLow, max-high+min, 
			viewFilter->xHigh- viewFilter->xLow+1, high-low+1);
	}
}

void ViewRanges::RangeDeleted(long low, long high){
	WindowRep *winRep = GetWindowRep();
	winRep->SetFgColor(WhiteColor);
	VisualFilter *viewFilter = GetVisualFilter();
	if (_horizontal)
		winRep->Line(low,0.0, high,0.0,  2);
	else {
		long min, max;
		_source->Extent(min, max);
		/*
		winRep->Line(0.0, max-low+min,0.0, max-high+min, 2);
		*/
		winRep->FillRect(viewFilter->xLow, max-high+min, 
			viewFilter->xHigh- viewFilter->xLow+1, high-low+1);
	}
}

