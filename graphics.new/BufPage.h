/*
  $Id$

  $Log$*/

#ifndef BufPage_h
#define BufPage_h
#include <stdio.h>
#include "DeviseTypes.h"
#include "PageSize.h"


/* A buffer page contains: page file, page number, dirty bit,
	in use count (increment by 1 every time it is
	to be fixed in memory, decrement by 1 when the page
	is no longer needed in memory),
	page data, iteration number, and index number.
	If a page's iteration number == iteration number of the
	buffer manager, then  it's on the in future list. Otherwise,
	it is on the current list
	The index == page's index in the array of pages when RND policy is used.
Note:
	Each page can be on 2 lists at the same time:
		the hash table list and (current list or future list)
	In order to improve performance, the hash tabel access
	the "nextHash" and "prevHash" fields directly.
	The BufPageList class access the "next" and "prev" fields
	directly.
*/
class DiskFile;
typedef char DataPage[DISKFILE_PAGESIZE];
class BufPage {
public:
	void *PageData() { return _data; };

	/* page number of this page */
	int PageNum() { return _pageNum; };

	static void SetMaxPages(int);		/* set max # of pages to allocated */


protected:
	friend class BufMgr;

	Boolean Dirty() { return _dirty; };
	Boolean InUse() { return (_inUse != 0); };
	DiskFile *GetDiskFile(){ return _pfile; };
	int Iteration() { return _iteration; }

	/* Initialize the page. Set dirty to false, and inuse count to 0*/
	void Init(DiskFile *pfile, int page, int iteration, int index){
		_pageNum = page;
		_pfile = pfile;
		_dirty = false;
		_inUse = 0;
		_iteration = iteration;
		_index = index;
	}

	/* set/clear dirty bit */
	void SetDirtyBit() { _dirty = true; };
	void ClearDirtyBit() { _dirty= false; };

	/* Update In Use count */
	void SetUse(){ ++_inUse ; };
	void ClearUse();

	/* set/get the index */
	void SetIndex(int index) { _index = index; };
	int GetIndex(){return _index; };

	/* set iteration number */
	void SetIteration(int iter) { _iteration = iter; };

	/* for buffer manager linked list */
	friend class BufPageList;
	BufPage *next, *prev;

	/* for hash table linked list*/
	friend class BufHash;
	BufPage *nextHash, *prevHash;

	/* new and delete functions to allocate space for page */
	void *operator new(size_t);  
	void operator delete(void *);


private:
	void *_data;
	int _pageNum;		/* page number */
	int _inUse;	/* Count of # of times this page is to be fixed in memory*/
	Boolean _dirty;	/* TRUE if this buffer is dirty */
	DiskFile *_pfile; /* DiskFile serving as backing store for
					this page */
	int _iteration; /* iteration of this page */	
	int _index; /* index in RND list */

	/* These are for buffer page allocation */
	static BufPage *_startAddr;	/* address of starting page */
	static BufPage *_endAddr;	/* ending address */
	static BufPage *_nextAlloc;	/* next place to allocate buffer */
	static Boolean _initialized; /* TRUE if we initialized _startAddr, etc */
	static int _maxPages;	/* # of pages to allocated */

	/* These are for page data allocation */
	static DataPage *_startDataAddr; /* address of starting page */
	static DataPage *_endDataAddr;
	static DataPage *_nextDataAlloc; /* next place to allocate buffer */
};

#endif
