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
  Revision 1.2  1996/06/21 19:49:31  jussi
  Minor change so that older compilers will accept the code.

  Revision 1.1  1996/06/21 19:25:22  jussi
  Initial revision.
*/

#include "Map3D.h"
#include "WindowRep.h"
#include "Temp.h"

//#define DEBUG

Transform3D Map3D::_transform[1];
int Map3D::_current = 0;

// This sort function is used by qsort(); the metric is the distance
// from a point to the camera; however, the objects are sorted in reverse
// order so that objects further away are drawn first

static int _reversePlaneSortFunction(const void *p1, const void *p2)
{
  const PLANE *pp1 = (PLANE *)p1;
  const PLANE *pp2 = (PLANE *)p2;
  Coord dist1 = pp1->dist;
  Coord dist2 = pp2->dist;
  if (dist1 < dist2)
    return +1;
  if (dist1 > dist2)
    return -1;
  return 0;
}

// ---------------------------------------------------------- 
// assign block data to vertices of the block
// ---------------------------------------------------------- 

// sample block = unit cube
//    cube center at (0, 0, 0), edge size = 1
//    pt0 (-0.5, -0.5,  0.5)       pt1( 0.5, -0.5,  0.5)
//    pt2 ( 0.5, -0.5, -0.5)       pt3(-0.5, -0.5, -0.5)
//    pt4 (-0.5,  0.5,  0.5)       pt5( 0.5,  0.5,  0.5)
//    pt6 ( 0.5,  0.5, -0.5)       pt7(-0.5,  0.5, -0.5)

void Map3D::AssignBlockVertices(BLOCK &block)
{
  Coord W = block.W / 2;
  Coord H = block.H / 2;
  Coord D = block.D / 2;

  block.vt[0].x_ = block.pt.x_ - W;
  block.vt[0].y_ = block.pt.y_ - H; 
  block.vt[0].z_ = block.pt.z_ + D;

  block.vt[1].x_ = block.pt.x_ + W;
  block.vt[1].y_ = block.pt.y_ - H;
  block.vt[1].z_ = block.pt.z_ + D;

  block.vt[2].x_ = block.pt.x_ + W;
  block.vt[2].y_ = block.pt.y_ - H;
  block.vt[2].z_ = block.pt.z_ - D;

  block.vt[3].x_ = block.pt.x_ - W;
  block.vt[3].y_ = block.pt.y_ - H;
  block.vt[3].z_ = block.pt.z_ - D;

  block.vt[4].x_ = block.pt.x_ - W;
  block.vt[4].y_ = block.pt.y_ + H;
  block.vt[4].z_ = block.pt.z_ + D;

  block.vt[5].x_ = block.pt.x_ + W;
  block.vt[5].y_ = block.pt.y_ + H;
  block.vt[5].z_ = block.pt.z_ + D;

  block.vt[6].x_ = block.pt.x_ + W;
  block.vt[6].y_ = block.pt.y_ + H;
  block.vt[6].z_ = block.pt.z_ - D;

  block.vt[7].x_ = block.pt.x_ - W;
  block.vt[7].y_ = block.pt.y_ + H;
  block.vt[7].z_ = block.pt.z_ - D;
}

// ---------------------------------------------------------- 
// assign block data to edges of the block
// ---------------------------------------------------------- 

void Map3D::AssignBlockEdges(BLOCK &block)
{
  block.ed[0].p  = 0; block.ed[0].q  = 1;
  block.ed[1].p  = 1; block.ed[1].q  = 2;
  block.ed[2].p  = 2; block.ed[2].q  = 3;
  block.ed[3].p  = 3; block.ed[3].q  = 0;
  block.ed[4].p  = 4; block.ed[4].q  = 5;
  block.ed[5].p  = 5; block.ed[5].q  = 6;
  block.ed[6].p  = 6; block.ed[6].q  = 7;
  block.ed[7].p  = 7; block.ed[7].q  = 4;
  block.ed[8].p  = 4; block.ed[8].q  = 0;
  block.ed[9].p  = 5; block.ed[9].q  = 1;
  block.ed[10].p = 6; block.ed[10].q = 2;
  block.ed[11].p = 7; block.ed[11].q = 3;
}

