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

class ViewGraph;
class TDataMap;
class AttrList;
class RecInterp;

class DrillDown3D {
public:
    DrillDown3D();
    ~DrillDown3D();

    // Coords is the set of GData x, y, z triplets for which we need
    // TData info.
    DevStatus RunQuery(ViewGraph *view, int count, Point3D coords[]);

    void GetResults(int &count, const char *const *& messages);

private:
    void GetMapping(ViewGraph *view);
    void SetVisualFilter();
    void ExecuteQuery();
    void ProcessRecord(char *tData, char *gData);
    void PrintTData(Coord gdX, Coord gdY, Coord gdZ, char *tData);

    static const int GDATA_BUF_SIZE = 4 * 1024 * sizeof(double);

    DevStatus _status;

    int _count; // number of input points
    Point3D *_coords; // coordinates of input points

    int _recordCount; // number of records found

    TDataMap *_tdMap;
    VisualFilter _filter;

    char* _gdataBuf;

    AttrList* _attrs;
    RecInterp* _recInterp;

    ArgList _results;
};

#endif // _DrillDown3D_h_

/*============================================================================*/
