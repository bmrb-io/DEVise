#ifndef __PAGEENTRY_H__
#define __PAGEENTRY_H__

/* Kevin's page/record stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

class PageEntry
{

public:

    PageEntry(DteAdtPage *page, char *buf);
    ~PageEntry();

    friend class BBoxEntry;
    friend class RangeBufferManager;
    friend class RangeBufferManagerAccessMethod;

private:

    DteAdtPage *_page;
    char *_buf;

    PageEntry *_prev;
    PageEntry *_next;
};

#endif
