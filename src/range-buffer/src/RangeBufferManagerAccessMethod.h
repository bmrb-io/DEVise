#ifndef __RANGEBUFFERMANAGERACCESSMETHOD_H__
#define __RANGEBUFFERMANAGERACCESSMETHOD_H__

#include "RangeAccessMethod.h"
#include "BoundingBox.h"
#include "BoundingBoxList.h"
#include "BBoxEntry.h"

class RangeBufferManagerAccessMethod: public RangeAccessMethod
{

public:

    RangeBufferManagerAccessMethod(RBMObject obj,		/* IN: Object */
				   RangeAccessMethod *ram); 	/* IN: pointer
								       to RAM in
								       lower
								       level */

    virtual ~RangeBufferManagerAccessMethod();

    /* return 0 if OK. return negtive number if something is wrong */
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      */
             BoundingBoxList *notIn);       /* IN: The "not in" ranges */

    /* return 0 if OK */
    /* return -1 if no more record */
    virtual int
    nextRec(void *&record);                  /* OUT: pointer to record  */

    virtual void
    closeScan(); 

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

    /* unpack record into _unpackedRecordAbove */
    void unpackRecord(void *in_mem_record);

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
};

#endif
