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
  Declaration of CountMapping class.  This class is used to provide mappings
  in which the value of one GData attribute is given by the number of symbols
  that have landed in the current symbol's "bin" in another GData attribute.

  In the first implementation, we are only allowing the counting to be done
  on pixels, with single-pixel bins.  We are also temporarily bypassing
  the mapping and associating this with the views, to keep things simpler.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/02/26 22:59:46  wenger
  Added "count mappings" to views, except for API and GUI (waiting for
  Dongbin to finish his mods to ParseAPI); conditionaled out unused parts
  of VisualFilter struct; did some cleanup of MappingInterp class.

 */

#ifndef _CountMapping_h_
#define _CountMapping_h_

#include <sys/types.h>

#include "DevStatus.h"

class ViewGraph;
class Transform2D;
class VisualFilter;

class CountMapping {
public:
  enum Attr {
    AttrInvalid = 0,
    AttrX,
    AttrY
  };

  // Constructor and destructor.
  CountMapping(Attr countAttr, Attr putAttr);
  ~CountMapping();

  void GetAttrs(Attr &countAttr, Attr &putAttr);

  // Initializer -- must be called each time the view is drawn, before
  // any GData records are processed.
  DevStatus Init(ViewGraph *view);

  // Process each GData record, changing values if necessary.
  DevStatus ProcessRecord(void *gdataRec);

private:
  Attr _countAttr; // attribute to count
  Attr _putAttr; // attribute to put count into

  int _xOffset; // offset into GData record of X value
  int _yOffset; // offset into GData record of Y value
  int _putOffset; // offset into GData record of _putAttr

  Transform2D *_transform;
  VisualFilter *_filter;

  int _binCount;;
  int *_bins;

  Boolean _valid;
};

#endif /* _CountMapping_h_ */

/*============================================================================*/
