/* ViewWinHor.c */
#include "ViewWinHor.h"

ViewWinHor:: ViewWinHor(char *name,  Coord x, Coord y, Coord w, Coord h)
	:ViewWin(name,1, x,y,w,h,NULL){}

ViewWinHor::ViewWinHor(char *name, ViewWin *parent, int weight):
	ViewWin(name, weight,0.0,0.0,.9,.9,parent){}

inline unsigned WidthHeightToUse(unsigned h ){
	if (h > 2)
		return h-2;
	else return 1;
}

void ViewWinHor::Append(ViewWin *child){
	/* Resize the children */
	int num = NumChildren();
	int x,y; unsigned int w,h;
	Geometry(x,y,w,h);
	int weight = TotalWeight()+ child->GetWeight();
	int finalX= DoResize(weight,w,h);

	/* move/resize the new child */
	Coord widthIncrement= ((Coord)child->GetWeight())/weight*w;
	child->MoveResize(finalX,0,
		WidthHeightToUse((unsigned)widthIncrement),
		WidthHeightToUse((unsigned)h));

	/* append child */
	ViewWin::Append(child);
}

void ViewWinHor::Delete(ViewWin *child){
	ViewWin::Delete(child);
	int x,y; unsigned int w,h;
	Geometry(x,y,w,h);
	DoResize(TotalWeight(),w,h);
}


void ViewWinHor::HandleResize(WindowRep * w, int xlow,
		int ylow, unsigned width, unsigned height){
	DoResize(TotalWeight(), width, height);
}

/* Do actual resize of each child, assuming that the
total weight is as given. Return the final X position  */
int ViewWinHor::DoResize(int totalWeight, unsigned int width,
	unsigned int height){
	int num = NumChildren();
	int currentX = 0;
	if (num > 0){
		int index;
		for (index=InitIterator(); More(index);){
			ViewWin *vw = Next(index);
			Coord widthIncrement= ((Coord)vw->GetWeight())/totalWeight*width;
			vw->MoveResize(currentX,0, 
				WidthHeightToUse((unsigned)widthIncrement), 
				WidthHeightToUse((unsigned)height));
			currentX += (int)widthIncrement;
		}
		DoneIterator(index);
	}
	return currentX;
}

/*
void ViewWinHor::HandleKey(WindowRep * ,char key, int x, int y){
	printf("Key 0x%x,%c,%d,%d\n",this,key,x,y);
}
*/

