#ifndef __PHONYRAM_H__
#define __PHONYRAM_H__

#include "RangeAccessMethod.h"

/* include Kevin's page/tuple stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

/* assuming the bounding box attribute is the first attr of the tuple */
/* attr start counting from 0 */
#define BBOX_ATTR_POSITION 0

/************************************************************************/
/* Phony RangeAccessMethod used for testing and demo purposes.		*/
/* Acts as the lower level data provider under RangeBufferManager.	*/
/*									*/
/* Structure: Just keep an array of pages of records.			*/
/*									*/	
/* Method used to answer query: Simple. Scan all records and filter.	*/
/************************************************************************/
class PhonyRAM: public RangeAccessMethod
{

public:

    /* initialize with the right object, DteTupleAdt and pages */
    PhonyRAM(RBMObject obj, const DteTupleAdt& rec, vector<char*>& inPages);
    virtual ~PhonyRAM();

    /********************************************************************/
    /* The open scan function. Specifies the query composed of the      */
    /* 'in' range and the 'not in' list.                                */
    /*                                                                  */
    /* The query result would be all records whose bounding box         */
    /* attribute value is in the 'in' range but is not in any of the    */
    /* elements of the 'not in' list.                                   */
    /*                                                                  */
    /* return 0 if OK. return negtive number if something is wrong      */
    /********************************************************************/
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      */
             BoundingBoxList *notIn);       /* IN: The "not in" ranges */

    /********************************************************************/
    /* The iterating function. Attempting to get the next record if     */
    /* there is any.                                                    */
    /*                                                                  */
    /* return 0 if OK                                                   */
    /* return NO_MORE_RECORD (-1) if no more record                     */
    /********************************************************************/
    virtual int
    nextRec(void *&record);                 /* OUT: pointer to record  */

    /********************************************************************/
    /* Close the scan.                                                  */
    /*                                                                  */
    /* No matter what state the scan is in now and no matter whether    */
    /* there is actually more record or not, close it. Do clean up      */
    /* as necessary.                                                    */
    /********************************************************************/
    virtual void
    closeScan();

    /* print out info about this instance */
    /* be concise if verbose == 0 */
    /* be verbose if verbose != 0 */
    virtual void output(int verboe);

private:

    RBMObject _obj;		/* the object */

    BoundingBox *_in;		/* The query's in */
    BoundingBoxList *_notIn;	/* the query's not-in */

    bool _scanOutstanding;	/* If any query is active */

    /* should get rid of this since now we use array of pages */
    DteAdtPage _page;		/* there is one page of record */

    vector<char*> _inPages;	/* there is an array of pages of records */
    size_t _curPage;		/* current page in the page list */
    int _curSlot;		/* # of records read out of the page so far. */
				/* May be more than or equal to # of records */
				/* returned so far */
};

#endif
