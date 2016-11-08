/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2003
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of HomeCalc class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.3  2003/08/01 18:31:10  wenger
  Fixed bug 886 (home problem); found bug 887 (another home problem).

  Revision 1.1.2.2  2003/06/16 16:42:06  wenger
  Fixed bug 879 (a problem with the home code) -- made pretty significant
  improvements to how we deal with the data ranges.

  Revision 1.1.2.1  2003/06/06 17:10:15  wenger
  Refactored the View::GetHome2D code so that the actual calculation is
  done in the new HomeCalc class, and there's only one chunk of code
  instead of four almost-identical chunks (xlo, xhi, ylo, yhi).

 */

#include <stdio.h>

#include "HomeCalc.h"
#include "AttrList.h"
#include "TDataMap.h"
#include "TData.h"

#define DEBUG 0

//-----------------------------------------------------------------------------
HomeCalc::HomeCalc(const char *name, TDataMap *tdMap, Coord autoMargin,
  const AttrInfo *attr, Coord &filterValue) :
    _name(name),
    _autoMargin(autoMargin),
    _attr(attr),
    _filterValue(filterValue)
{
#if (DEBUG >= 1)
    printf("HomeCalc(%p)::HomeCalc(%s, %g)\n", this, _name, _filterValue);
#endif

    if (_attr && !strcmp(_attr->name, REC_ID_NAME)) {
        TData *tdata = tdMap->GetPhysTData();
	RecId tmpFirst, tmpLast;
        _hasFirstRec = tdata->HeadID(tmpFirst);
        _hasLastRec = tdata->LastID(tmpLast);
	_firstRec = (Coord)tmpFirst;
	_lastRec = (Coord)tmpLast;
    } else {
        _hasFirstRec = false;
        _hasLastRec = false;
	_firstRec = 0.0;
	_lastRec = 0.0;
    }
#if (DEBUG >= 2)
    printf("  Home: hasFirstRec: %d; firstRec: %g\n", _hasFirstRec,
      _firstRec);
    printf("  Home: hasLastRec: %d; lastRec: %g\n", _hasLastRec,
      _lastRec);
#endif
}

//-----------------------------------------------------------------------------
HomeCalc::~HomeCalc()
{
#if (DEBUG >= 1)
    printf("HomeCalc(%p, %s)::~HomeCalc()\n", this, _name);
#endif

    _name = NULL;
}

