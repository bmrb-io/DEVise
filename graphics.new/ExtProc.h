/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ExtProc class (for running an external process to
  dynamically generate data).

  Note: arguments in mapping are as follows:
    ShapeAttr5: program name
    ShapeAttr6: output file
    ShapeAttr7: argument 1 (optional)
      ...
    ShapeAttr14: argument 8 (optional)
 */

/*
  $Id$

  $Log$
  Revision 1.1  2001/04/12 20:16:30  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

 */

#ifndef _ExtProc_h_
#define _ExtProc_h_


#include <sys/types.h>

#include "DevStatus.h"

class TDataMap;

class ExtProc {
public:
  static ExtProc *GetInstance();

  DevStatus Run(TDataMap *map, const char *gdataRecP);

private:
  ExtProc();
};

#endif // _ExtProc_h_

/*============================================================================*/
