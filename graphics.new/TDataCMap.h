/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.9  1999/11/30 22:28:30  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.8  1998/08/17 18:51:54  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.7  1997/11/24 23:15:19  weaver
  Changes for the new ColorManager.

  Revision 1.6.10.1  1997/05/21 20:40:48  weaver
  Changes for new ColorManager

  Revision 1.6  1996/11/13 16:57:09  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.5  1996/01/30 21:15:51  jussi
  Removed references to specific colors.

  Revision 1.4  1995/12/29 22:40:13  jussi
  Cleaned up the code a bit.

  Revision 1.3  1995/12/28 19:52:11  jussi
  Small fixes to remove compiler warnings. Added copyright notice.

  Revision 1.2  1995/09/05 22:15:53  jussi
  Added CVS header.
*/

#ifndef TDataCMap_h
#define TDataCMap_h

#include "Pattern.h"
#include "ConnectorShape.h"
#include "Connector.h"
#include "GDataBin.h"
#include "Exit.h"
#include "DevError.h"

#include "Color.h"

class Stream;
class Connector;

//******************************************************************************
// class TDataCMap
//******************************************************************************

class TDataCMap
{
	private:

		Coloring	coloring;		// Default data coloring

	public:

		// Constructors and Destructors
		TDataCMap(void){
			defaultPattern = Pattern0;
			_shapeId = ContLineConnectorID;
			_numShapeAttrs = 0;
		}
		virtual ~TDataCMap() {}

		// Getters and Setters
		Coloring&			GetColoring(void)			{ return coloring;	}
		const Coloring&		GetColoring(void) const 	{ return coloring;	}
		
  /* called by GDataBin to map 2 gdata records to one connection */
  virtual Boolean MapToConnection(void *grec1, void *grec2, Connector *c) {
    GDataBinRec *s1 = (GDataBinRec *)grec1;
    GDataBinRec *s2 = (GDataBinRec *)grec2;
    c->x1 = s1->x;
    c->y1 = s1->y;
    c->x2 = s2->x;
    c->y2 = s2->y;
    c->pattern = defaultPattern;
    c->color = GetColoring().GetForeground();
    c->cShapeID = _shapeId;
    c->numShapeAttrs = _numShapeAttrs;
    for(int i = 0; i < _numShapeAttrs; i++)
      c->shapeAttrs[i] = _shapeAttrs[i];
    return Map(grec1, grec2, c);
  }

  void SetDefaultPattern(Pattern p) { defaultPattern = p; }
  void SetDefaultConnectorShape(ConnectorShapeID shapeId,
				int numAttrs = 0,
				ShapeAttr *shapeAttr = NULL) {
    if (numAttrs > (int)MAX_CONNECTOR_SHAPE_ATTRS) {
      fprintf(stderr, "ConnectorMapping::SetDefaultShape:too many attrs %d\n",
	      numAttrs);
      reportErrNosys("Fatal error");//TEMP -- replace with better message
      Exit::DoExit(1);
    }
    _shapeId = shapeId;
    _numShapeAttrs = numAttrs;
    _shapeAttrs = shapeAttr;
  }
  Pattern GetDefaultPattern() { return defaultPattern; }

protected:
  virtual Boolean Map(void *, void *, Connector *) { return true; }

private:
  Pattern          defaultPattern;
  ConnectorShapeID _shapeId;
  int              _numShapeAttrs;
  ShapeAttr        *_shapeAttrs;
};

//******************************************************************************
#endif
