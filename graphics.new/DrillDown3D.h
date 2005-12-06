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
  Declaration of DrillDown3D class (DEVise end of drill-down for 3D views
  in the JavaScreen.
 */

/*
  $Id$

  $Log$
  Revision 1.3.10.1  2003/11/21 23:05:11  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.3  2001/05/24 18:42:02  wenger
  Fixed bug 674 (drill-down doesn't work correctly on record link follower
  views).

  Revision 1.2  2001/05/18 21:14:59  wenger
  Fixed bug 671 (potential GData buffer overflow).

  Revision 1.1  2001/05/18 19:25:36  wenger
  Implemented the DEVise end of 3D drill-down; changed DEVise version to
  1.7.3.

 */

#ifndef _DrillDown3D_h_
#define _DrillDown3D_h_

#include <sys/types.h>

#include "DevStatus.h"
#include "Geom.h"
#include "ArgList.h"
#include "VisualArg.h"

class ViewData;
class TDataMap;
class QueryCallback;
class AttrList;
class RecInterp;

class DrillDown3D {
public:
    DrillDown3D();
    ~DrillDown3D();

    // Coords is the set of GData x, y, z triplets for which we need
    // TData info.
    DevStatus RunQuery(ViewData *view, int count, Point3D coords[]);

    void GetResults(int &count, const char *const *& messages);

private:
    void GetMapping(ViewData *view);
    void SetVisualFilter();
    void ExecuteQuery(ViewData *view);
    void ProcessRecord(char *tData, char *gData);
    void PrintTData(Coord gdX, Coord gdY, Coord gdZ, char *tData);

    static const int GDATA_BUF_SIZE = 4 * 1024 * sizeof(double);

    DevStatus _status;

    int _count; // number of input points
    Point3D *_coords; // coordinates of input points

    int _recordCount; // number of records found

    TDataMap *_tdMap;
    QueryCallback *_callback;
    VisualFilter _filter;

    char* _gdataBuf;

    AttrList* _attrs;
    RecInterp* _recInterp;

    ArgList _results;
};

#endif // _DrillDown3D_h_

/*============================================================================*/
