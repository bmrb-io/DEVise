/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4.10.1  1997/05/21 20:40:24  weaver
  Changes for new ColorManager

  Revision 1.4  1996/11/13 16:57:02  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.3  1995/11/28 00:08:20  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:38  jussi
  Added CVS header.
*/

#ifndef Connector_h
#define Connector_h

#include "DeviseTypes.h"
#include "ShapeID.h"
#include "Pattern.h"
#include "ConnectorShape.h"

#include "Color.h"

const unsigned MAX_CONNECTOR_SHAPE_ATTRS = 10;

struct Connector
{
	public:
		Coord				x1,y1,x2,y2;	// End points of the connector
		PColorID			color;
		Pattern				pattern;
		ConnectorShapeID	cShapeID;		// ID of connector shape
		int					numShapeAttrs;	// # of attributes for shape
		ShapeAttr			shapeAttrs[MAX_CONNECTOR_SHAPE_ATTRS];
};

//******************************************************************************
#endif
