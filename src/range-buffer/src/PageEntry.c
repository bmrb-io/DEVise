#include "PageEntry.h"

PageEntry::PageEntry(DteAdtPage *page, char *buf)
{
    _page = page;
    _buf = buf;
}

PageEntry::~PageEntry()
{
    if (_page)
    {
    	delete _page;
    }

    /* Do not delete _buf */
    /* Instead, let who ever that wants to delete this PageEntry to worry */
    /* about it! */
}