// ---------------------------------------------------------- 
// assign block data to sides of the block
// ---------------------------------------------------------- 

void Map3D::AssignBlockSides(BLOCK &block)
{
  block.sd[0].pt[0] = 0; block.sd[0].pt[1] = 1;
  block.sd[0].pt[2] = 2; block.sd[0].pt[3] = 3;
  block.sd[1].pt[0] = 0; block.sd[1].pt[1] = 3;
  block.sd[1].pt[2] = 7; block.sd[1].pt[3] = 4;
  block.sd[2].pt[0] = 0; block.sd[2].pt[1] = 4;
  block.sd[2].pt[2] = 5; block.sd[2].pt[3] = 1;
  block.sd[3].pt[0] = 1; block.sd[3].pt[1] = 5;
  block.sd[3].pt[2] = 6; block.sd[3].pt[3] = 2;
  block.sd[4].pt[0] = 2; block.sd[4].pt[1] = 6;
  block.sd[4].pt[2] = 7; block.sd[4].pt[3] = 3;
  block.sd[5].pt[0] = 4; block.sd[5].pt[1] = 7;
  block.sd[5].pt[2] = 6; block.sd[5].pt[3] = 5;
}

// ---------------------------------------------------------- 
// map vertices of blocks to 2D data points
// ---------------------------------------------------------- 

void Map3D::MapBlockPoints(BLOCK *block_data, int numSyms,
			   Camera camera, int H, int V)
{
  double x1 = 0.0, y1 = 0.0, z1 = 0.0;

  for(int i = 0; i < numSyms; i++) {

    block_data[i].clipout = false;  // default is no clip

    if (fabs(block_data[i].pt.x_ - camera.x_) <= block_data[i].W / 2
	&& fabs(block_data[i].pt.y_ - camera.y_) <= block_data[i].D / 2
	&& fabs(block_data[i].pt.z_ - camera.z_) <= block_data[i].H / 2) {
#ifdef DEBUG
      printf("Block %d clipped because camera is inside block\n", i);
#endif
      block_data[i].clipout = true;
      continue;
    }

#define _closeness_ 1.3

    if ((block_data[i].pt.x_ > 0 && camera.x_ > 0 && 
	 camera.x_ < block_data[i].pt.x_ && 
	 block_data[i].pt.x_ * _closeness_ > camera._rho) ||
	(block_data[i].pt.x_ < 0 && camera.x_ < 0 && 
	 camera.x_ > block_data[i].pt.x_ &&
	 block_data[i].pt.x_ * _closeness_ < -camera._rho) ||
	(block_data[i].pt.y_ > 0 && camera.y_ > 0 && 
	 camera.y_ < block_data[i].pt.y_ &&
	 block_data[i].pt.y_ * _closeness_ > camera._rho) ||
	(block_data[i].pt.y_ < 0 && camera.y_ < 0 && 
	 camera.y_ > block_data[i].pt.y_ &&
	 block_data[i].pt.y_ * _closeness_ < -camera._rho) ||
	(block_data[i].pt.z_ > 0 && camera.z_ > 0 && 
	 camera.z_ < block_data[i].pt.z_ &&
	 block_data[i].pt.z_ * _closeness_ > camera._rho) ||
	(block_data[i].pt.z_ < 0 && camera.z_ < 0 && 
	 camera.z_ > block_data[i].pt.z_ &&
	 block_data[i].pt.z_ * _closeness_ < -camera._rho)) {
#ifdef DEBUG
      printf("Block %d clipped because it's behind the camera\n", i);
#endif
      block_data[i].clipout = true;
      continue;
    }

    POINT3D tmppt = CompLocationOnViewingSpace(block_data[i].pt);
    Point pt = CompProjectionOnViewingPlane(tmppt, camera);
    if (fabs(pt.x) > (H * 1.2) || fabs(pt.y) > (V * 1.2)) {
#ifdef DEBUG
      printf("Block %d clipped because it's outside of screen\n", i);
#endif
      block_data[i].clipout = true;
      continue;
    }

#ifdef DEBUG
    printf("\tcamera:x = %f  y = %f  z = %f\n",
	   camera.x_, camera.y_, camera.z_);
    printf("\tblk.pt:x = %f  y = %f  z = %f\n",
	   block_data[i].pt.x_, block_data[i].pt.y_, block_data[i].pt.z_);
    printf("\ttmppt: x = %f  y = %f  z = %f\n",
	   tmppt.x_, tmppt.y_, tmppt.z_);
    printf("\tpt:    x = %f  y = %f\n", pt.x, pt.y);
    printf("\t       H = %d  V = %d\n", H, V);
#endif
  }
}

