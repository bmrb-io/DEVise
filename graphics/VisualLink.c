/* VisualLinkc */
#include "VisualLink.h"
#include "View.h"

VisualLink::VisualLink(char *name, VisualFlag linkFlag){
	_name = name;
	_flag = linkFlag;
	_viewList = new LinkViewList;
	_updating = false; 

	_view = NULL;
	Dispatcher::Current()->Register(this);
	View::InsertViewCallback(this);
}

VisualLink::~VisualLink(){
	Dispatcher::Current()->Unregister(this);
	View::DeleteViewCallback(this);
}

/* REturn true if view belongs to this link */
Boolean VisualLink::ViewInLink(View *view){
	return _viewList->Find(view);
}

/* insert view into visual link */
void VisualLink::InsertView(View *view){
	if (_viewList->Find(view))
		/* view already inserted */
		return;
	_viewList->Append(view);
	if (_viewList->Size() > 1) {
		View *firstView = _viewList->GetFirst();
		VisualFilter *filter = firstView->GetVisualFilter();
		SetVisualFilter(view, *filter);
	}
}

/* delete view from visual link */
void VisualLink::DeleteView(View *view){
	_viewList->Delete(view);
}

/* Called by View when its visual filter has changed.
flushed == index if 1st element in the history that has been flushed.*/
void VisualLink::FilterChanged(View *view, VisualFilter &filter,
		int flushed){
	if (_view != NULL){
		/* already have a view queued up. Ignore this one  */
		/*
		fprintf(stderr,"VisualLink: can't queue >1 view\n");
		Exit::DoExit(1);
		*/
		return;
	}
	/* first, make sure that this view is under our control */
	int index;
	Boolean found = false;
	for (index= _viewList->InitIterator(); _viewList->More(index); ){
		View *viewInList = _viewList->Next(index);
		if (viewInList == view){
			found = true;
			break;
		}
	}
	_viewList->DoneIterator(index);

	if (found){
		_view = view;
		_filter = filter;
	}
}

void VisualLink::Run(){
	if (_view != NULL){
		ProcessFilterChanged(_view,_filter);
		_view = NULL;
	}
}

void VisualLink::ProcessFilterChanged(View *view, VisualFilter &filter){
/*
printf("ProcessFilter changed for %s\n",view->GetName());
*/

	/* We need to do this because once a visual link starts changing
	the filter of its views, this function can be called subsequently by
	those views, and we need to ignore all those calls because we have
	already changed those filters.*/
	if (_updating){
		/*
		printf("ignored because updating\n");
		*/
		return;
	}
	
	int index;
	for (index= _viewList->InitIterator(); _viewList->More(index); ){
		View *viewInList = _viewList->Next(index);
		if (viewInList != view){
			/* Change this view */
			/*
			printf("link: setting %s\n",viewInList->GetName());
			*/
			SetVisualFilter(viewInList, filter);
		}
	}
	_viewList->DoneIterator(index);
}

/* update visual filters for view views. */
void VisualLink::SetVisualFilter(View *view,  VisualFilter &filter){
	_updating = true;

	VisualFilter tempFilter;
	view->GetVisualFilter(tempFilter);
	VisualFlag testFlag;
	testFlag = _flag;
	Boolean change = false;

	if ((testFlag & VISUAL_X) && (tempFilter.xLow != filter.xLow 
				|| tempFilter.xHigh != filter.xHigh)){
		Coord xMin;
		Boolean hasXMin = HasXMin(xMin);
		/*
		printf("VisualLink::SetVisualFilter ::hasXMin: %d %f\n",hasXMin, xMin);
		*/
		tempFilter.xLow = filter.xLow;
		tempFilter.xHigh = filter.xHigh;
		if (hasXMin && filter.xLow < xMin){
			tempFilter.xHigh = xMin+(filter.xHigh-filter.xLow);
			tempFilter.xLow = xMin;
		}

		if (tempFilter.xHigh <= tempFilter.xLow)
			tempFilter.xHigh = tempFilter.xLow+1;
			
		change = true;
	}
	if ((testFlag & VISUAL_Y) && (tempFilter.yLow != filter.yLow 
			|| tempFilter.yHigh != filter.yHigh)){
		tempFilter.yLow = filter.yLow;
		tempFilter.yHigh = filter.yHigh;
		change = true;
	}
	if ((testFlag & VISUAL_COLOR) && 
		(tempFilter.colorLow != filter.colorLow 
			|| tempFilter.colorHigh != filter.colorHigh)){
		tempFilter.colorLow = filter.colorLow;
		tempFilter.colorHigh = filter.colorHigh;
		change = true;
	}
	if ((testFlag & VISUAL_SIZE) && 
		(tempFilter.sizeLow != filter.sizeLow 
			|| tempFilter.sizeHigh != filter.sizeHigh)){
		tempFilter.sizeLow = filter.sizeLow;
		tempFilter.sizeHigh = filter.sizeHigh;
		change = true;
	}
	if((testFlag & VISUAL_PATTERN) &&
		(tempFilter.patternLow != filter.patternLow 
			|| tempFilter.patternHigh != filter.patternHigh)){
		tempFilter.patternLow = filter.patternLow;
		tempFilter.patternHigh = filter.patternHigh;
		change = true;
	}
	if ((testFlag & VISUAL_ORIENTATION) && 
		(tempFilter.orientationLow != filter.orientationLow 
			|| tempFilter.orientationHigh != filter.orientationHigh)){
		tempFilter.orientationLow = filter.orientationLow;
		tempFilter.orientationHigh = filter.orientationHigh;
		change = true;
	}
	if ((testFlag & VISUAL_SHAPE) && 
		(tempFilter.shapeLow != filter.shapeLow 
			|| tempFilter.shapeHigh != filter.shapeHigh)){
		tempFilter.shapeLow = filter.shapeLow;
		tempFilter.shapeHigh = filter.shapeHigh;
		change = true;
	}
	
	if (change)
		view->SetVisualFilter(tempFilter);
	
	_updating = false;
}

int VisualLink::InitIterator(){
	return _viewList->InitIterator();
}

Boolean VisualLink::More(int index){
	return _viewList->More(index);
}

View *VisualLink::Next(int index){
	return _viewList->Next(index);
}

void VisualLink::DoneIterator(int index){
	_viewList->DoneIterator(index);
}

/* Find xMin, if any */
Boolean VisualLink::HasXMin(Coord &xMin){
	Boolean hasXMin = true;
	Coord lowX;
	Boolean first = true;
	int index;
	for (index= _viewList->InitIterator(); _viewList->More(index); ){
		View *view = _viewList->Next(index);
		if (!view->GetXMin(lowX)){
			hasXMin = false;
			break;
		}
		else if (first){
			xMin = lowX;
			first = false;
		}
		else if (xMin > lowX)
			xMin = lowX;
	}
	_viewList->DoneIterator(index);
	if (first)
		return false;
	return hasXMin;
}
