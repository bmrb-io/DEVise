/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "ViewWin.h"
#include "Display.h"
#include "WindowRep.h"

ViewWin::ViewWin(char *name, Color fg, Color bg, int weight, Boolean boundary){
	_name = name;
	_windowRep = NULL;
	_parent = NULL;
	_mapped = false;
	_weight = weight;
	_winBoundary = boundary;
	_iconified = true;
	_background = bg;
	_foreground = fg;
}

void ViewWin::Iconify(){
	if (_windowRep != NULL)
		_windowRep->Iconify();
}

void ViewWin::AppendToParent(ViewWin *parent){
	if ( _parent != NULL){
		_parent->Delete(this);
	}
	if (_mapped)
		Unmap();
	_parent = parent;
	_parent->Append(this);
}

void ViewWin::DeleteFromParent() {
	if (_parent != NULL){
		_parent->Delete(this);
		if (_mapped)
			Unmap();
		_parent = NULL;
	}
}

void ViewWin::Map(int x, int y, unsigned w, unsigned h) {
	if (_mapped){
		fprintf(stderr,"ViewWin::Map() already mapped\n");
		Exit::DoExit(1);
	}

	if (_parent != NULL){
		_windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(
			_name, x,y,w,h,_foreground,_background,_parent->GetWindowRep(),
			1.0,1.0,false, _winBoundary);
		_windowRep->RegisterCallback(this);
	}
	else {
		/* Create a new WindowRep */
		_windowRep = DeviseDisplay::DefaultDisplay()->CreateWindowRep(_name,
			x,y,w,h,BlackColor,WhiteColor,NULL,100, 100, false, _winBoundary);
		_windowRep->RegisterCallback(this);
	}
	_hasGeometry = false;
	_mapped = true;
	SubClassMapped();
}

void ViewWin::Unmap(){
	if (_mapped){
		SubClassUnmapped();
		DeviseDisplay::DefaultDisplay()->DestroyWindowRep(_windowRep);
		_windowRep = NULL;
		_hasGeometry = false;
		_mapped = false;
	}
}

Boolean ViewWin::Mapped(){
	return _mapped;
}


ViewWin::~ViewWin(){
/*
printf("ViewWin destructor\n");
*/
	DeleteFromParent();
	Unmap();
}

/* Append child */
void ViewWin::Append(ViewWin *child){
	_children.Append(child);
}

/* Delete child */
void ViewWin::Delete(ViewWin *child){
	if (!_children.Delete(child)){
		fprintf(stderr,"ViewWin::Delete child not found\n");
		Exit::DoExit(2);
	}
}

/* Get current geometry of child w. r. t. parent */
void ViewWin::Geometry(int &x, int &y, unsigned &w, unsigned &h){
	if (_windowRep == NULL){
		fprintf(stderr,"ViewWin::Geometry: not mapped\n");
		Exit::DoExit(2);
	}
	if (!_hasGeometry){
		_windowRep->Dimensions(_width,_height);
		_windowRep->Origin(_x,_y);
		_hasGeometry = true;
	}
	x = _x;
	y = _y;
	w = _width;
	h = _height;
}

void ViewWin::AbsoluteOrigin(int &x, int &y) {
	if (_windowRep == NULL){
		fprintf(stderr,"ViewWin::AbsoluteOrigin: not mapped\n");
		Exit::DoExit(2);
	}
	_windowRep->AbsoluteOrigin(x,y);
}

/* Move/Resize ourselves */
void ViewWin::MoveResize(int x, int y, unsigned w, unsigned h){
/*
printf("ViewWin::MoveResize 0x%x,%d,%d,%d,%d\n", this,x,y,x+w-1,y+h-1);
*/

	if (! _mapped ) {
		fprintf(stderr,"ViewWin::MoveResize not mapped\n");
		Exit::DoExit(1);
	}
	else _windowRep->MoveResize(x,y,w,h);
}

int ViewWin::TotalWeight(){
	int w = 0;
	int index;
	for (index=InitIterator(); More(index);){
		ViewWin *vw= Next(index);
		w += vw->GetWeight();
	}
	DoneIterator(index);
	return w;
}

void ViewWin::HandleResize(WindowRep * w, int xlow,
	int ylow, unsigned width, unsigned height){

/*
printf("ViewWin::HandleResize 0x%x,%d,%d,%d,%d\n", this,xlow,ylow,width,height);
*/

	_hasGeometry = true;
	_x = xlow;
	_y = ylow;
	_width = width;
	_height = height;
}

void ViewWin::HandleWindowMappedInfo(WindowRep *, Boolean  mapped){
	_iconified = !mapped;
	Iconify(_iconified);
}


Boolean ViewWin::Iconified(){
	if (_parent != NULL){
		/* subwindow must query parent */
		return _parent->Iconified();
	}
	else {
		/* no parent: this is a top level window. */
		if (Mapped() && !_iconified)
			return false;
		else return true;
	}
}

/* Replace child1 by child2. child1 must be a valid child.
Update the list of children.*/
void ViewWin::Replace(ViewWin *child1, ViewWin *child2){
	int index;
	for (index = InitIterator(); More(index); ) {
		ViewWin *win = Next(index);
		if (win == child1){
			_children.InsertBeforeCurrent(index, child2);
			child2->SetParent(this);
			_children.DeleteCurrent(index);
			DoneIterator(index);
			child1->Unmap();
			child1->SetParent(NULL);
			return;
		}
	}
	fprintf(stderr,"ViewWin::Replace: can't find child1\n");
	Exit::DoExit(2);
}

/* Swap child1 and child2. Both must be valid children, and child1 != child2.*/
void ViewWin::SwapChildren(ViewWin *child1, ViewWin *child2) {
	_children.Swap(child1,child2);
}
