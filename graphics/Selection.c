/*
  $Id$

  $Log$*/

#if defined(SUN)||defined(PENTIUM)
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include "Selection.h"
#include "View.h"
#include "Journal.h"

/* ID number of next selection.
Id of 0 means NULL selection.*/
int Selection::_nextId= 0;	
SelectionList *Selection::_selections = NULL; /* list of all selections */

Selection::Selection(char *name,VisualFlag flag){
	_id = ++_nextId;
	_name = name;
	_flag = flag;
	_numFilters = 0;
	_viewList = new ViewList;

	/* put ourself on the list of all selections */
	if (_selections == NULL)
		_selections = new SelectionList ;
	_selections->Insert(this);
};

Selection::~Selection(){
	_selections->Delete(this);
	int index;
	for(index=_viewList->InitIterator(); _viewList->More(index);){
		View *view = _viewList->Next(index);
		/*
		view->DeleteSelectionCallback(this);
		*/
		_viewList->DeleteCurrent(index);
	}
	_viewList->DoneIterator(index);
}


/*****************************************************************
Find selection with the given id
id == 0 return NULL.
******************************************************************/
Selection *Selection::FindSelectionById(int id){

	if (id == 0)
		return NULL;

	if (_selections == NULL){
		fprintf(stderr,"Selection::findSelectionById: no selections\n");
		Exit::DoExit(1);
	}

	int index;
	for (index= _selections->InitIterator(); _selections->More(index);){
		Selection *sel = _selections->Next(index);
		if (sel->GetId() == id){
			_selections->DoneIterator(index);
			return sel;
		}
	}
	fprintf(stderr,"Selection::findSelectionById: no selection found\n");
	Exit::DoExit(1);
	return NULL; /* keeps compiler happy */
}

/*****************************************************************
Find selection with given name.
******************************************************************/
Selection *Selection::FindSelectionByName(char *name){

	if (_selections == NULL){
		fprintf(stderr,"Selection::findSelectionByName: no selections\n");
		Exit::DoExit(1);
	}

	int index;
	for (index= _selections->InitIterator(); _selections->More(index);){
		Selection *sel = _selections->Next(index);
		if (strcmp(sel->GetName(),name)==0 ){
			_selections->DoneIterator(index);
			return sel;
		}
	}
	fprintf(stderr,"Selection::findSelectionByName: no selection found\n");
	Exit::DoExit(1);
	return NULL; /* keeps compiler happy */
}

/************************************************************************
insert a view into a selection 
************************************************************************/
void Selection::InsertView(View *v){
	/*
	v->InsertSelectionCallback(this);
	*/
	if (ViewListSize() == 0)
		v->GetVisualFilter(_filter);
	else
		SetVisualFilters(v,false, NULL,_flag);

	_viewList->Append(v);
}

/************************************************************************
Find current visual filter. Return TRUE if filter is valid
*************************************************************************/
Boolean Selection::CurrentFilter(VisualFilter &filter){
	if (ViewListSize() == 0)
		return false;
	filter = _filter;
	return true;
}

/* from SelectionCallback */
void Selection::PushSelection(View *view, VisualFilter &filter, 
	VisualFilter &hint,VisualFlag changeFlag){
	/* inform journal */
	Journal::RecordEvent(Journal::PushSelection, this, view, &filter, &hint,
		0,0,0,0);

	PushFilter(_filter);
	_filter = filter;
	SetVisualFilters(view,true,&hint,changeFlag);

}

/* from SelectionCallback */
void Selection::ChangeSelection(View *view, VisualFilter &filter,
	VisualFilter &hint, VisualFlag changeFlag){
	/* inform journal */
	Journal::RecordEvent(Journal::ChangeSelection, this, view, &filter, &hint,
		0,0,0,0);

	_filter = filter;
	SetVisualFilters(view,true,&hint,changeFlag);

}


void Selection::PopSelection(View *view, VisualFilter &hint, 
		VisualFlag changeFlag){
	/* inform journal */
	Journal::RecordEvent(Journal::PopSelection, this, view, NULL,&hint,
		0,0,0,0);

	if (PopFilter(_filter))
		SetVisualFilters(view,true, &hint,changeFlag);
}

