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

class PhonyRAM: public RangeAccessMethod
{

public:

    PhonyRAM(RBMObject obj, DteAdtPage *page);
    virtual ~PhonyRAM();

    /* return 0 if OK. return negtive number if something is wrong */
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      */
             BoundingBoxList *notIn);       /* IN: The "not in" ranges */

    /* return 0 if OK */
    /* return NO_MORE_RECORD (-1) if no more record */
    virtual int
    nextRec(void *&record);                 /* OUT: pointer to record  */

    virtual void
    closeScan();

    virtual void output(int verboe);

private:

    RBMObject _obj;

    BoundingBox *_in;
    BoundingBoxList *_notIn;

    bool _scanOutstanding;

    DteAdtPage *_page;		/* there is one page of record */
    int _totalNumRecs;		/* total # of records in the page */
    int _numReadSofar;		/* # of records read out of the page so far. */
				/* May be less than or equal to # of records */
				/* returned so far */
};

#endif
