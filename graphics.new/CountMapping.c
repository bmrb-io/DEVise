/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of CountMapping class.

  Possible improvements:
  - Take symbol size into account when "binning".
  - Allow size of bins to be defined.
  - Allow bins to be in terms of data units rather than pixel units.
  - Deal better with symbols that are partly within the visual filter,
    but whose centers are not within the visual filter.
  - Allow dealing with GData attributes other than X and Y.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/10/20 19:46:13  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.2  1998/08/17 18:51:49  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.1  1998/02/26 22:59:46  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

 */

#include <stdio.h>

#include "CountMapping.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "Transform.h"
#include "DevError.h"
#include "VisualArg.h"

//#define DEBUG

/*----------------------------------------------------------------------------*/
CountMapping::CountMapping(CountMapping::Attr countAttr,
    CountMapping::Attr putAttr)
{
#if defined(DEBUG)
  printf("CountMapping(0x%p)::CountMapping(%d, %d)\n", this,
      (int) countAttr, (int) putAttr);
#endif

  _valid = false;
  _countAttr = countAttr;
  _putAttr = putAttr;
  _binCount = 0;
  _bins = NULL;

  if (countAttr == putAttr) {
    reportErrNosys("Attribute to count should be different than attribute "
	"to put count into");
  } else {
    _valid = true;
  }
}

/*----------------------------------------------------------------------------*/
CountMapping::~CountMapping()
{
#if defined(DEBUG)
  printf("CountMapping(0x%p)::~CountMapping()\n", this);
#endif

  _valid = false;
  delete [] _bins;
}

/*----------------------------------------------------------------------------*/
void
CountMapping::GetAttrs(Attr &countAttr, Attr &putAttr)
{
#if defined(DEBUG)
  printf("CountMapping(0x%p)::GetAttrs()\n", this);
#endif

  countAttr = _countAttr;
  putAttr = _putAttr;
}

/*----------------------------------------------------------------------------*/
DevStatus
CountMapping::Init(ViewGraph *view)
{
#if defined(DEBUG)
  printf("CountMapping(0x%p)::Init()\n", this);
#endif

  DevStatus result = StatusOk;

  if (!_valid) {
    reportErrNosys("Invalid CountMapping object");
    result += StatusFailed;
  }

  // Make sure we're doing this on a two-dimensional view.
  if (result.IsComplete() && view->GetNumDimensions() != 2) {
    reportErrNosys("Cannot do count mapping on a non-two-dimensional view");
    result += StatusFailed;
  }

  // Find the offsets for the GData attributes we'll have to access.
  if (result.IsComplete()) {
    TDataMap *gdataMap = view->GetFirstMap();
    GDataAttrOffset *offsets = gdataMap->GetGDataOffset();

    _xOffset = offsets->_xOffset;
    _yOffset = offsets->_yOffset;

    switch (_putAttr) {
    case AttrX:
      _putOffset = offsets->_xOffset;
      break;

    case AttrY:
      _putOffset = offsets->_yOffset;
      break;

    default:
      reportErrNosys("Illegal GData attribute");
      result += StatusFailed;
      break;
    }
    if (_putOffset < 0) {
      reportErrNosys(
	  "Attribute to put count into is a constant (not in GData record)");
      result += StatusFailed;
    }
  }
  
  // Get the view geometry and figure out from that how many bins we'll need.
  // Note: this is giving us the size of the whole view, not just the data
  // area, but it doesn't seem worth any extra complication to save a few
  // bins.
  if (result.IsComplete()) {
    int xLoc, yLoc;
    unsigned width, height;
    view->Geometry(xLoc, yLoc, width, height);

    int newBinCount;

    switch (_countAttr) {
    case AttrX:
      newBinCount = width;
      break;

    case AttrY:
      newBinCount = height;
      break;

    default:
      reportErrNosys("Illegal GData attribute");
      break;
    }

    // Kludge to make sure we have enough bins -- we sometimes don't otherwise.
    newBinCount++;

    if (newBinCount > _binCount) {
      delete [] _bins;
      _binCount = newBinCount;
      _bins = new int[_binCount];
    }

    int index;
    for (index = 0; index < _binCount; index++) {
      _bins[index] = 0;
    }
  }

  // Get the transform from the View's WindowRep.
  if (result.IsComplete()) {
    WindowRep *windowRep = view->GetWindowRep();
    _transform = windowRep->TopTransform();
  }

  // Get the view's visual filter.
  if (result.IsComplete()) {
    _filter = view->GetVisualFilter();
  }

  return result;
}

/*----------------------------------------------------------------------------*/
DevStatus
CountMapping::ProcessRecord(void *gdataRec)
{
#if defined(DEBUG)
  printf("CountMapping(0x%p)::ProcessRecord()\n", this);
#endif

  DevStatus result = StatusOk;

  if (!_valid) {
    reportErrNosys("Invalid CountMapping object");
    result += StatusFailed;
  } else {
    // Note: we are assuming that the GData is aligned on double (8-bit)
    // boundaries.
    double *xP = (double *)((char *)gdataRec + _xOffset);
    double *yP = (double *)((char *)gdataRec + _yOffset);
    double *putP = (double *)((char *)gdataRec + _putOffset);

    Coord oldX = (Coord)(*xP);
    Coord oldY = (Coord)(*yP);
#if defined(DEBUG)
    printf("  oldX = %g, oldY = %g\n", oldX, oldY);
#endif

    // Find out if this record is within the visual filter (force the attribute
    // that's getting the count to the center of the visual filter).
    Boolean inFilter;
    switch (_putAttr) {
    case AttrX: {
      Coord tmpX = (_filter->xLow + _filter->xHigh) / 2.0;
      inFilter = View::InVisualFilter(*_filter, tmpX, oldY);
      break;
    }

    case AttrY: {
      Coord tmpY = (_filter->yLow + _filter->yHigh) / 2.0;
      inFilter = View::InVisualFilter(*_filter, oldX, tmpY);
      break;
    }

    default:
      inFilter = false;
      reportErrNosys("Illegal GData attribute");
      result += StatusFailed;
      break;
    }

    if (inFilter) {
      // Do the WindowRep's transform to get the counted attribute in terms
      // of pixels.
      Coord newVal;
      switch (_countAttr) {
      case AttrX:
        _transform->TransformX(oldX, oldY, newVal);
        break;

      case AttrY:
        _transform->TransformY(oldX, oldY, newVal);
        break;

      default:
        reportErrNosys("Illegal GData attribute");
        result += StatusFailed;
        break;
      }

      // Put the count into the appropriate attribute and increment the bin
      // count.
      if (result.IsComplete()) {
        int index = (int) newVal;
        if (index < 0 || index >= _binCount) {
	  reportErrNosys("Internal error in CountMapping: index outside bins");
        } else {
          *putP = (double) _bins[index];
#if defined(DEBUG)
          printf("  newX = %g, newY = %g\n", *xP, *yP);
#endif
          _bins[index]++;
        }
      }
    }
  }

  return result;
}

/*============================================================================*/
