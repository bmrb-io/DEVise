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

  $Log$*/

#ifndef Map3D_h
#define Map3D_h

#include "DeviseTypes.h"
#include "Transform.h"
#include "Geom.h"
#include "VisualArg.h"

// ---------------------------------------------------------- 
// 3D data structures for blocks
// ---------------------------------------------------------- 

typedef POINT3D VERTEX;

class EDGE {
public:
  int p;  // start pt for an edge, refer to vertex index num
  int q;  // end   pt for an edge, refer to vertex index num
};

class SIDE {
public:
  int pt[4]; // corner points of a rectangular sides, refer to vertex index num
};

#define BLOCK_VERTEX 8
#define BLOCK_EDGES  12
#define BLOCK_SIDES  6

class BLOCK {               // cube shape
public:
  POINT3D pt;               // location (center) of block
  Coord   W, H, D;          // size of the block
  VERTEX  vt[BLOCK_VERTEX]; // shape's vertices
  EDGE    ed[BLOCK_EDGES];  // shape's edges
  SIDE    sd[BLOCK_SIDES];  // shape's sides
  Color   color;            // base color of shape
  Boolean clipout;          // true = clip the pt, false = keep it
};

// ---------------------------------------------------------- 
// 3D mapping functions
// ---------------------------------------------------------- 

class WindowRep;

class Map3D {
public:
  static void AssignBlockVertices(BLOCK &block);
  static void AssignBlockEdges(BLOCK &block);
  static void AssignBlockSides(BLOCK &block);
  static void MapBlockPoints(BLOCK *block_data, int numSyms, 
			     Camera camera, int H, int V);
  static void MapBlockSegments(BLOCK *block_data, int numSyms,
			       Camera camera, int H, int V);
  static void MapBlockPlanes(BLOCK *block_data, int numSyms,
			     Camera camera, int H, int V);

  // compute the location of a "user" point to the viewing
  // space's coordinates
  static POINT3D CompLocationOnViewingSpace(POINT3D &pt);

  // now the point in the viewing space coordinates, "flatten"
  // the 3D POINT3D into a 2D Point
  static Point CompProjectionOnViewingPlane(POINT3D &pt, Camera camera);

  static void DrawSegments(WindowRep *win);
  static void DrawPlanes(WindowRep *win);
  static void DrawRefAxis(WindowRep *win, Camera camera);

  // ---------------------------------------------------------- 

  static void MakeIdentity() {
    _transform[_current].MakeIdentity();
  }

  /* return the transform on top of the stack. */
  static Transform3D *TopTransform() {
    return &_transform[_current];
  }

  static void PostMultiply(Transform3D *t) {
    _transform[_current].PostMultiply(t);
  }

  static void Transform(Coord x, Coord y, Coord z, Coord &newX, 
			Coord &newY, Coord &newZ) {
    _transform[_current].Transform(x, y, z, newX, newY, newZ);
  }

  static void PrintTransform() {
    _transform[_current].Print();
  }

  // compute the viewing transformation matrix
  static void CompViewingTransf(Camera camera) {
    _transform[_current].SetViewMatrix(camera);
#ifdef DEBUG
    printf("WinR.h: Camera x = %.2f y = %.2f z = %.2f ", 
	   camera.x_, camera.y_, camera.z_);
    printf("fx = %.2f fy = %.2f fz = %.2f\n", camera.fx,
	   camera.fy, camera.fz);
    _transform[_current].Print();
    printf("_______ WindowRep.h: CompViewingTransf, _current =%d\n",
	   _current);
#endif
  }

  // ----------------------------------------------------------

  /* return distance between two points, squared */
  static Coord DistSq(const POINT3D &p1, const POINT3D &p2) {
    return (p1.x_ - p2.x_) * (p1.x_ - p2.x_)
           + (p1.y_ - p2.y_) * (p1.y_ - p2.y_)
	   + (p1.z_ - p2.z_) * (p1.z_ - p2.z_);
  }

  /* return distance between a triangular plane and a point, squared */
  static Coord TriDistSq(BLOCK &block, int v1, int v2, int v3,
			 POINT3D &p2) {
    POINT3D p1;
    p1.x_ = (block.vt[v1].x_ + block.vt[v2].x_ + block.vt[v3].x_) / 3;
    p1.y_ = (block.vt[v1].y_ + block.vt[v2].y_ + block.vt[v3].y_) / 3;
    p1.z_ = (block.vt[v1].z_ + block.vt[v2].z_ + block.vt[v3].z_) / 3;
    return DistSq(p1, p2);
  }

protected:
  /* draw a 3D axis */
  static void DrawAxis(WindowRep *win, POINT3D axisPt[4],
		       EDGE axis[3], Camera camera);

  static Transform3D _transform[1];
  static int _current;
};

#endif
