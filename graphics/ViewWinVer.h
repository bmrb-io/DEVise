/*
  $Id$

  $Log$*/

/* stacks its children vertically */

#ifndef ViewWinVer_h
#define ViewWinVer_h
#include "ViewWin.h"
#include "WindowRep.h"

class ViewWinVer: public ViewWin{
public:
	/* For creating a top-level window */
	ViewWinVer(char *name,  Coord x=0.1, Coord y=0.0, 
		Coord w=.8, Coord h=.9);
	/* For creating a sub-window */
	ViewWinVer(char *name, int weight);
	~ViewWinVer();

    /* These are called by parents */
	virtual void Map(int x, int y, unsigned w, unsigned h);
	virtual void Unmap();
	void Iconify(Boolean iconified);


	virtual void Append(ViewWin *child);
	virtual void Delete(ViewWin *child);
	virtual void Replace(ViewWin *child1, ViewWin *child2);
	virtual void SwapChildren(ViewWin *child1, ViewWin *child2);


protected:
private:
    virtual void SubClassMapped(){};
	virtual void SubClassUnmapped(){};

	/* DoResize. Return final Y position */
	int DoResize(int totalWeight, unsigned int width, unsigned int height);

	/* Map the children */
	void MapChildren(int totalWeight, unsigned int width, unsigned int height);
	void UnmapChildren();


	/* from WindowRepCallback */
	/* Handle resize by resizing its children */
	virtual void HandleResize(WindowRep * w, int xlow,
		int ylow, unsigned width, unsigned height);

	/*
	virtual void HandleKey(WindowRep * ,char key, int x, int y);
	*/


};

#endif
