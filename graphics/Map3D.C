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
  Revision 1.17  1998/06/24 18:28:50  beyer
  changed #ifdef 0 to #if 0

  Revision 1.16  1998/05/28 15:04:58  wenger
  OpenGL cursors now drawn in view foreground color;
  fixes to OpenGL crashes with some sessions (bugs 342, 356?).

  Revision 1.15  1998/05/05 15:14:45  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.14  1998/03/05 08:10:24  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.13  1998/02/26 00:18:58  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.12  1997/12/16 17:53:48  zhenhai
  Added OpenGL features to graphics.

  Revision 1.11  1997/11/24 23:14:26  weaver
  Changes for the new ColorManager.

  Revision 1.10  1997/08/12 15:32:18  wenger
  Removed unnecessary include dependencies.

  Revision 1.9.10.1  1997/05/21 20:39:43  weaver
  Changes for new ColorManager

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
#if 0
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
#else
  screenPt.x = 0;
  screenPt.y = 0;
#endif

  return screenPt;
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
  axisPt[1].x_ = axisPt[0].x_ + 2.0;
  axisPt[1].y_ = axisPt[0].y_;
  axisPt[1].z_ = axisPt[0].z_;
  axisPt[2].x_ = axisPt[0].x_;
  axisPt[2].y_ = axisPt[0].y_ + 2.0;
  axisPt[2].z_ = axisPt[0].z_;
  axisPt[3].x_ = axisPt[0].x_;
  axisPt[3].y_ = axisPt[0].y_;
  axisPt[3].z_ = axisPt[0].z_ + 2.0;
  axis[0].p = 0;  axis[0].q = 1;  // x axis
  axis[1].p = 0;  axis[1].q = 2;  // y axis
  axis[2].p = 0;  axis[2].q = 3;  // z axis
  DrawAxis(win, axisPt, axis, camera);

  // draw the camera axis
#if 0
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
#endif
}

// ---------------------------------------------------------- 

void Map3D::DrawAxis(WindowRep *win, Point3D axisPt[4],
		     Edge3D axis[3], Camera camera)
{
#ifdef DEBUG
  printf("Begin DrawAxis\n");
#endif
#if 0
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
#endif
  for (int i=0; i<3; i++)
    win->Line3D(axisPt[axis[i].p].x_, axisPt[axis[i].p].y_,
		axisPt[axis[i].p].z_,
		axisPt[axis[i].q].x_, axisPt[axis[i].q].y_,
		axisPt[axis[i].q].z_,
		1.0);
  for (int i=0; i<3; i++)
    win->Line3D(-axisPt[axis[i].p].x_, -axisPt[axis[i].p].y_,
		-axisPt[axis[i].p].z_,
		-axisPt[axis[i].q].x_, -axisPt[axis[i].q].y_,
		-axisPt[axis[i].q].z_, 1.0);
  
  win->Text3D(axisPt[axis[0].q].x_,
	      axisPt[axis[0].q].y_,
	      axisPt[axis[0].q].z_, "X");
  win->Text3D(axisPt[axis[1].q].x_,
	      axisPt[axis[1].q].y_,
	      axisPt[axis[1].q].z_, "Y");
  win->Text3D(axisPt[axis[2].q].x_,
	      axisPt[axis[2].q].y_,
	      axisPt[axis[2].q].z_, "Z");
  win->Text3D(-axisPt[axis[0].q].x_,
	      axisPt[axis[0].q].y_,
	      axisPt[axis[0].q].z_, "-X");
  win->Text3D(axisPt[axis[1].q].x_,
	      -axisPt[axis[1].q].y_,
	      axisPt[axis[1].q].z_, "-Y");
  win->Text3D(axisPt[axis[2].q].x_,
	      axisPt[axis[2].q].y_,
	      -axisPt[axis[2].q].z_, "-Z");

#ifdef DEBUG
  printf("End DrawAxis\n");
#endif
}

//******************************************************************************