// ---------------------------------------------------------- 
// map edges of blocks to segments
// ---------------------------------------------------------- 

void Map3D::MapBlockSegments(BLOCK *block_data, int numSyms,
			     Camera camera, int H, int V)
{
  _numSegments = 0;

  for(int i = 0; i < numSyms; i++) {
    if (block_data[i].clipout)
      continue;

    Point pts[BLOCK_VERTEX];

    int j;
    for(j = 0; j < BLOCK_VERTEX; j++) {
      POINT3D tmppt = CompLocationOnViewingSpace(block_data[i].vt[j]);
      pts[j] = CompProjectionOnViewingPlane(tmppt, camera);
    }
    
    for(j = 0; j < BLOCK_EDGES; j++) {
      _segment[_numSegments].pt[0].x = pts[block_data[i].ed[j].p].x;
      _segment[_numSegments].pt[0].y = pts[block_data[i].ed[j].p].y;
      _segment[_numSegments].pt[1].x = pts[block_data[i].ed[j].q].x;
      _segment[_numSegments].pt[1].y = pts[block_data[i].ed[j].q].y;
      _numSegments++;
    }
  }
}

// ---------------------------------------------------------- 
// map sides of blocks to triangular planes
// ---------------------------------------------------------- 

void Map3D::MapBlockPlanes(BLOCK *block_data, int numSyms,
			   Camera camera, int H, int V)
{
  POINT3D cameraPoint;
  cameraPoint.x_ = camera.x_;
  cameraPoint.y_ = camera.y_;
  cameraPoint.z_ = camera.z_;

#ifdef DEBUG
  printf("Mapping planes of %d symbols\n", numSyms);
#endif

  _numPlanes = 0;

  for(int i = 0; i < numSyms; i++) {
    if (block_data[i].clipout)
      continue;

    Point pts[BLOCK_VERTEX];

    int j;
    for(j = 0; j < BLOCK_VERTEX; j++) {
      POINT3D tmppt = CompLocationOnViewingSpace(block_data[i].vt[j]);
      pts[j] = CompProjectionOnViewingPlane(tmppt, camera);
    }
    
    for(j = 0; j < BLOCK_SIDES; j++) {
      _plane[_numPlanes].pt[0].x = pts[block_data[i].sd[j].pt[0]].x;
      _plane[_numPlanes].pt[0].y = pts[block_data[i].sd[j].pt[0]].y;
      _plane[_numPlanes].pt[1].x = pts[block_data[i].sd[j].pt[1]].x;
      _plane[_numPlanes].pt[1].y = pts[block_data[i].sd[j].pt[1]].y;
      _plane[_numPlanes].pt[2].x = pts[block_data[i].sd[j].pt[2]].x;
      _plane[_numPlanes].pt[2].y = pts[block_data[i].sd[j].pt[2]].y;
      _plane[_numPlanes].dist = TriDistSq(block_data[i],
					  block_data[i].sd[j].pt[0],
					  block_data[i].sd[j].pt[1],
					  block_data[i].sd[j].pt[2],
					  cameraPoint);
      _plane[_numPlanes].color = block_data[i].color;
      _numPlanes++;
      _plane[_numPlanes].pt[0].x = pts[block_data[i].sd[j].pt[0]].x;
      _plane[_numPlanes].pt[0].y = pts[block_data[i].sd[j].pt[0]].y;
      _plane[_numPlanes].pt[1].x = pts[block_data[i].sd[j].pt[2]].x;
      _plane[_numPlanes].pt[1].y = pts[block_data[i].sd[j].pt[2]].y;
      _plane[_numPlanes].pt[2].x = pts[block_data[i].sd[j].pt[3]].x;
      _plane[_numPlanes].pt[2].y = pts[block_data[i].sd[j].pt[3]].y;
      _plane[_numPlanes].dist = TriDistSq(block_data[i],
					  block_data[i].sd[j].pt[0],
					  block_data[i].sd[j].pt[2],
					  block_data[i].sd[j].pt[3],
					  cameraPoint);
      _plane[_numPlanes].color = block_data[i].color;
      _numPlanes++;
    }
  }

  // sort the triangular planes back to front, based on their distance
  // from the camera
  qsort(_plane, _numPlanes, sizeof(PLANE), _reversePlaneSortFunction);
}

