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
  Revision 1.9  1996/11/13 16:56:08  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.8  1996/08/03 15:35:42  jussi
  Added line segment width and commented out line segment clipping
  algorithm.

  Revision 1.7  1996/07/20 18:47:58  jussi
  Added 3D line segment shape and renamed some 3D type names to
  be more general.

  Revision 1.6  1996/07/13 17:24:44  jussi
  Diagonal line across the side of a block is not drawn.

  Revision 1.5  1996/07/10 18:59:18  jussi
  Moved 3D transform variables to WindowRep.

  Revision 1.4  1996/06/27 22:54:41  jussi
  Added support for XOR color value.

  Revision 1.3  1996/06/23 18:35:24  jussi
  Block height is now mapped to the Y axis and block depth to
  the Z axis. Y and Z were reversed.

  Revision 1.2  1996/06/21 19:49:31  jussi
  Minor change so that older compilers will accept the code.

  Revision 1.1  1996/06/21 19:25:22  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Map3D.h"
#include "WindowRep.h"
#include "Temp.h"

//#define DEBUG

// This sort function is used by qsort(); the metric is the distance
// from a point to the camera; however, the objects are sorted in reverse
// order so that objects further away are drawn first

static int _reversePlaneSortFunction(const void *p1, const void *p2)
{
  const Plane *pp1 = (Plane *)p1;
  const Plane *pp2 = (Plane *)p2;
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

void Map3D::AssignBlockVertices(Object3D &block)
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

void Map3D::AssignBlockEdges(Object3D &block)
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

void Map3D::AssignBlockSides(Object3D &block)
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
// clip blocks
// ---------------------------------------------------------- 

void Map3D::ClipBlocks(WindowRep *win,
                       Object3D *block, int numSyms,
                       Camera camera, int H, int V)
{
  double x1 = 0.0, y1 = 0.0, z1 = 0.0;

  for(int i = 0; i < numSyms; i++) {

    block[i].clipout = false;  // default is no clip

    if (fabs(block[i].pt.x_ - camera.x_) <= block[i].W / 2
	&& fabs(block[i].pt.y_ - camera.y_) <= block[i].D / 2
	&& fabs(block[i].pt.z_ - camera.z_) <= block[i].H / 2) {
#ifdef DEBUG
      printf("Block %d clipped because camera is inside block\n", i);
#endif
      block[i].clipout = true;
      continue;
    }

#define _closeness_ 1.3

    if ((block[i].pt.x_ > 0 && camera.x_ > 0 && 
	 camera.x_ < block[i].pt.x_ && 
	 block[i].pt.x_ * _closeness_ > camera._rho) ||
	(block[i].pt.x_ < 0 && camera.x_ < 0 && 
	 camera.x_ > block[i].pt.x_ &&
	 block[i].pt.x_ * _closeness_ < -camera._rho) ||
	(block[i].pt.y_ > 0 && camera.y_ > 0 && 
	 camera.y_ < block[i].pt.y_ &&
	 block[i].pt.y_ * _closeness_ > camera._rho) ||
	(block[i].pt.y_ < 0 && camera.y_ < 0 && 
	 camera.y_ > block[i].pt.y_ &&
	 block[i].pt.y_ * _closeness_ < -camera._rho) ||
	(block[i].pt.z_ > 0 && camera.z_ > 0 && 
	 camera.z_ < block[i].pt.z_ &&
	 block[i].pt.z_ * _closeness_ > camera._rho) ||
	(block[i].pt.z_ < 0 && camera.z_ < 0 && 
	 camera.z_ > block[i].pt.z_ &&
	 block[i].pt.z_ * _closeness_ < -camera._rho)) {
#ifdef DEBUG
      printf("Block %d clipped because it's behind the camera\n", i);
#endif
      block[i].clipout = true;
      continue;
    }

    // If any of the vertices of the block appear on screen,
    // the block is not clipped, otherwise it is clipped
    block[i].clipout = true;
    for(int j = 0; j < BLOCK_VERTEX; j++) {
      Point3D tmppt = CompLocationOnViewingSpace(win, block[i].vt[j]);
      Point pt = CompProjectionOnViewingPlane(tmppt, camera);
      if (fabs(pt.x) <= H && fabs(pt.y) <= V) {
        block[i].clipout = false;
        break;
      }
    }
          
    if (block[i].clipout) {
#ifdef DEBUG
      printf("Block %d clipped because it's outside of screen\n", i);
#endif
    }
  }
}

// ---------------------------------------------------------- 
// map edges of blocks to segments
// ---------------------------------------------------------- 

void Map3D::MapBlockSegments(WindowRep *win,
                             Object3D *block, int numSyms,
			     Camera camera, int H, int V)
{
  _numSegments = 0;

  for(int i = 0; i < numSyms; i++) {
    if (block[i].clipout)
      continue;

    Point pts[BLOCK_VERTEX];

    int j;
    for(j = 0; j < BLOCK_VERTEX; j++) {
      Point3D tmppt = CompLocationOnViewingSpace(win, block[i].vt[j]);
      pts[j] = CompProjectionOnViewingPlane(tmppt, camera);
    }
    
    for(j = 0; j < BLOCK_EDGES; j++) {
      _segment[_numSegments].pt[0].x = pts[block[i].ed[j].p].x;
      _segment[_numSegments].pt[0].y = pts[block[i].ed[j].p].y;
      _segment[_numSegments].pt[1].x = pts[block[i].ed[j].q].x;
      _segment[_numSegments].pt[1].y = pts[block[i].ed[j].q].y;
      _segment[_numSegments].width = block[i].segWidth;
      _segment[_numSegments].color = block[i].color;
      _numSegments++;
    }
  }
}

// ---------------------------------------------------------- 
// map sides of blocks to triangular planes
// ---------------------------------------------------------- 

void Map3D::MapBlockPlanes(WindowRep *win,
                           Object3D *block, int numSyms,
			   Camera camera, int H, int V)
{
  Point3D cameraPoint;
  cameraPoint.x_ = camera.x_;
  cameraPoint.y_ = camera.y_;
  cameraPoint.z_ = camera.z_;

#ifdef DEBUG
  printf("Mapping planes of %d symbols\n", numSyms);
#endif

  _numPlanes = 0;

  for(int i = 0; i < numSyms; i++) {
    if (block[i].clipout)
      continue;

    Point pts[BLOCK_VERTEX];

    int j;
    for(j = 0; j < BLOCK_VERTEX; j++) {
      Point3D tmppt = CompLocationOnViewingSpace(win, block[i].vt[j]);
      pts[j] = CompProjectionOnViewingPlane(tmppt, camera);
    }
    
    for(j = 0; j < BLOCK_SIDES; j++) {
      _plane[_numPlanes].pt[0].x = pts[block[i].sd[j].pt[0]].x;
      _plane[_numPlanes].pt[0].y = pts[block[i].sd[j].pt[0]].y;
      _plane[_numPlanes].pt[1].x = pts[block[i].sd[j].pt[1]].x;
      _plane[_numPlanes].pt[1].y = pts[block[i].sd[j].pt[1]].y;
      _plane[_numPlanes].pt[2].x = pts[block[i].sd[j].pt[2]].x;
      _plane[_numPlanes].pt[2].y = pts[block[i].sd[j].pt[2]].y;
      _plane[_numPlanes].dist = TriDistSq(block[i],
					  block[i].sd[j].pt[0],
					  block[i].sd[j].pt[1],
					  block[i].sd[j].pt[2],
					  cameraPoint);
      _plane[_numPlanes].color = block[i].color;
      _numPlanes++;
      _plane[_numPlanes].pt[0].x = pts[block[i].sd[j].pt[2]].x;
      _plane[_numPlanes].pt[0].y = pts[block[i].sd[j].pt[2]].y;
      _plane[_numPlanes].pt[1].x = pts[block[i].sd[j].pt[3]].x;
      _plane[_numPlanes].pt[1].y = pts[block[i].sd[j].pt[3]].y;
      _plane[_numPlanes].pt[2].x = pts[block[i].sd[j].pt[0]].x;
      _plane[_numPlanes].pt[2].y = pts[block[i].sd[j].pt[0]].y;
      _plane[_numPlanes].dist = TriDistSq(block[i],
					  block[i].sd[j].pt[0],
					  block[i].sd[j].pt[2],
					  block[i].sd[j].pt[3],
					  cameraPoint);
      _plane[_numPlanes].color = block[i].color;
      _numPlanes++;
    }
  }

  // sort the triangular planes back to front, based on their distance
  // from the camera
  qsort(_plane, _numPlanes, sizeof(Plane), _reversePlaneSortFunction);
}

// ---------------------------------------------------------- 
// clip segments
// ---------------------------------------------------------- 

void Map3D::ClipLineSegments(WindowRep *win,
                             Object3D *segment, int numSyms,
                             Camera camera, int H, int V)
{
  double x1 = 0.0, y1 = 0.0, z1 = 0.0;

  for(int i = 0; i < numSyms; i++) {

    segment[i].clipout = false;  // default is no clip

#if 0
#define _closeness_ 1.3

    if ((segment[i].pt.x_ > 0 && camera.x_ > 0 && 
	 camera.x_ < segment[i].pt.x_ && 
	 segment[i].pt.x_ * _closeness_ > camera._rho) ||
	(segment[i].pt.x_ < 0 && camera.x_ < 0 && 
	 camera.x_ > segment[i].pt.x_ &&
	 segment[i].pt.x_ * _closeness_ < -camera._rho) ||
	(segment[i].pt.y_ > 0 && camera.y_ > 0 && 
	 camera.y_ < segment[i].pt.y_ &&
	 segment[i].pt.y_ * _closeness_ > camera._rho) ||
	(segment[i].pt.y_ < 0 && camera.y_ < 0 && 
	 camera.y_ > segment[i].pt.y_ &&
	 segment[i].pt.y_ * _closeness_ < -camera._rho) ||
	(segment[i].pt.z_ > 0 && camera.z_ > 0 && 
	 camera.z_ < segment[i].pt.z_ &&
	 segment[i].pt.z_ * _closeness_ > camera._rho) ||
	(segment[i].pt.z_ < 0 && camera.z_ < 0 && 
	 camera.z_ > segment[i].pt.z_ &&
	 segment[i].pt.z_ * _closeness_ < -camera._rho)) {
#ifdef DEBUG
      printf("Segment %d clipped because it's behind the camera\n", i);
#endif
      segment[i].clipout = true;
      continue;
    }
#endif

    // If the center of the segment appears on screen,
    // the segment is not clipped, otherwise it is clipped.
    segment[i].clipout = true;
    Point3D tmppt = CompLocationOnViewingSpace(win, segment[i].pt);
    Point pt = CompProjectionOnViewingPlane(tmppt, camera);
    if (fabs(pt.x) <= H && fabs(pt.y) <= V) {
        segment[i].clipout = false;
    }
          
    if (segment[i].clipout) {
#ifdef DEBUG
      printf("Segment %d clipped because it's outside of screen\n", i);
#endif
    }
  }
}

// ---------------------------------------------------------- 
// map 3D segments to 2D segments
// ---------------------------------------------------------- 

void Map3D::MapLineSegments(WindowRep *win,
                            Object3D *segment, int numSyms,
                            Camera camera, int H, int V)
{
  _numSegments = 0;

  for(int i = 0; i < numSyms; i++) {
    if (segment[i].clipout)
      continue;

    Point3D pt1, pt2;
    pt1.x_ = segment[i].pt.x_ - segment[i].W / 2;
    pt2.x_ = segment[i].pt.x_ + segment[i].W / 2;
    pt1.y_ = segment[i].pt.y_ - segment[i].H / 2;
    pt2.y_ = segment[i].pt.y_ + segment[i].H / 2;
    pt1.z_ = segment[i].pt.z_ - segment[i].D / 2;
    pt2.z_ = segment[i].pt.z_ + segment[i].D / 2;

    Point3D tmppt = CompLocationOnViewingSpace(win, pt1);
    Point pt = CompProjectionOnViewingPlane(tmppt, camera);
    _segment[_numSegments].pt[0].x = pt.x;
    _segment[_numSegments].pt[0].y = pt.y;

    tmppt = CompLocationOnViewingSpace(win, pt2);
    pt = CompProjectionOnViewingPlane(tmppt, camera);
    _segment[_numSegments].pt[1].x = pt.x;
    _segment[_numSegments].pt[1].y = pt.y;

    _segment[_numSegments].width = segment[i].segWidth;
    _segment[_numSegments].color = segment[i].color;

    _numSegments++;
  }
}

// ----------------------------------------------------------

Point3D Map3D::CompLocationOnViewingSpace(WindowRep *win, Point3D &pt)
{
  Transform3D tmpVec, tmpTransf;
  Point3D NewPt;

  tmpVec.SetVector(pt);
  tmpTransf.Copy(*win->TopTransform3());
  tmpTransf.PreMultiply(&tmpVec);
  tmpTransf.GetVector(NewPt);
  return NewPt;
}

// ----------------------------------------------------------

Point Map3D::CompProjectionOnViewingPlane(Point3D &viewPt, Camera camera)
{
  Point screenPt;
  double z_over_dvs = viewPt.z_ / camera._dvs;

  if (camera._perspective) {
    screenPt.x = (z_over_dvs == 0 ? viewPt.x_ : viewPt.x_ / z_over_dvs);
    screenPt.y = (z_over_dvs == 0 ? viewPt.y_ : viewPt.y_ / z_over_dvs);
  } else {
    screenPt.x = viewPt.x_;
    screenPt.y = viewPt.y_;
  }

  // Upper left-hand corner of the physical screen is the
  // default origin (0, 0) in the Xlib coordinate system.
  // The following code will move the focus point to whereever
  // we want it to be.
  screenPt.x += camera.H;
  screenPt.y = camera.V - screenPt.y;

  return screenPt;
}

// ---------------------------------------------------------- 
// draw the edges of the shapes
// ---------------------------------------------------------- 

void Map3D::DrawSegments(WindowRep *win)
{
  for(int i = 0; i < _numSegments; i++) {
    GlobalColor color = _segment[i].color;
    if (color == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(color);
    win->Line(_segment[i].pt[0].x, _segment[i].pt[0].y,
	      _segment[i].pt[1].x, _segment[i].pt[1].y,
              _segment[i].width);
    if (color == XorColor)
      win->SetCopyMode();
  }
}

// ---------------------------------------------------------- 
// draw the sides of the shapes
// ---------------------------------------------------------- 

void Map3D::DrawPlanes(WindowRep *win, Boolean frame)
{
  for(int i = 0; i < _numPlanes; i++) {
    GlobalColor color = _plane[i].color;
    if (color == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(color);
    win->FillPoly(_plane[i].pt, 3);
    if (color == XorColor)
      win->SetCopyMode();

    if (frame) {
      win->SetFgColor(BlackColor);
      win->Line(_plane[i].pt[0].x, _plane[i].pt[0].y,
                _plane[i].pt[1].x, _plane[i].pt[1].y, 1);
      win->Line(_plane[i].pt[1].x, _plane[i].pt[1].y,
                _plane[i].pt[2].x, _plane[i].pt[2].y, 1);
#if 0
      // don't draw diagonal line
      win->Line(_plane[i].pt[2].x, _plane[i].pt[2].y,
                _plane[i].pt[0].x, _plane[i].pt[0].y, 1);
#endif
    }
  }
}

// ---------------------------------------------------------- 

void Map3D::DrawRefAxis(WindowRep *win, Camera camera)
{
  Point3D axisPt[4]; // 0 == origin, 1 == on x, 2 = on y, 3 == on z
  Edge3D axis[3];    // 0 == x, 1 == y, 2 = z

  // draw the reference axis
  axisPt[0].x_ = 0.0;
  axisPt[0].y_ = 0.0;
  axisPt[0].z_ = 0.0;
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

void Map3D::DrawAxis(WindowRep *win, Point3D axisPt[4],
		     Edge3D axis[3], Camera camera)
{
#ifdef DEBUG
  printf("Begin DrawAxis\n");
#endif

  Point pt[4];
  for(int j = 0; j < 4; j++) {
    Point3D sa_pts = CompLocationOnViewingSpace(win, axisPt[j]);
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
  printf("End DrawAxis\n");
#endif
}