//-----------------------------------------------------------------------------
void
HomeCalc::Calculate(Boolean dataRangesValid, Coord dataValue)
{
#if (DEBUG >= 1)
    printf("HomeCalc(%s)::Calculate(%d, %g)\n", _name, dataRangesValid,
      dataValue);
#endif

    //
    // There are three cases here as far as the data ranges go:
    // 1. the ViewData class has processed all TData records;
    // 2. the ViewData class has processed some, but not all TData records
    // (e.g., X is recId);
    // 3. the ViewData class has not processed any TData records.
    // In case 1 (dataRangesValid is true), we want to just use the data
    // range info for calculating the home filter.  In case 2
    // (dataRangesValid is false), we want to use the data range info,
    // but possible override it with attribute info.  In case 3
    // (dataRangesValid is also false) we don't want to use the data range
    // info at all.  This is accomplished by having the data ranges set to
    // the invalid values, which don't change the filter.
    //

    Boolean isSet = false;

    // Check data ranges first (fixes bug 469).
    if (dataRangesValid) {
        if (!isSet) {
	    UpdateFilterValue(dataValue, true);
	    isSet = true;
#if (DEBUG >= 2)
            printf("  Home: set %s from data range: %g\n", _name,
	      _filterValue);
#endif
	}
    } else {
        // This is kind of a kludgey partial fix for bug 485.
	// RKW 2002-04-30.
	if (!isSet) {
	    UpdateFilterValue(dataValue, false);
#if (DEBUG >= 2)
            printf("  Home: tentatively set %s from data range: %g\n", _name,
	      _filterValue);
#endif
	}
    }

    if (_attr) {
        if (!isSet) {
            if (HasAttrValue()) {
	        UpdateFilterValue(GetAttrValue(), false);
                isSet = true;
#if (DEBUG >= 2)
                printf("  Home: set %s from attr info: %g\n", _name, _filterValue);
#endif
            } else if (!strcmp(_attr->name, REC_ID_NAME)) {
                if (HasRecIdValue()) {
	            UpdateFilterValue(GetRecIdValue(), false);
	            isSet = true;
#if (DEBUG >= 2)
                    printf("  Home: set %s from recId info: %g\n", _name,
	              _filterValue);
#endif
	        }
            }
	}
    }

#if (DEBUG >= 1)
    printf("  HomeCalc(%s)::Calculate() returning %g\n", _name, _filterValue);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
HomeCalcLow::HomeCalcLow(const char *name, TDataMap *tdMap, Coord autoMargin,
  const AttrInfo *attr, Coord &filterValue) :
    HomeCalc(name, tdMap, autoMargin, attr, filterValue)
{
#if (DEBUG >= 1)
    printf("HomeCalcLow(%p)::HomeCalcLow(%s, %g)\n", this, _name,
      _filterValue);
#endif
}

//-----------------------------------------------------------------------------
HomeCalcLow::~HomeCalcLow()
{
#if (DEBUG >= 1)
    printf("HomeCalcLow(%p, %s)::~HomeCalcLow()\n", this, _name);
#endif
}

//-----------------------------------------------------------------------------
void
HomeCalcLow::UpdateFilterValue(Coord newValue, Boolean force)
{
#if (DEBUG >= 2)
    printf("  HomeCalcLow(%s)::UpdateFilterValue(%g, %d)\n", _name, newValue,
      force);
#endif

    if (force) {
        _filterValue = newValue - _autoMargin;
    } else {
        _filterValue = MIN(_filterValue, newValue - _autoMargin);
    }
}

//-----------------------------------------------------------------------------
Boolean
HomeCalcLow::HasAttrValue()
{
    return _attr->hasLoVal;
}

//-----------------------------------------------------------------------------
Coord
HomeCalcLow::GetAttrValue()
{
    return AttrList::GetVal(&_attr->loVal, _attr->type);
}

//-----------------------------------------------------------------------------
Boolean
HomeCalcLow::HasRecIdValue()
{
    return _hasFirstRec;
}

//-----------------------------------------------------------------------------
Coord
HomeCalcLow::GetRecIdValue()
{
    return _firstRec;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------------------------------
HomeCalcHigh::HomeCalcHigh(const char *name, TDataMap *tdMap, Coord autoMargin,
  const AttrInfo *attr, Coord &filterValue) :
    HomeCalc(name, tdMap, autoMargin, attr, filterValue)
{
#if (DEBUG >= 1)
    printf("HomeCalcHigh(%p)::HomeCalcHigh(%s, %g)\n", this, _name,
      _filterValue);
#endif
}

//-----------------------------------------------------------------------------
HomeCalcHigh::~HomeCalcHigh()
{
#if (DEBUG >= 1)
    printf("HomeCalcHigh(%p, %s)::~HomeCalcHigh()\n", this, _name);
#endif
}

//-----------------------------------------------------------------------------
void
HomeCalcHigh::UpdateFilterValue(Coord newValue, Boolean force)
{
#if (DEBUG >= 2)
    printf("HomeCalcHigh(%s)::UpdateFilterValue(%g, %d)\n", _name, newValue,
      force);
#endif

    if (force) {
        _filterValue = newValue + _autoMargin;
    } else {
        _filterValue = MAX(_filterValue, newValue + _autoMargin);
    }
}

//-----------------------------------------------------------------------------
Boolean
HomeCalcHigh::HasAttrValue()
{
    return _attr->hasHiVal;
}

//-----------------------------------------------------------------------------
Coord
HomeCalcHigh::GetAttrValue()
{
    return AttrList::GetVal(&_attr->hiVal, _attr->type);
}

//-----------------------------------------------------------------------------
Boolean
HomeCalcHigh::HasRecIdValue()
{
    return _hasLastRec;
}

//-----------------------------------------------------------------------------
Coord
HomeCalcHigh::GetRecIdValue()
{
    return _lastRec;
}

/*============================================================================*/
