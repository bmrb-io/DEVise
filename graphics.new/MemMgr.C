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
  Revision 1.10  1998/12/01 20:04:32  wenger
  More reductions of memory usage in DEVise -- basically eliminated the
  histogram capability (this really saves a lot, since there are big
  structures in every ViewGraph for this); made creation of TDatas more
  efficient by bypassing command code.

  Revision 1.9  1998/03/04 19:11:03  wenger
  Fixed some more dynamic memory errors.

  Revision 1.8  1997/12/19 00:05:56  donjerko
  Changes made be Kevin to get DTE to compile.

  Revision 1.7  1997/12/11 04:25:42  beyer
  Shared memory and semaphores are now released properly when devise
  terminates normally.

  Revision 1.6  1996/12/31 18:45:54  jussi
  Improve error handling in case shared memory is not available.

  Revision 1.5  1996/12/20 16:30:01  jussi
  Removed call to SemaphoreV::destroy().

  Revision 1.4  1996/12/18 15:31:35  jussi
  Improved the destructors so that IPC structures are properly destroyed.

  Revision 1.3  1996/12/13 22:59:39  jussi
  Added missing return statement.

  Revision 1.2  1996/12/13 21:34:38  jussi
  Added checking of available semaphores and shared memory.

  Revision 1.1  1996/12/03 20:28:48  jussi
  Initial revision.
*/

#include <stdio.h>
#include <memory.h>

#include "MemMgr.h"
#include "Exit.h"

#define DEBUGLVL 0

MemMgr *MemMgr::_instance = 0;

MemMgr::MemMgr(int numPages, int pageSize, int &status) :
	_numPages(numPages), _tableSize(numPages), _pageSize(pageSize)
{
#if defined(DEBUG)
    printf("MemMgr::MemMgr(%d, %d)\n", numPages, pageSize);
#endif

    _instance = this;

    status = SetupLocalMemory();
    if (status >= 0) {
        status = Initialize();
    }
}

int MemMgr::SetupLocalMemory()
{
    // We need space for page and also address in _freePage
    int size = _numPages * _pageSize
               + _tableSize * (sizeof(char *) + sizeof(int))
               + sizeof(CountStruct);

    _buf = new char [size];
    if (!_buf) {
      fprintf(stderr, "Cannot allocate local memory\n");
      return -1;
    }
#if DEBUGLVL >= 1
    printf("Created a %d-byte local memory buffer at 0x%p\n", size, _buf);
#endif

    return 0;
}

int MemMgr::Initialize()
{
#if DEBUGLVL >= 1
    printf("Initializing memory manager\n");
#endif

    memset(_buf, 0, _numPages * _pageSize);

#if DEBUGLVL >= 1
    printf("Creating free, cache, and buffer lists\n");
#endif

    _freePage = (char **)(_buf + _numPages * _pageSize);
    _freePageCount = (int *)(_freePage + _tableSize);
    _count = (CountStruct *)(_freePageCount + _tableSize);

    // Initially, there is one contiguous free memory area
    _freePage[0] = _buf;
    _freePageCount[0] = _numPages;

    for(int i = 1; i < _tableSize; i++) {
        _freePage[i] = 0;
        _freePageCount[i] = 0;
    }

    _count->entries = 1;
    _count->free = _numPages;
    _count->cache = 0;
    _count->buffer = 0;
    _count->maxCache = _numPages;
    _count->maxBuffer = _numPages;

    return 0;
}

MemMgr::~MemMgr()
{
    delete [] _buf;
}

int MemMgr::SetMaxUsage(PageType type, int pages)
{
    AcquireMutex();
    if (type == Cache)
        _count->maxCache = pages;
    else
        _count->maxBuffer = pages;
    ReleaseMutex();

    return 0;
}

int MemMgr::Allocate(PageType type, char *&buf, int &pages, Boolean block)
{
    // Return immediately if no pages requested
    if (pages <= 0)
        return -1;

    AcquireMutex();

    // Return immediately if no free pages available in non-blocking mode,
    // or if no pages of given type are available.
    if (!block) {
        if (!_count->free
            || (type == Cache && _count->cache >= _count->maxCache)
            || (type == Buffer && _count->buffer >= _count->maxBuffer)) {
            ReleaseMutex();
            return -1;
        }
    }

    while (!_count->free) {
#if DEBUGLVL >= 5
        printf("Out of free pages: %d cache, %d buffer\n",
               _count->cache, _count->buffer);
#endif
        ReleaseMutex();
        AcquireFree();
        AcquireMutex();
#if DEBUGLVL >= 5
        printf("Woke up from sleep, %d free pages, %d cache, %d buffer\n",
               _count->free, _count->cache, _count->buffer);
#endif
    }
           
    // Find first contiguous area that is at least the size requested.
    // Make note of largest contiguous area smaller than requested size.

    int pick = -1;
    int i = 0;
    for(; i < _count->entries; i++) {
        if (_freePageCount[i] >= pages) {
#if DEBUGLVL >= 3
            printf("Found %d-page contiguous block at index %d\n",
                   _freePageCount[i], i);
#endif
            pick = i;
            break;
        }
        if (pick < 0 || _freePageCount[i] > _freePageCount[pick])
            pick = i;
    }

    DOASSERT(pick >= 0 && pick < _count->entries, "Invalid index");

    if (_freePageCount[pick] < pages) {
#if DEBUGLVL >= 3
        printf("Reducing %d-page request to largest available %d\n",
               pages, _freePageCount[pick]);
#endif
        pages = _freePageCount[pick];
        DOASSERT(pages > 0, "Invalid page count");
    }

    DOASSERT(_count->free >= pages, "Invalid free count");
    _count->free -= pages;
    buf = _freePage[pick];
    DOASSERT(buf, "Invalid page");
    _freePageCount[pick] -= pages;

    // If nothing left of memory chunk, move another chunk into this
    // table position. Otherwise, adjust memory chunk location.

    if (!_freePageCount[pick]) {
        _count->entries--;
        if (pick < _count->entries) {
            _freePage[pick] = _freePage[_count->entries];
            _freePageCount[pick] = _freePageCount[_count->entries];
            _freePage[_count->entries] = 0;
            _freePageCount[_count->entries] = 0;
        } else {
            _freePage[pick] = 0;
        }
    } else {
        _freePage[i] = buf + pages * _pageSize;
    }

    if (type == Buffer)
        _count->buffer += pages;
    else
        _count->cache += pages;

#if DEBUGLVL >= 3
    printf("Allocated %d %s page(s) at 0x%p\n",
           pages, (type == Cache ? "cache" : "buffer"), buf);
#endif

#if DEBUGLVL >= 5
    printf("Memory allocation table now:\n");
    Dump();
#endif

    ReleaseMutex();

    return 0;
}