void Selection::PushFilter(VisualFilter filter){
	if (_numFilters >= VisualFilterStackSize){
		fprintf(stderr,"visual stack overflow\n");
		Exit::DoExit(1);
	}
	_filterStack[_numFilters++] = filter;
}


/* pop filter, return TRUE if not empty */
Boolean Selection::PopFilter(VisualFilter &filter){
	if (_numFilters == 0)
		return false;
	else {
		filter = _filterStack[--_numFilters];
		return true;
	}
}

/*************************************************************
operations on views
**************************************************************/
int Selection::ViewListSize(){
	return _viewList->Size();
}

int Selection::InitViewIterator(){
	return _viewList->InitIterator();
}

Boolean Selection::MoreView(int index){
	return _viewList->More(index);
}

View *Selection::NextView(int index){
	return _viewList->Next(index);
}

void Selection::DoneViewIterator(int index){
	_viewList->DoneIterator(index);
}


/* update visual filters for all views. If hasHint == TRUE,
use it to update the view with the hint.*/
void Selection::SetVisualFilters(View *selView, Boolean hasHint, 
	VisualFilter *hint, VisualFlag changeFlag){
	int index;
	for(index=_viewList->InitIterator(); _viewList->More(index);){
		View *view = _viewList->Next(index);
		Boolean change; /* TRUE if need to change filter of this view*/
		change = false;
		VisualFilter tempFilter;
		view->GetVisualFilter(tempFilter);
		VisualFlag testFlag;
		if (view == selView)
			/* change according to changeFlag */
			testFlag = changeFlag;
		else  /* change according to linkage flag */
			testFlag = _flag;

		if ((testFlag & VISUAL_X) && (tempFilter.xLow != _filter.xLow 
				|| tempFilter.xHigh != _filter.xHigh)){
			tempFilter.xLow = _filter.xLow;
			tempFilter.xHigh = _filter.xHigh;
			change = true;
		}
		if ((testFlag & VISUAL_Y) && (tempFilter.yLow != _filter.yLow 
				|| tempFilter.yHigh != _filter.yHigh)){
			tempFilter.yLow = _filter.yLow;
			tempFilter.yHigh = _filter.yHigh;
			change = true;
		}
		if ((testFlag & VISUAL_COLOR) && 
			(tempFilter.colorLow != _filter.colorLow 
				|| tempFilter.colorHigh != _filter.colorHigh)){
			tempFilter.colorLow = _filter.colorLow;
			tempFilter.colorHigh = _filter.colorHigh;
			change = true;
		}
		if ((testFlag & VISUAL_SIZE) && 
			(tempFilter.sizeLow != _filter.sizeLow 
				|| tempFilter.sizeHigh != _filter.sizeHigh)){
			tempFilter.sizeLow = _filter.sizeLow;
			tempFilter.sizeHigh = _filter.sizeHigh;
			change = true;
		}
		if((testFlag & VISUAL_PATTERN) &&
			(tempFilter.patternLow != _filter.patternLow 
				|| tempFilter.patternHigh != _filter.patternHigh)){
			tempFilter.patternLow = _filter.patternLow;
			tempFilter.patternHigh = _filter.patternHigh;
			change = true;
		}
		if ((testFlag & VISUAL_ORIENTATION) && 
			(tempFilter.orientationLow != _filter.orientationLow 
				|| tempFilter.orientationHigh != _filter.orientationHigh)){
			tempFilter.orientationLow = _filter.orientationLow;
			tempFilter.orientationHigh = _filter.orientationHigh;
			change = true;
		}
	
		if (change){
			if (hasHint)
				view->SetVisualFilter(tempFilter);
			else view->SetVisualFilter(tempFilter);
		}
	}
	_viewList->DoneIterator(index);
}

int Selection::FindSelectionId(Selection *selection){
	if (selection == NULL)
			return 0;
	else return selection->GetId();
};

