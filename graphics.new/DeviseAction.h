/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */

#ifndef _DEVISEACTION_H_
#define _DEVISEACTION_H_


class DeviseAction
{
  public:

    enum Action {
	NO_ACTION = 0,		// must be first entry

	ZOOM_IN_X,
	ZOOM_IN_Y,
	ZOOM_IN_Z,

	ZOOM_OUT_X,
	ZOOM_OUT_Y,
	ZOOM_OUT_Z,

	ZOOM_MAX_X,
	ZOOM_MAX_Y,
	ZOOM_MAX_Z,
	ZOOM_MAX_XYZ,

	ZOOM_AREA_X,		// used with mouse drag
	ZOOM_AREA_Y,
	ZOOM_AREA_XY,

	PAN_LEFT,
	PAN_RIGHT,
	PAN_UP,
	PAN_DOWN,
	PAN_IN,
	PAN_OUT,
	
	TOGGLE_SOLID_3D,
	TOGGLE_ZOOM_3D,

	TOGGLE_VALUES,
	TOGGLE_CONNECTORS,
	TOGGLE_SYMBOLS,

	SET_HISTOGRAM_WIDTH_TO_FILTER_RANGE,
	SET_HISTOGRAM_WIDTH_TO_FILE_RANGE,
	SET_HISTOGRAM_WIDTH_TO_VIEW_RANGE,
	
	TOGGLE_FOCUS_POINT,
	TOGGLE_COORDINATES,	// Rectangular/Radial
	TOGGLE_PERSPECTIVE,

	INCREASE_PIXEL_SIZE,
	DECREASE_PIXEL_SIZE,
	
	RECORD_QUERY,
    };
};


#endif // _DEVISEACTION_H_
