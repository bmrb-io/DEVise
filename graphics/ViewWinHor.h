/*
  $Id$

  $Log$*/

/* stacks its children vertically */

#ifndef ViewWinHor_h
#define ViewWinHor_h
#include "ViewWin.h"
#include "WindowRep.h"

class ViewWinHor: public ViewWin{
public:
	/* For creating a top-level window */
	ViewWinHor(char *name,  Coord x=0.0, Coord y=0.2, 
		Coord w=.2, Coord h=.7);
	/* For creating sub-window */
	ViewWinHor(char *name,  ViewWin *parent,int weight=1);


	virtual void Append(ViewWin *child);
	virtual void Delete(ViewWin *child);
private:
	/* DoResize. Return final X position */
	int DoResize(int totalWeight, unsigned int width, unsigned int height);

	/* from WindowRepCallback */
	/* Handle resize by resizing its children */
	virtual void HandleResize(WindowRep * w, int xlow,
		int ylow, unsigned width, unsigned height);

	/*
	virtual void HandleKey(WindowRep * ,char key, int x, int y);
	*/


};

#endif
