/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:14:16  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <libc.h>
/*
#include <sysent.h>
*/
#include "BufPage.h"
#include "Exit.h"

BufPage *BufPage::_startAddr = NULL;
BufPage *BufPage::_endAddr = NULL;
BufPage *BufPage::_nextAlloc = NULL;
Boolean BufPage::_initialized = false;
DataPage *BufPage::_startDataAddr = NULL; /* address of starting page data */
DataPage *BufPage::_endDataAddr = NULL; /* end address of page data */
DataPage *BufPage::_nextDataAlloc = NULL; /* next place to allocate buffer */

int BufPage::_maxPages = -1;	/* max # of pages to allocate */

/********************************************************
Set max # of pages to allocated
**********************************************************/
void BufPage::SetMaxPages(int pages){
	if (_maxPages != -1){
		fprintf(stderr,"BufPage: maxPages set more than once\n");
		Exit::DoExit(1);
	}
	_maxPages = pages;
}

/***********************************************************
Buffer page allocation
************************************************************/
void *BufPage :: operator new(size_t sz){
static int numAlloc = 0;

	numAlloc ++;
	if (sz != sizeof(BufPage)){
		fprintf(stderr,"BufPage::new: size %d != buffer page size %d\n",
				sz, sizeof(BufPage));
		Exit::DoExit(2);
	}

	if (!_initialized){
		if (_maxPages <= 0){
			fprintf(stderr,"BufMgr::new(): max pages(%d) <= 0\n",_maxPages);
			Exit::DoExit(2);
		}
		/*
		if ((_startAddr = (BufPage *)malloc(sizeof(BufPage)*_maxPages))==NULL){
			fprintf(stderr,"BufPage::new can't alloc memory\n");
			Exit::DoExit(2);
		}
		*/
		if ((_startAddr = (BufPage *)sbrk(sizeof(BufPage)*_maxPages+4))== 
			(BufPage *)-1){
			fprintf(stderr,"BufPage::new can't alloc memory\n");
			Exit::DoExit(2);
		}
		unsigned long addr = (unsigned long) _startAddr;
		if (addr % 8 != 0){
			/* round off the address to the next higher multiple of 8 */
			addr = addr /8*8 + 8;
			_startAddr = (BufPage *)addr;
		}
		printf("BufPage: allocating %d bytes addr = 0x%p\n",
			sizeof(BufPage)*_maxPages, _startAddr);
		_endAddr = &_startAddr[_maxPages];
		_nextAlloc = _startAddr;

		if ((_startDataAddr = (DataPage *)sbrk(sizeof(DataPage)*_maxPages+4))== 
			(DataPage *)-1){
			fprintf(stderr,"BufPage::new can't alloc Data memory\n");
			Exit::DoExit(2);
		}
		addr = (unsigned long) _startDataAddr;
		if (addr % 8 != 0){
			/* round off the address to the next higher multiple of 4 */
			addr = addr /8*8 + 8;
			_startDataAddr = (DataPage *)addr;
		}
		_endDataAddr = &_startDataAddr[_maxPages];
		_nextDataAlloc = _startDataAddr;

		_initialized = true;
	}

	if (_nextAlloc >= _endAddr){
		fprintf(stderr,"BufPage::new() out of memory\n");
		fprintf(stderr,"allocating %d pages out of %d\n",numAlloc, _maxPages);
		Exit::DoExit(2);
	}

	BufPage *page = _nextAlloc++;
	DataPage *dataPtr = _nextDataAlloc++;
	page->_data = dataPtr;
	return page;
}

void BufPage:: operator delete(void *){
	fprintf(stderr,"BufPage::delete() not allowed\n");
	Exit::DoExit(2);
}

/**************************************************************
Increment inuse count of page
*****************************************************************/
void BufPage::ClearUse() { 
	if (--_inUse < 0){
		fprintf(stderr,"BufPage::ClearUse: use count < 0\n");
		Exit::DoExit(1);
	}
}


