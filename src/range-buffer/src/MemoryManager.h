#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

/************************************************************************/
/* Manage pages.							*/
/* Provides a page when requested if the limit has not been exceeded.	*/
/* Recycle pages when asked.						*/
/*									*/
/* The current MemoryManager simply does malloc() or free() upon each	*/
/* request. If justified, a future extension may be adding its own	*/
/* page management (e.g., keeping track of free pages.			*/
/************************************************************************/
class MemoryManager
{

public:

    /* size is Max number of pages that this MemoryManager manages */
    MemoryManager(unsigned int size);

    /* destructor */
    ~MemoryManager();

    /* Return pointer to page if a page is successfully provided, */
    /* Return NULL otherwist */
    void *generateNewPage();

    /* Caller no longer needs the page pointed to by page. Recycle. */
    void recyclePage(void *page);

private:

    /* max # of page that can be managed by this MemoryManager */
    unsigned int _maxNumPage;

    /* current # of pages in use */
    unsigned int _numPage;

};

#endif
