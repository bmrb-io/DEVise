/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2003-2006
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DrillDown class (code implementing drill-down to find
  data at mouse location for 2D views).
 */

/*
  $Id$

  $Log$
  Revision 1.3  2006/05/25 20:15:27  wenger
  Merged V1_8b0_br_0 thru V1_8b0_br_1 to the trunk.

  Revision 1.2.2.2  2006/12/07 22:09:14  wenger
  Fixed bug 929 (drill-down on view with empty data source causes crash)
  and associated bugs in the JavaScreen client.

  Revision 1.2.2.1  2005/12/27 22:51:51  wenger
  Somewhat kludgily fixed DEVise bug 917/919 (drill-down works
  poorly on fixed text symbols) -- expanded visual filter for
  query because we don't know the bounding box of the symbols.

  Revision 1.2  2005/12/06 20:03:55  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.1.2.2  2003/11/21 23:05:11  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.1.2.1  2003/11/21 18:45:08  wenger
  Totally restructured 2D drill-down code into new DrillDown class;
  increased the tolerance for not being *exactly* on a symbol with
  the mouse.

 */

#ifndef _DrillDown_
#define _DrillDown_

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

class DrillDown
{
public:
    static DrillDown *GetInstance();

    /**
     * Get the data for drill-down at the given location.
     * @param The view in which the drill-down is occurring.
     * @param The X coordinate of the drill-down.
     * @param The Y coordinate of the drill-down.
     * @param The distance in the X direction corresponding to one pixel.
     * @param The distance in the Y direction corresponding to one pixel.
     * @param Reference to an int to hold the number of messages generated.
     * @param Reference to a char ** that will be set to an array of
     *        the messages.  Note: this really should be const char **
     *        or const char * const, but I'm leaving it just char **
     *        for compatibility with existing functions.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus GetData(ViewData *view, Coord drillX, Coord drillY,
      Coord pixelX, Coord pixelY, int &numMsgs, char **&msgs);

    /**
     * Find out whether the given view has a TData with no valid records.
     * @param The view
     * @return True iff the view has no valid records.
     */
    static Boolean EmptyTData(ViewGraph *view);

private:
    /**
     * Constructor.
     */
    DrillDown();

    /**
     * Destructor.
     */
    ~DrillDown();

    /**
     * Do the processing for a view, including doing drill-down on all
     * views in a pile.
     * @param The view in which the drill-down is occurring.
     * @param The X coordinate of the drill-down.
     * @param The Y coordinate of the drill-down.
     * @param The distance in the X direction corresponding to one pixel.
     * @param The distance in the Y direction corresponding to one pixel.
     * @param The number of pixel's worth of tolerance to apply.
     * @param A reference to a boolean that will tell whether we got
     *        any data.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus ProcessView(ViewData *view, Coord drillX, Coord drillY,
      Coord pixelX, Coord pixelY, int pixelTol, Boolean &gotData);

    /**
     * Run a drill-down query on a single view and save or print the results.
     * @param The view in which the drill-down is occurring.
     * @param The X coordinate of the drill-down.
     * @param The Y coordinate of the drill-down.
     * @param The distance in the X direction corresponding to one pixel.
     * @param The distance in the Y direction corresponding to one pixel.
     * @param The number of pixel's worth of tolerance to apply.
     * @param A reference to a boolean that will tell whether we got
     *        any data.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus RunQuery(ViewData *view, Coord drillX, Coord drillY,
      Coord pixelX, Coord pixelY, int pixelTol, Boolean &gotData);

    /**
     * Process (save or print) the data returned from a query.
     * @param The view in which the drill-down is occurring.
     * @param The query processor.
     * @param The TData.
     * @param The TDataMap (mapping).
     * @param A reference to a boolean that will tell whether we got
     *        any data.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus ProcessData(ViewData *view, QueryProc *qp, TData *tdata,
      TDataMap *map, Boolean &gotData);

    /**
     * Test all records against any GAttr links of which the drill-down
     * view is a follower.
     * @param The view in which the drill-down is occurring.
     * @param The TDataMap (mapping).
     * @param The first record ID of this batch of TData.
     * @param The buffer containing the TData records.
     * @param The number of TData records.
     * @param The symbol info array (holds a flag for whether a given
     *        record passes the GAttr links).
     */
    static void GAttrLinkFollower(ViewData *view, TDataMap *map,
      RecId startRid, char *tdBuf, int numRecs,
      ViewData::SymbolInfo symInfo[]);

    /**
     * Show the data of the record currently being processed.
     * @return DevStatus value reporting the result of the operation.
     */
    DevStatus ShowRecord(AttrList *attrs);

    /**
     * Initialize the message buffers.
     * @param The X coordinate of the drill-down.
     * @param The attribute type of the X attribute (used for choosing
     *        float or date format).
     * @param The Y coordinate of the drill-down.
     * @param The attribute type of the Y attribute (used for choosing
     *        float or date format).
     * 
     */
    Boolean InitPutMessage(ViewData *view, Coord drillX, AttrType xAttr,
      Coord drillY, AttrType yAttr);

    /**
     * Process (save or print) a message).
     * @param The message to process.
     */
    void ProcessMessage(const char *msg);

    /**
     * Put the given message into the message buffers (if there's room).
     * @param The message to process.
     */
    Boolean PutMessage(const char *msg);

    /**
     * Determine whether there's room for another message.
     * @param The length of the new message (including terminating null).
     * @return True iff there's room for another message.
     */
    Boolean RoomForNewMessage(int msgLen);

    /**
     * Save the warning that there was too much data to show.
     */
    void PutTooMuchMsg();

    /**
     * Finalize the message buffers and set the arguments accordingly.
     * @param Reference to an int to hold the number of messages generated.
     * @param Reference to a char ** that will be set to an array of
     *        the messages.
     */
    void EndPutMessage(int &numMessages, char **&msgs);

    /**
     * Print out the existing messages.
     */
    void PrintMsgBuf();


    static DrillDown *_instance;

    static const int MAX_MSGS = 128;
    static const int NON_JS_MAX_MSGS = 48;
    static const int MSG_BUF_SIZE = 4096;

    int _maxMsgs;
    char _msgBuf[MSG_BUF_SIZE];
    int _msgBufIndex;
    // Note: this really should be const char * -- see note about the
    // GetData() method.
    char *_msgArray[MAX_MSGS];
    int _numMsgs;
    Boolean _spaceLeft;

    static const int PIXEL_TOL_MAX = 5;
    static const int PIXEL_TOL_INC = 5;

    static const int FIXED_TEXT_EXTRA_TOL = 5;
    static const int FIXED_TEXT_X_FACTOR = 2;

    RecInterp *_recInterp;
};

#endif // _DrillDown_

/*============================================================================*/
