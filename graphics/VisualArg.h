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
  Revision 1.11  1998/04/10 18:29:12  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.10  1998/03/18 08:19:44  zhenhai
  Added visual links between 3D graphics.

  Revision 1.9  1998/03/08 00:00:54  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.8  1998/02/26 22:59:37  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

  Revision 1.7  1997/12/16 17:53:53  zhenhai
  Added OpenGL features to graphics.

  Revision 1.6  1997/11/24 23:14:37  weaver
  Changes for the new ColorManager.

  Revision 1.5.10.1  1997/05/21 20:40:06  weaver
  Changes for new ColorManager

  Revision 1.5  1996/11/13 16:56:16  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.4  1996/06/15 07:08:14  yuc
  Add Camera structure to the system.

  Revision 1.3  1996/05/31 15:31:24  jussi
  Added VISUAL_RECORD visual argument.

  Revision 1.2  1995/09/05 21:13:23  jussi
  Added/updated CVS header.
*/

#ifndef VisualArg_h
#define VisualArg_h

#include <stdio.h>

#include "DeviseTypes.h"

#if 0 // Not currently used.  RKW Feb. 25, 1998.
#include "Pattern.h"
#include "Color.h"
#endif

#include "ViewDir.h"

/* Index of the attributes */

const unsigned VISUAL_X_INDEX = 0;
const unsigned VISUAL_Y_INDEX = 1;
const unsigned VISUAL_SIZE_INDEX = 2;
const unsigned VISUAL_PATTERN_INDEX = 3;
const unsigned VISUAL_COLOR_INDEX = 4;
const unsigned VISUAL_ORIENTATION_INDEX = 5;
const unsigned VISUAL_SHAPE_INDEX = 6;
const unsigned VISUAL_RECORD_INDEX = 7;
const unsigned VISUAL_CAMERA_INDEX = 8;
const unsigned VISUAL_ANTICAMERA_INDEX = 9;
const unsigned VISUAL_TATTR_INDEX = 10;

/*
   A VisualFlag is the union of visual attributes.
   It indicates which attributes are changeable or tested in a filter.
*/

typedef unsigned VisualFlag;

const unsigned VISUAL_X           = (1 << VISUAL_X_INDEX);
const unsigned VISUAL_Y           = (1 << VISUAL_Y_INDEX);
const unsigned VISUAL_LOC         = ((1 << VISUAL_X_INDEX)
				     | (1 << VISUAL_Y_INDEX));
const unsigned VISUAL_COLOR       = (1 << VISUAL_COLOR_INDEX);
const unsigned VISUAL_SIZE        = (1 << VISUAL_SIZE_INDEX);
const unsigned VISUAL_PATTERN     = (1 << VISUAL_PATTERN_INDEX);
const unsigned VISUAL_ORIENTATION = (1 << VISUAL_ORIENTATION_INDEX);
const unsigned VISUAL_SHAPE       = (1 << VISUAL_SHAPE_INDEX);

const unsigned VISUAL_RECORD      = (1 << VISUAL_RECORD_INDEX);
const unsigned VISUAL_TATTR       = (1 << VISUAL_TATTR_INDEX);

// Note that this doesn't include RECORD or TATTR bits.
const unsigned VISUAL_ALLBITS     = (VISUAL_X | VISUAL_Y | VISUAL_LOC |
				     VISUAL_COLOR | VISUAL_SIZE |
				     VISUAL_PATTERN | VISUAL_ORIENTATION
				     | VISUAL_SHAPE );

/* Complement visual flag */

inline unsigned VisualComplement(VisualFlag flag)
{
  return (flag ^ VISUAL_ALLBITS);
}

/* A CameraFlag indicates whether the attributes are changeable or tested */
typedef unsigned CameraFlag;

/* A camera; used to store view point, perspective, etc */
struct Camera {
        Camera():view_dir(NegZ),min_x(-2.5),max_x(2.5),min_y(-2.5),max_y(2.5),
	         near(-2.5), far(2.5){}

        // The following fields are currently used

	ViewDir view_dir;

        // View port info
        Coord min_x, max_x, min_y, max_y, near, far;

        // how much to pan after aligning the viewing direction to one axis 
	Coord pan_right, pan_up;

	friend unsigned char operator == (Camera const &a, Camera const &b)
	{ return
                  (a.view_dir==b.view_dir)
                &&(a.min_x==b.min_x)
                &&(a.max_x==b.max_x)
                &&(a.min_y==b.min_y)
                &&(a.max_y==b.max_y)
                &&(a.near==b.near)
                &&(a.far==b.far)
		&&(a.pan_right==b.pan_right)
		&&(a.pan_up==b.pan_up);
        }
};

/* A visual filter: used to filter symbols inside a view. */

struct VisualFilter {
  VisualFlag flag;	           /* which attribute is to test.
				      set to 0 if no filter  */
  Coord xLow, xHigh;               /* X filter */
  Coord yLow, yHigh;               /* y filter */
#if 0 // Not currently used.  RKW Feb. 25, 1998.
  int lastN;		           /* # of records to examine */
  Coord sizeLow, sizeHigh;         /* size filter */
  Pattern patternLow, patternHigh; /* pattern filter */
  PColorID colorLow, colorHigh;	// Color filter
  Coord orientationLow, orientationHigh; /* orientation filter*/
  int shapeLow, shapeHigh;         /* shape filter */
#endif
  Camera camera;
  
  Boolean marked;                  /* TRUE if this is marked in the
				      control panel list box */
  friend unsigned char operator ==
    (VisualFilter const &a, VisualFilter const &b) {
    return a.flag==b.flag && a.xLow==b.xLow && a.xHigh==b.xHigh
           && a.yLow==b.yLow && a.yHigh==b.yHigh && a.camera == b.camera;
  }
};

//******************************************************************************
#endif

