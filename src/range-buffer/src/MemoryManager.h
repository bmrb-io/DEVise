#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

class MemoryManager
{

public:

    /* size is Max number of pages that this MemoryManager manages */
    MemoryManager(unsigned int size);
    ~MemoryManager();

    /* Return pointer to page if a page is successfully provided, */
    /* Return NULL otherwist */
    void *generateNewPage();

    /* caller no longer needs the page pointed to by page */
    void recyclePage(void *page);

private:

    /* max # of page that can be managed by this MemoryManager */
    unsigned int _maxNumPage;

    /* current # of pages in use */
    unsigned int _numPage;

};

#endif
