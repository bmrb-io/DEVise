/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.7  1997/12/16 17:53:49  zhenhai
  Added OpenGL features to graphics.

  Revision 1.6  1997/11/24 23:14:27  weaver
  Changes for the new ColorManager.

  Revision 1.5.10.1  1997/05/21 20:39:57  weaver
  Changes for new ColorManager

  Revision 1.5  1996/11/13 16:56:08  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.4  1996/08/03 15:36:41  jussi
  Added line segment width.

  Revision 1.3  1996/07/20 18:47:59  jussi
  Added 3D line segment shape and renamed some 3D type names to
  be more general.

  Revision 1.2  1996/07/10 18:59:20  jussi
  Moved 3D transform variables to WindowRep.

  Revision 1.1  1996/06/21 19:25:23  jussi
  Initial revision.
*/

#ifndef Map3D_h
#define Map3D_h

#include "DeviseTypes.h"
#include "Geom.h"
#include "VisualArg.h"

#include "Coloring.h"

// ---------------------------------------------------------- 
// 3D data structures for blocks
// ---------------------------------------------------------- 

typedef Point3D Vertex3D;

class Edge3D {
public:
  int p;  // start pt for an edge, refer to vertex index num
  int q;  // end   pt for an edge, refer to vertex index num
};

class Side3D {
public:
  int pt[4]; // corner points of a rectangular sides, refer to vertex index num
};

#define BLOCK_VERTEX 8
#define BLOCK_EDGES  12
#define BLOCK_SIDES  6

// 5*12+2
#define SPHERE_VERTEX 62

// 5*12+6*12
#define SPHERE_EDGES 132

// 6*12
#define SPHERE_SIDES 72

class WindowRep;

// ---------------------------------------------------------- 
// 3D mapping functions
// ---------------------------------------------------------- 

//******************************************************************************
// class Map3D
//******************************************************************************

class Map3D {
public:
  // compute the location of a "user" point to the viewing
  // space's coordinates
  static Point3D CompLocationOnViewingSpace(WindowRep *win, Point3D &pt);

  // now the point in the viewing space coordinates, "flatten"
  // the 3D Point3D into a 2D Point
  static Point CompProjectionOnViewingPlane(Point3D &pt, Camera camera);

  static void DrawRefAxis(WindowRep *win, Camera camera);

  // ---------------------------------------------------------- 

  /* return distance between two points, squared */
  static Coord DistSq(const Point3D &p1, const Point3D &p2) {
    return (p1.x_ - p2.x_) * (p1.x_ - p2.x_)
           + (p1.y_ - p2.y_) * (p1.y_ - p2.y_)
	   + (p1.z_ - p2.z_) * (p1.z_ - p2.z_);
  }

protected:
  static void DrawAxis(WindowRep *win, Point3D axisPt[4],
		       Edge3D axis[3], Camera camera);
};

//******************************************************************************
#endif
