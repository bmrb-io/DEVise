#ifndef __RANGEBUFFERMANAGERACCESSMETHOD_H__
#define __RANGEBUFFERMANAGERACCESSMETHOD_H__

#include "RangeAccessMethod.h"
#include "BoundingBox.h"
#include "BoundingBoxList.h"
#include "BBoxEntry.h"

class RangeBufferManagerAccessMethod: public RangeAccessMethod
{

public:

    /********************************************************************/
    /* Constructor.							*/
    /* 									*/
    /* Specifies the object to be queried on as well as	a handle to a	*/
    /* low level RangeAccessMethod where this				*/
    /* RangeBufferManagerAccessMethod can get more records if what is	*/
    /* currently cache doesn't include the entire answer to the query.	*/
    /*									*/
    /* Must be called before any query (scan) can be issued.		*/
    /********************************************************************/
    RangeBufferManagerAccessMethod(RBMObject obj,	
						/* IN: Object 		*/

				   RangeAccessMethod *ram); 
						/* IN: pointer to RAM	*/
						/*     in lower level 	*/

    /********************************************************************/
    /* Destructor							*/
    /*									*/
    /* Must be called when there is no scan outstanding.		*/
    /********************************************************************/
    virtual ~RangeBufferManagerAccessMethod();

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
    openScan(BoundingBox *in,               /* IN: The "in" range      	*/
             BoundingBoxList *notIn);       /* IN: The "not in" ranges 	*/

    /********************************************************************/
    /* The iterating function. Attempting to get the next record if     */
    /* there is any.                                                    */
    /*                                                                  */
    /* return 0 if OK                                                   */
    /* return NO_MORE_RECORD (-1) if no more record                     */
    /********************************************************************/
    virtual int
    nextRec(void *&record);                  /* OUT: pointer to record  */

    /********************************************************************/
    /* Close the scan.                                                  */
    /*                                                                  */
    /* No matter what state the scan is in now and no matter whether    */
    /* there is actually more record or not, close it. Do clean up      */
    /* as necessary.                                                    */
    /********************************************************************/
    virtual void
    closeScan(); 

    /********************************************************************/
    /* Print out the state of this RangeBufferManagerAccessMethod.      */
    /********************************************************************/
    void output(int verbose);

private:

    /* use in and notIn, check with what's currently buffered, and */
    /* generates the right _lowIn and _lowNotIn for the low level */
    /* RAM's openScan */
    /* _lowIn and _lowNotIn should change from NULL to not NULL here */
    int generateLowLevelBoundingBox(BoundingBox *in, BoundingBoxList *notIn);

    /* for all the bounding boxes for this object corresponding to */
    /* records asked for by the */
    /* current query but not previously existing in Memory, create the */
    /* BBoxEntry for them. Mark all these BBoxEntry as incomplete */
    /* called only before first time going to low level RAM */
    void createHoleBBoxes();

    /* Do NOT delete empty BBoxEntry associated with this object. */
    /* Mark all BBoxEntry for this object as complete */
    /* make decisions whether to merge BBoxEntries or not */
    /* called only when the last record from low level has just been returned */
    void cleanUpBBoxEntries();

    /* find the next in-memory record and return pointer to it */
    /* return NULL if no more record */
    void *findNextInMemRec();

    /* insert lowLevelRec into the appropriate bounding box entry */
    int insertRecord(void *lowLevelRec);

    /* Given _in and _notIn, decide if this bbe can possibly contain any */
    /* record that satisfy the query */
    bool boundingBoxQulify(BBoxEntry *bbe);

    RBMObject _obj;
    RangeAccessMethod *_ram;
    BoundingBox *_in;
    BoundingBoxList *_notIn;
    bool _scanOutstanding;
    bool _inMemPhase;		/* true means dealing with in-M records */
				/* false means has started on low level data
				   already				*/
    BoundingBox *_lowIn;        /* used only to call low level */
                                /* RAM's openScan() */
    BoundingBoxList *_lowNotIn; /* used only to call low level */
                                /* RAM's openScan() */

    BBoxEntry *_nextToConsider;
				/* Where to start searching upon the next */
				/* insertRecord() call */

    bool _lowLevelScanActive;	/* True iff low level scan has been opened */
				/* but we are not sure if low level has	   */
				/* returned all records it has.		   */

				/* If closeScan() is called when 	   */
				/* _lowLevelScanActive is true, must 	   */
				/* remove all bboxes created for low level */
				/* scan since they may be incomplete.	   */
};

#endif