// ----------------------------------------------------------

POINT3D Map3D::CompLocationOnViewingSpace(POINT3D &pt)
{
  Transform3D tmpVec, tmpTransf;
  POINT3D NewPt;

  tmpVec.SetVector(pt);
  tmpTransf.Copy(*Map3D::TopTransform());
  tmpTransf.PreMultiply(&tmpVec);
  tmpTransf.GetVector(NewPt);
  return NewPt;
}

// ----------------------------------------------------------

Point Map3D::CompProjectionOnViewingPlane(POINT3D &viewPt, Camera camera)
{
  Point screenPt;
  double z_over_dvs = viewPt.z_ / camera._dvs;

#ifdef DEBUG
  printf ("CPOVP (2)-> vpt x = %f y = %f z = %f\n",
	  viewPt.x_,viewPt.y_,viewPt.z_);
#endif
  if (camera._perspective == 1) {
    screenPt.x = (fabs(viewPt.z_) == 0 ? viewPt.x_ :
		  viewPt.x_ / z_over_dvs);
    screenPt.y = (fabs(viewPt.z_) == 0 ? viewPt.y_ :
		  viewPt.y_ / z_over_dvs);
  } else {
    screenPt.x = viewPt.x_;
    screenPt.y = viewPt.y_;
  }

  // upper left-hand corner of the physical screen is the
  // default origin (0, 0) in the Xlib coordinate system
  // the following code will move the focus point to whereever
  // we don't it to be
  screenPt.x += camera.H;
  screenPt.y = (screenPt.y < 0 ?
		camera.V + fabs(screenPt.y) : camera.V - screenPt.y);

  return screenPt;
}

// ---------------------------------------------------------- 
// draw the edges of the shapes
// ---------------------------------------------------------- 

void Map3D::DrawSegments(WindowRep *win)
{
  for(int i = 0; i < _numSegments; i++) {
    win->Line(_segment[i].pt[0].x, _segment[i].pt[0].y,
	      _segment[i].pt[1].x, _segment[i].pt[1].y, 1);
  }
}

// ---------------------------------------------------------- 
// draw the sides of the shapes
// ---------------------------------------------------------- 

