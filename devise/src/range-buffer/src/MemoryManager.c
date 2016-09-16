#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "MemoryManager.h"

/* Kevin's page/record stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

MemoryManager::MemoryManager(unsigned int size)
{
    assert (size >= 0);
    _maxNumPage = size;
    _numPage = 0;
}

MemoryManager::~MemoryManager()
{
    /* Don't do anything ... */
}

void *MemoryManager::generateNewPage()
{
    void *p;

    if (_numPage >= _maxNumPage)
    {
	return NULL;
    }

    p = (void*)malloc(DTE_PAGESIZE);
    if (p == NULL)
    {
	printf("malloc failed in MemoryManager::getPage\n");
	exit (1);
    }

    _numPage++;

    return p;
}

void MemoryManager::recyclePage(void *page)
{
    delete page;
    _numPage--;
}
