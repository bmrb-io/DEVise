/*
  $Id$

  $Log$*/

#include "ViewWinVer.h"
#include "Display.h"

/* top level window */
ViewWinVer:: ViewWinVer(char *name,  Coord x, Coord y, Coord w, Coord h)
	:ViewWin(name,1){
	Coord width, height;
	DeviseDisplay::DefaultDisplay()->Dimensions(width,height);
	Map((int)(x*width),(int)(y*height),(unsigned)(w*width),
		(unsigned)(h*height));
}

/* sub window */
ViewWinVer::ViewWinVer(char *name, int weight):
	ViewWin(name, weight, 1){}

ViewWinVer::~ViewWinVer(){
	DeleteFromParent();
	Unmap();
}


inline unsigned WidthHeightToUse(unsigned h){
	if (h > 2)
		return h-2;
	else return 1;
}

void ViewWinVer::Map(int x, int y, unsigned w, unsigned h) {
	ViewWin::Map(x,y,w,h);

	/* Map the children */
	int x,y; unsigned int w,h;
	Geometry(x,y,w,h);
	MapChildren(TotalWeight(), w, h);
}

void ViewWinVer::Unmap(){
	/* Unmap the children */
	UnmapChildren();

	ViewWin::Unmap();
}

void ViewWinVer::Append(ViewWin *child){
	if (Mapped()){
		/* Resize the children */
		int num = NumChildren();
		int x,y; unsigned int w,h;
		Geometry(x,y,w,h);
		int weight = TotalWeight()+ child->GetWeight();
		int finalY= DoResize(weight,w,h);

		/* move/resize the new child */
		Coord heightIncrement= ((Coord)child->GetWeight())/weight*h;
		child->Map(0,finalY,
			WidthHeightToUse(w),WidthHeightToUse((unsigned)heightIncrement));
	}

	/* append child */
	ViewWin::Append(child);
}

void ViewWinVer::Delete(ViewWin *child){
	ViewWin::Delete(child);

	if (Mapped() ){
		child->Unmap();
		int x,y; unsigned int w,h;
		Geometry(x,y,w,h);
		DoResize(TotalWeight(),w,h);
	}
}


void ViewWinVer::HandleResize(WindowRep * w, int xlow,
		int ylow, unsigned width, unsigned height){
	ViewWin::HandleResize(w,xlow,ylow,width,height);
	if (Mapped())
		DoResize(TotalWeight(), width, height);
}

/* Do actual resize of each child, assuming that the
total weight is as given. Return the final Y position  */
int ViewWinVer::DoResize(int totalWeight, unsigned int width,
	unsigned int height){
	int num = NumChildren();
	int currentY = 0;
	if (num > 0){
		int index;
		for (index=InitIterator(); More(index);){
			ViewWin *vw = Next(index);
			Coord heightIncrement= ((Coord)vw->GetWeight())/totalWeight*height;
			vw->MoveResize(0,currentY, 
				WidthHeightToUse(width), 
				WidthHeightToUse((unsigned)heightIncrement));
			currentY += (int)heightIncrement;
		}
		DoneIterator(index);
	}
	return currentY;
}

/* Do actual mapping of each child, assuming
total weight is as given. Return the final Y position  */
void ViewWinVer::MapChildren(int totalWeight, unsigned int width,
	unsigned int height){
	int num = NumChildren();
	int currentY = 0;
	if (num > 0){
		int index;
		for (index=InitIterator(); More(index);){
			ViewWin *vw = Next(index);
			Coord heightIncrement= ((Coord)vw->GetWeight())/totalWeight*height;
			vw->Map(0,currentY, 
				WidthHeightToUse(width), 
				WidthHeightToUse((unsigned)heightIncrement));
			currentY += (int)heightIncrement;
		}
		DoneIterator(index);
	}
}

void ViewWinVer::UnmapChildren(){
	int index;
	for (index=InitIterator(); More(index);){
		ViewWin *vw = Next(index);
		vw->Unmap();
	}
	DoneIterator(index);
}

/*
void ViewWinVer::HandleKey(WindowRep * ,char key, int x, int y){
	printf("Key 0x%x,%c,%d,%d\n",this,key,x,y);
}
*/

void ViewWinVer::Iconify(Boolean iconified){
	int index;
	for (index=InitIterator(); More(index);){
		ViewWin *vw = Next(index);
		vw->Iconify(iconified);
	}
	DoneIterator(index);
}

/* Replace child1 by child2. child1 must be a valid child. */
void ViewWinVer::Replace(ViewWin *child1, ViewWin *child2) {
/*
printf("ViewWinVer::Replace: %s %s\n", child1->GetName(),
	child2->GetName());
*/
	ViewWin::Replace(child1, child2);
/*
printf("ViewWinVer::Replace 2\n");
*/
	if (Mapped()){
/*
printf("viewWin mapped\n");
*/
		int num = NumChildren();
		int currentY = 0;
		int totalWeight = TotalWeight();
		int x,y; unsigned int width,height;
		Geometry(x,y,width,height);

		if (num > 0){
			int index;
			for (index=InitIterator(); More(index);){
				ViewWin *vw = Next(index);
				Coord heightIncrement= ((Coord)vw->GetWeight())/totalWeight*height;
				if (vw == child2){
printf("map 0 %d %d %f\n", currentY, width, heightIncrement);
					vw->Map(0,currentY, 
						WidthHeightToUse(width), 
						WidthHeightToUse((unsigned)heightIncrement));
					break;
				}
				currentY += (int)heightIncrement;
			}
			DoneIterator(index);
		}
	}
	else printf("not mapped\n");
}

/* Replace child1 by child2 */
void ViewWinVer::SwapChildren(ViewWin *child1, ViewWin *child2){
	ViewWin::SwapChildren(child1,child2);
	if (Mapped()){
		int x1,y1,x2,y2;
		unsigned w1,w2,h1,h2;
		child1->Geometry(x1,y1,w1,h1);
		child2->Geometry(x2,y2,w2,h2);
		child1->MoveResize(x2,y2,w1,h1);
		child2->MoveResize(x1,y1,w2,h2);
	}
}
