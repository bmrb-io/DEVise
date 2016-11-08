/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.6  1999/01/13 15:58:07  beyer
  fixed static const

  Revision 1.5  1997/12/19 00:05:56  donjerko
  Changes made be Kevin to get DTE to compile.

  Revision 1.4  1997/02/07 22:01:13  liping
  Fixed complile problem

  Revision 1.3  1997/02/03 19:45:29  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.2  1996/12/13 21:34:39  jussi
  Added checking of available semaphores and shared memory.

  Revision 1.1  1996/12/03 20:28:49  jussi
  Initial revision.
*/

#ifndef MemMgr_h
#define MemMgr_h

#include "DeviseTypes.h"

// Memory manager

class MemMgr {
  public:
    MemMgr(int numPages, int pageSize, int &status);
    ~MemMgr();

    // Page types
    enum PageType { Cache, Buffer };

    // Set maximum usage of a page type
    int SetMaxUsage(PageType type, int pages);

    // Allocate a memory buffer of one page; block until memory available
    int Allocate(PageType type, char *&buf) {
        int pages = 1;
        return Allocate(type, buf, pages);
    }

    // Allocate a memory buffer of given size; block until at least one
    // page available
    int Allocate(PageType type, char *&buf, int &pages, Boolean block = true);

    // Try to allocate a memory buffer of one page; return -1 if no
    // memory available
    int Try(PageType type, char *&buf) {
        int pages = 1;
        return Try(type, buf, pages);
    }        

    // Try to allocate a memory buffer of given size; return -1 if no
    // memory available
    int Try(PageType type, char *&buf, int &pages) {
        return Allocate(type, buf, pages, false);
    }

    // Deallocate a memory buffer of given size
    int Deallocate(PageType type, char *buf, int pages = 1);

    // Convert buffer memory to cache memory, or vice versa
    int Convert(char *buf, PageType oldType, PageType &newType);

    // Return number of free memory size in pages
    int NumFree() {
        AcquireMutex();
        int num = _count->free;
        ReleaseMutex();
        return num;
    }

    // Dump contents of free page table
    void Dump();

    int PageSize() { return _pageSize; }
    int NumPages() { return _numPages; }

    static MemMgr *Instance() { return _instance; }

  protected:
    // Allocation and initialization
    int SetupLocalMemory();
    int Initialize();

    // Acquire and release mutex
    void AcquireMutex() {}
    void ReleaseMutex() {}

    // Acquire and release free semaphore
    void AcquireFree() {}
    void ReleaseFree() {}

    // Number of memory pages, free page table size, and page size
    const int _numPages;
    const int _tableSize;
    const int _pageSize;

    // Base address of memory
    char *_buf;

    char **_freePage;                   // table of free memory chunks
    int *_freePageCount;                // # of pages in each chunk
    struct CountStruct {
        int entries;                    // valid entries in free table
        int free;                       // # of free pages left
        int cache;                      // # of cache pages in use
        int buffer;                     // # of buffer pages in use
        int maxCache;                   // max # of cache pages allowed
        int maxBuffer;                  // max # of buffer pages allowed
    } *_count;

    // An instance of this class
    static MemMgr *_instance;
};

// Data types for I/O

typedef unsigned long long streampos_t;
typedef unsigned long long bytecount_t;
typedef unsigned long iosize_t;

//TEMP?>>>
class DataPipe {
  public:
    DataPipe(int maxSize, int &status) {}
    ~DataPipe() {}

    int Consume(char *&buf, streampos_t &offset, iosize_t &bytes) { return 0; }
    int Produce(char *buf, streampos_t offset, iosize_t bytes) { return 0; }
    int SetSize(int size) { return 0; }

    int NumData() { return 0; }
};
//<<<

#endif