int MemMgr::Deallocate(PageType type, char *buf, int pages)
{
    AcquireMutex();

    DOASSERT(buf, "Invalid page");
    DOASSERT(_count->free + pages <= _numPages, "Invalid page");

    // Find free page area that merges perfectly with deallocated range.

    char *endBuf = buf + pages * _pageSize;
    int mergeLeft = -1;
    int mergeRight = -1;
    for(int i = 0; i < _count->entries; i++) {
        DOASSERT(_freePageCount[i] > 0, "Invalid free page count");
        DOASSERT(_freePage[i], "Invalid free memory area");
        char *endFreeBuf = _freePage[i] + _freePageCount[i] * _pageSize;
        if (_freePage[i] == endBuf)
            mergeLeft = i;
        if (buf == endFreeBuf)
            mergeRight = i;
    }

    if (mergeLeft >= 0 && mergeRight >= 0) {
        // Freed area sits perfectly between two previously freed areas
        DOASSERT(mergeLeft != mergeRight, "Impossible merge");
        _freePageCount[mergeRight] += pages + _freePageCount[mergeLeft];
        _count->entries--;
        if (mergeLeft < _count->entries) {
            _freePage[mergeLeft] = _freePage[_count->entries];
            _freePageCount[mergeLeft] = _freePageCount[_count->entries];
            _freePage[_count->entries] = 0;
            _freePageCount[_count->entries] = 0;
        } else {
            _freePageCount[mergeLeft] = 0;
            _freePage[mergeLeft] = 0;
        }
    } else if (mergeLeft >= 0) {
        // Freed area is just to the left of a previously freed area
        _freePage[mergeLeft] -= pages * _pageSize;
        _freePageCount[mergeLeft] += pages;
    } else if (mergeRight >= 0) {
        // Freed area is just to the right of a previously freed area
        _freePageCount[mergeRight] += pages;
    } else {
        // Freed area is not adjacent to any previously freed area
        int freeEntry = _count->entries;
        DOASSERT(freeEntry <= _tableSize, "Inconsistent state");
        DOASSERT(!_freePage[freeEntry] && !_freePageCount[freeEntry],
                 "Entry not free");
        _freePage[freeEntry] = buf;
        _freePageCount[freeEntry] = pages;
        _count->entries++;
    }

    _count->free += pages;

#if DEBUGLVL >= 3
    printf("Deallocated %d %s page(s) at 0x%p\n",
           pages, (type == Cache ? "cache" : "buffer"), buf);
#endif

    if (type == Buffer) {
        _count->buffer -= pages;
#if DEBUGLVL >= 3
        printf("Now %d buffer pages remain in use\n", _count->buffer);
#endif
    } else {
        _count->cache -= pages;
#if DEBUGLVL >= 3
        printf("Now %d cache pages remain in use\n", _count->cache);
#endif
    }

#if DEBUGLVL >= 5
    printf("Memory allocation table now:\n");
    Dump();
#endif

    DOASSERT(_count->cache >= 0 && _count->buffer >= 0, "Inconsistent state");

    // If someone is waiting for a free page, signal it
    if (_count->free == pages)
        ReleaseFree();

    ReleaseMutex();

    return 0;
}

int MemMgr::Convert(char *page, PageType oldType, PageType &newType)
{
    AcquireMutex();

    page = page;

    if (oldType != newType) {
        if (oldType == Cache) {
            _count->cache--;
            _count->buffer++;
        } else {
            _count->cache++;
            _count->buffer--;
        }

#if DEBUGLVL >= 3
        printf("Memory manager now has %d cache pages, %d buffer pages\n",
               _count->cache, _count->buffer);
#endif
    }

    ReleaseMutex();

    return 0;
}

void MemMgr::Dump()
{
    printf("Free %d, buffer %d (%d max), cache %d (%d max), entries %d\n",
           _count->free, _count->buffer, _count->maxBuffer,
           _count->cache, _count->maxCache, _count->entries);
    for(int i = 0; i < (_tableSize > 10 ? 10 : _tableSize); i++) {
        printf("memory[%d] = 0x%p, %d pages\n", i, _freePage[i],
               _freePageCount[i]);
    }
}
