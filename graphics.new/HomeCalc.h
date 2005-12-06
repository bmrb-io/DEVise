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
  Declaration of the HomeCalc class (does calculations for home on
  views).  This class is basically code that was extracted from
  ViewGraph::GetHome2D().  The purpose of this class is to eliminate
  a four-way almost-exact duplication of code (for xlow, xhigh, ylow,
  and yhigh) in ViewGraph::GetHome2D().
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  2003/06/06 17:10:15  wenger
  Refactored the View::GetHome2D code so that the actual calculation is
  done in the new HomeCalc class, and there's only one chunk of code
  instead of four almost-identical chunks (xlo, xhi, ylo, yhi).

 */

#ifndef _HomeCalc_h_
#define _HomeCalc_h_

#include "DeviseTypes.h"

class AttrInfo;
class TDataMap;

class HomeCalc {
public:
    // Constructor/destructor.
    HomeCalc(const char *name, TDataMap *tdMap, Coord autoMargin,
      const AttrInfo *attr, Coord &filterValue);
    virtual ~HomeCalc();

    void Calculate(Boolean dataRangesValid, Coord dataValue);

protected:
    virtual void UpdateFilterValue(Coord newValue, Boolean force) = 0;
    virtual Boolean HasAttrValue() = 0;
    virtual Coord GetAttrValue() = 0;
    virtual Boolean HasRecIdValue() = 0;
    virtual Coord GetRecIdValue() = 0;


    //TEMPTEMP -- do we really want these to be const *? (see C++ book)
    const char *_name;
    Coord _autoMargin;
    const AttrInfo *_attr;

    Boolean _hasFirstRec;
    Coord _firstRec;
    Boolean _hasLastRec;
    Coord _lastRec;

    Coord &_filterValue;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class HomeCalcLow : public HomeCalc {
public:
    // Constructor/destructor.
    HomeCalcLow(const char *name, TDataMap *tdMap, Coord autoMargin,
      const AttrInfo *attr, Coord &filterValue);
    virtual ~HomeCalcLow();

protected:
    virtual void UpdateFilterValue(Coord newValue, Boolean force);
    virtual Boolean HasAttrValue();
    virtual Coord GetAttrValue();
    virtual Boolean HasRecIdValue();
    virtual Coord GetRecIdValue();
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class HomeCalcHigh : public HomeCalc {
public:
    // Constructor/destructor.
    HomeCalcHigh(const char *name, TDataMap *tdMap, Coord autoMargin,
      const AttrInfo *attr, Coord &filterValue);
    virtual ~HomeCalcHigh();

protected:
    virtual void UpdateFilterValue(Coord newValue, Boolean force);
    virtual Boolean HasAttrValue();
    virtual Coord GetAttrValue();
    virtual Boolean HasRecIdValue();
    virtual Coord GetRecIdValue();
};

#endif // _HomeCalc_h_

/*============================================================================*/