void Map3D::DrawPlanes(WindowRep *win)
{
  for(int i = 0; i < _numPlanes; i++) {
    win->SetFgColor(_plane[i].color);
    win->FillPoly(_plane[i].pt, 3);
    win->SetFgColor(BlackColor);
    win->Line(_plane[i].pt[0].x, _plane[i].pt[0].y,
	      _plane[i].pt[1].x, _plane[i].pt[1].y, 1);
    win->Line(_plane[i].pt[1].x, _plane[i].pt[1].y,
	      _plane[i].pt[2].x, _plane[i].pt[2].y, 1);
    win->Line(_plane[i].pt[2].x, _plane[i].pt[2].y,
	      _plane[i].pt[0].x, _plane[i].pt[0].y, 1);
  }
}

// ---------------------------------------------------------- 

void Map3D::DrawRefAxis(WindowRep *win, Camera camera)
{
  POINT3D axisPt[4]; // 0 == origin, 1 == on x, 2 = on y, 3 == on z
  EDGE axis[3];      // 0 == x, 1 == y, 2 = z

  // draw the reference axis
  axisPt[1].x_ = axisPt[3].z_ = 1.5;
  axisPt[2].y_ = axisPt[1].x_ * 1.5;
  axisPt[1].y_ = axisPt[1].z_ = 0.0;
  axisPt[2].x_ = axisPt[2].z_ = 0.0;
  axisPt[3].x_ = axisPt[3].y_ = 0.0;
  axisPt[0].x_ = axisPt[0].y_ = axisPt[0].z_ = 0.0;
  axis[0].p = 0;  axis[0].q = 1;  // x axis
  axis[1].p = 0;  axis[1].q = 2;  // y axis
  axis[2].p = 0;  axis[2].q = 3;  // z axis
  DrawAxis(win, axisPt, axis, camera);

  // draw the camera axis
  axisPt[0].x_ = camera.fx;
  axisPt[0].y_ = camera.fy;
  axisPt[0].z_ = camera.fz;
  axisPt[1].x_ = axisPt[0].x_ + 1.0;
  axisPt[1].y_ = axisPt[0].y_;
  axisPt[1].z_ = axisPt[0].z_;
  axisPt[2].x_ = axisPt[0].x_;
  axisPt[2].y_ = axisPt[0].y_ + 1.0;
  axisPt[2].z_ = axisPt[0].z_;
  axisPt[3].x_ = axisPt[0].x_;
  axisPt[3].y_ = axisPt[0].y_;
  axisPt[3].z_ = axisPt[0].z_ + 1.0;
  axis[0].p = 0;  axis[0].q = 1;  // x axis
  axis[1].p = 0;  axis[1].q = 2;  // y axis
  axis[2].p = 0;  axis[2].q = 3;  // z axis
  DrawAxis(win, axisPt, axis, camera);
}

// ---------------------------------------------------------- 

void Map3D::DrawAxis(WindowRep *win, POINT3D axisPt[4],
		     EDGE axis[3], Camera camera)
{
#ifdef DEBUG
  printf ("\t\t---------- Begin DrawAxis ---- \n");
#endif

  Point pt[4];
  for(int j = 0; j < 4; j++) {
    POINT3D sa_pts = CompLocationOnViewingSpace(axisPt[j]);
    pt[j] = CompProjectionOnViewingPlane(sa_pts, camera);
  }

  char *label[3] = { "X", "Y", "Z" };
  for(int i = 0; i < 3; i++) {
    // draw the axis
    win->Line(pt[axis[i].p].x, pt[axis[i].p].y,
	      pt[axis[i].q].x, pt[axis[i].q].y, 1);
    // label the axis
    const int fakeSize = 100;
    win->AbsoluteText(label[i],
		      pt[axis[i].q].x + 1 - fakeSize / 2,
		      pt[axis[i].q].y + 1 - fakeSize / 2,
		      fakeSize, fakeSize, WindowRep::AlignCenter,
		      false);
  }

#ifdef DEBUG
  printf ("\t\t---------- End DrawAxis ---- \n");
#endif
}
