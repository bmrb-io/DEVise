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

class Object3D {             // 3D object
public:
  Point3D  pt;               // location (center) of object
  Coord    W, H, D;          // size of object
  Coord    segWidth;         // width of frame segments
  Vertex3D vt[BLOCK_VERTEX]; // shape's vertices
  Edge3D   ed[BLOCK_EDGES];  // shape's edges
  Side3D   sd[BLOCK_SIDES];  // shape's sides
  Color    color;            // base color of shape
  Boolean  clipout;          // true = clip, false = keep it
};

// ---------------------------------------------------------- 
// 3D mapping functions
// ---------------------------------------------------------- 

class WindowRep;

class Map3D {
public:
  static void AssignBlockVertices(Object3D &block);
  static void AssignBlockEdges(Object3D &block);
  static void AssignBlockSides(Object3D &block);
  static void ClipBlocks(WindowRep *win,
                         Object3D *block, int numSyms, 
			 Camera camera, int H, int V);
  static void MapBlockSegments(WindowRep *win,
                               Object3D *block, int numSyms,
			       Camera camera, int H, int V);
  static void MapBlockPlanes(WindowRep *win,
                             Object3D *block, int numSyms,
			     Camera camera, int H, int V);
  static void ClipLineSegments(WindowRep *win,
                               Object3D *segment, int numSyms, 
	      		       Camera camera, int H, int V);
  static void MapLineSegments(WindowRep *win,
                              Object3D *segment, int numSyms,
			      Camera camera, int H, int V);

  // compute the location of a "user" point to the viewing
  // space's coordinates
  static Point3D CompLocationOnViewingSpace(WindowRep *win, Point3D &pt);

  // now the point in the viewing space coordinates, "flatten"
  // the 3D Point3D into a 2D Point
  static Point CompProjectionOnViewingPlane(Point3D &pt, Camera camera);

  static void DrawSegments(WindowRep *win);
  static void DrawPlanes(WindowRep *win, Boolean frame);
  static void DrawRefAxis(WindowRep *win, Camera camera);

  // ---------------------------------------------------------- 

  /* return distance between two points, squared */
  static Coord DistSq(const Point3D &p1, const Point3D &p2) {
    return (p1.x_ - p2.x_) * (p1.x_ - p2.x_)
           + (p1.y_ - p2.y_) * (p1.y_ - p2.y_)
	   + (p1.z_ - p2.z_) * (p1.z_ - p2.z_);
  }

  /* return distance between a triangular plane and a point, squared */
  static Coord TriDistSq(Object3D &block, int v1, int v2, int v3,
			 Point3D &p2) {
    Point3D p1;
    p1.x_ = (block.vt[v1].x_ + block.vt[v2].x_ + block.vt[v3].x_) / 3;
    p1.y_ = (block.vt[v1].y_ + block.vt[v2].y_ + block.vt[v3].y_) / 3;
    p1.z_ = (block.vt[v1].z_ + block.vt[v2].z_ + block.vt[v3].z_) / 3;
    return DistSq(p1, p2);
  }

protected:
  static void DrawAxis(WindowRep *win, Point3D axisPt[4],
		       Edge3D axis[3], Camera camera);
};

#endif
