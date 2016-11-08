/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2012
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DataDowload class (code to save all data related
  to a given view).
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.4  2012/04/30 20:40:04  wenger
  (Hopefully final) cleanup.

  Revision 1.1.2.3  2012/04/26 22:57:39  wenger
  Created the DrillDown::AdjustFilterForCountMapping() method, and
  DataDownload uses DrillDown::GAttrLinkFollower() to remove
  duplicate code; removed some debug/test code.

  Revision 1.1.2.2  2012/04/24 20:46:52  wenger
  Actual data saving now working except for some things like escaping
  commas; the code still needs some cleanup; JS GUI and help still need work.

  Revision 1.1.2.1  2012/04/24 20:09:59  wenger
  Saving of actual data values now mostly working (print format
  needs some fixes).

 */

#ifndef _DataDownload
#define _DataDownload

#include <sys/types.h>

#include "DeviseTypes.h"
#include "AttrList.h"
#include "DevStatus.h"
#include "ViewData.h"

//=============================================================================

class QueryProc;
class TData;
class RecInterp;
class TDataMap;

class DataDownload
{
public:
    static DataDownload *GetInstance();

    /**
     * Save data from the given view.
     * @param The view for which to save data.
     * @param Whether to use the view's visual filter (as opposed to
     *   saving *all* data).
     * @param The FILE to which to write the data.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus SaveData(ViewData *view, Boolean useVisualFilter, FILE *fp);

private:
    /**
     * Constructor.
     */
    DataDownload();

    /**
     * Destructor.
     */
    ~DataDownload();

    /**
     * Clear out all variables that need to be reset between uses of
     * a DataDownload object.
     */
    void Clear();

    /**
     * Run a drill-down query on a single view and save or print the results.
     * @param The view in which the drill-down is occurring.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus RunQuery(ViewData *view, Boolean useVisualFilter);

    /**
     * Process (save or print) the data returned from a query.
     * @param The view in which the drill-down is occurring.
     * @param The query processor.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus ProcessData(ViewData *view, QueryProc *qp);

    /**
     * Show the data of the record currently being processed.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus ShowRecord();

    static DataDownload *_instance;

    FILE *_fp;
    TDataMap *_map;
    TData *_tdata;
    AttrList *_attrs;

    RecInterp *_recInterp;
};

#endif // _DataDownload

/*============================================================================*/
