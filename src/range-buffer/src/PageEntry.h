#ifndef __PAGEENTRY_H__
#define __PAGEENTRY_H__

/* Kevin's page/record stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

/************************************************************************/
/* An element in the linked list hanging off a BBoxEntry.		*/
/* Keeps a page and enables the linked list.				*/
/************************************************************************/
class PageEntry
{

public:

    /* Initialize with specified parameters */
    PageEntry(DteAdtPage *page, char *buf);

    /**********************************************************************/
    /* Do not delete _buf 						  */
    /* Instead, let who ever that wants to delete this PageEntry to worry */
    /* about it! 							  */
    /**********************************************************************/
    ~PageEntry();

    friend class BBoxEntry;
    friend class RangeBufferManager;
    friend class RangeBufferManagerAccessMethod;

private:

    DteAdtPage *_page;		/* pointer to DteAdtPage object */
    char *_buf;			/* Memory location */

    PageEntry *_prev;		/* next in list 	*/
    PageEntry *_next;		/* next in linked list 	*/
};

#endif
