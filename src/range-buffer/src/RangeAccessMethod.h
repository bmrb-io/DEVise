#ifndef __RANGEACCESSMETHOD_H__
#define __RANGEACCESSMETHOD_H__

#include "RBMObject.h"
#include "BoundingBox.h"
#include "BoundingBoxList.h"

class RangeAccessMethod
{

public:

    virtual ~RangeAccessMethod();

    /* return 0 if OK. return negtive number if something is wrong */
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      */
             BoundingBoxList *notIn) = 0;   /* IN: The "not in" ranges */

    /* return 0 if OK */
    /* return NO_MORE_RECORD (-1) if no more record */
    virtual int
    nextRec(void *&record) = 0;             /* OUT: pointer to record  */

    virtual void
    closeScan() = 0;

    virtual void output(int verbose) = 0;
};

#endif
