#ifndef __RANGEACCESSMETHOD_H__
#define __RANGEACCESSMETHOD_H__

#include "RBMObject.h"
#include "BoundingBox.h"
#include "BoundingBoxList.h"

class RangeAccessMethod
{

public:

    /********************************************************************/
    /* virtual destructor 						*/
    /********************************************************************/
    virtual ~RangeAccessMethod();

    /********************************************************************/
    /* The open scan function. Specifies the query composed of the 	*/
    /* 'in' range and the 'not in' list.				*/
    /*									*/
    /* The query result would be all records whose bounding box		*/
    /* attribute value is in the 'in' range but is not in any of the	*/
    /* elements of the 'not in' list.					*/
    /* 									*/
    /* return 0 if OK. return negtive number if something is wrong 	*/
    /********************************************************************/
    virtual int
    openScan(BoundingBox *in,               /* IN: The "in" range      	*/
             BoundingBoxList *notIn) = 0;   /* IN: The "not in" ranges 	*/

    /********************************************************************/
    /* The iterating function. Attempting to get the next record if	*/
    /* there is any.							*/
    /* 									*/
    /* return 0 if OK 							*/
    /* return NO_MORE_RECORD (-1) if no more record 			*/
    /********************************************************************/
    virtual int
    nextRec(void *&record) = 0;             /* OUT: pointer to record  	*/

    /********************************************************************/
    /* Close the scan.							*/
    /* 									*/
    /* No matter what state the scan is in now and no matter whether	*/
    /* there is actually more record or not, close it. Do clean up	*/
    /* as necessary.							*/
    /********************************************************************/
    virtual void
    closeScan() = 0;

    /********************************************************************/
    /* Print out the state of this RangeAccessMethod.			*/
    /********************************************************************/
    virtual void output(int verbose) = 0;
};

#endif
