/*
  $Id$

  $Log$*/

#ifndef DeviseCursor_h
#define DeviseCursor_h
#include "DList.h"
#include "Color.h"
#include "VisualArg.h"
#include "ViewCallback.h"

class DeviseCursor;
class View;
DefinePtrDList(DeviseCursorList, DeviseCursor *)

class DeviseCursor : private ViewCallback{
public:
	DeviseCursor(char *name, VisualFlag flag, Color color = HighlightColor); 
	virtual ~DeviseCursor();

	/* Set source view. Changing this view's visual filter
	changes in the cursor's position. Can also set to NULL */
	void SetSource(View *view);
	View *GetSource() { return _src; }


	/* Set destination: this is where the cursor will be drawn.
	Can also set to NULL. */
	void SetDst(View *view);
	View *GetDst() { return _dst; }

	/* Get current visual filter. return TRUE if it exists. */
	Boolean GetVisualFilter(VisualFilter *&filter, Color &color);

	/* Move the X coord of source */
	void MoveSourceX(Coord x);

private:
    /* Called by View when its visual filter has changed.
	flushed == index of 1st element in the history that has been flushed,
	or -1 if none. index 0 == oldest element .*/
	virtual void FilterChanged(View *view, VisualFilter &filter,
			int flushed);

	void ViewCreated(View *view);
	void ViewDestroyed(View *view);

char *_name;
VisualFlag _visFlag;
View *_src, *_dst;
VisualFilter _filter;
Color _color;
};

#endif
