/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.7  1997/11/24 23:14:52  weaver
  Changes for the new ColorManager.

  Revision 1.6.10.1  1997/05/21 20:40:27  weaver
  Changes for new ColorManager

  Revision 1.6  1996/07/14 04:08:02  jussi
  ViewKGraph is now derived from KGraph. Renamed some KGraph
  member functions so they don't collide with ViewKGraph
  functions.

  Revision 1.5  1996/05/31 20:59:07  jussi
  Added ClearGraph() method and renamed 'rad' to 'diam'.

  Revision 1.4  1995/12/16 00:48:20  ravim
  Handles window events for popup and resize.

  Revision 1.3  1995/12/08 23:44:53  ravim
  Polygon filled.

  Revision 1.2  1995/12/06 05:44:38  ravim
  Initial version.
*/

//******************************************************************************
//
//******************************************************************************

#ifndef __KGRAPH_H
#define __KGRAPH_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "Geom.h"
#include "WindowRep.h"

#include "Color.h"

//******************************************************************************
// class KGraph
//******************************************************************************

class KGraph : protected WindowRepCallback
{
	public:

		// Names for data colors 0-3 in the coloring
		enum { circleColor, pointColor, textColor, labelColor };

	private:

		Coloring	coloring;

	public:

		// Constructors and Destructors
		KGraph(Coloring c = GetKGraphDefColoring());
		virtual ~KGraph(void);

		// Getters and Setters
		Coloring&				GetColoring(void)		{ return coloring;	}
		const Coloring&			GetColoring(void) const { return coloring;	}

		
  // Initialize the settings
  void InitGraph(char *winname, char *statname);

  // Set the number of axes to display
  void SetAxes(int num);

  // Specify points along the axes = number of axes in the graph
  void SetPoints(Coord *pts, int num);

  // Display on the graph
  void DisplayGraph();

protected:
  WindowRep *_win;
  char *_winame;
  char *_statname;
  int _naxes;
  Coord *_pts;
  Point *_xyarr;

  // Coords of center of circle and diameter
  Coord _cx, _cy;
  int _diam;

  void ClearGraph();
  void DrawCircle();
  void DrawAxes();
  void PlotPoints();
  void ShowVal();
  void Rotate(Coord xval, int degree, Coord &retx, Coord &rety);
  Coord Scale(Coord xval, Coord max);

  char **_msgBuf;

	protected:

		// Callback methods (WindowRepCallback)
		virtual void	HandlePress(WindowRep* w, int x1, int y1, int x2,
									int y2, int button);
		virtual void	HandleResize(WindowRep* w, int xlow, int ylow,
									 unsigned width, unsigned height);
		virtual Boolean	HandlePopUp(WindowRep* w, int x, int y, int button, 
									char**& msgs, int& numMsgs);
};

//******************************************************************************
#endif
