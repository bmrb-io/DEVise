/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.2  1995/09/05 22:15:05  jussi
  Added CVS header.
*/

/***************************************************************************
a PageFile uses the buffer manager to do paged I/O.
            PageFile
               |
               V
            Buf Mgr
               |
               V
            Disk File

Layout of DiskFile:
    page 0: stores # of pages in the file.
    page 1: data page 1
    page 2: data page 2
    ...
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "BufMgr.h"
#include "DiskFile.h"
#include "PageFile.h"
#include "PageRange.h"

/**********************************************************************
constructor
************************************************************************/
PageFile::PageFile(char *name, BufMgr *mgr, int maxAlloc){
	BufPage *bpage;
	_mgr = mgr;
	if ((_diskFile = DiskFile::OpenFile(name))!= NULL){
		/* File alredy exists. Get # of pages */
		bpage = mgr->GetAndFixPage(_diskFile,0);
		_header = *((PageFileHeader *)(bpage->PageData()));
		if (_header.numPages <0 ){
			fprintf(stderr,"PageFile::PageFile: invalid # of pages %d\n",
				_header.numPages);
			Exit::DoExit(1);
		}
		_mgr->UnfixPage(bpage,Current );
	}
	else {
		/* Create a new file */
		if ((_diskFile = DiskFile::CreateFile(name))== NULL){
			fprintf(stderr,"PageFile::PageFile(%s,): can't create file\n",
				name);
			Exit::DoExit(1);
		}
		bpage = mgr->CreateAndFixPage(_diskFile,0);
		/* # of pages == 0 */
		_header.numPages = 0;
		_header.pagesAlloc = 0;
		_header.maxAlloc = maxAlloc;
		*((PageFileHeader *)(bpage->PageData())) = _header;
		mgr->DirtyPage(bpage);
		mgr->UnfixPage(bpage, Current);
	}

}

/****************************************************************
Destructor: remove all pages from buffer manager
and quit
*****************************************************************/
PageFile::~PageFile(){

	/* write # of pages onto disk */
	BufPage *bpage = _mgr->GetAndFixPage(_diskFile, 0);
	*((PageFileHeader *)(bpage->PageData())) = _header;
	_mgr->DirtyPage(bpage);
	_mgr->UnfixPage(bpage, Current);

	/* clear buffer manager of all pages from disk file */
	_mgr->RemoveDiskFile(_diskFile);

	/* remove the disk file */
	delete _diskFile;
}

/*************************************************************
Get the data for the given page and fix it in
memory. Pages numbers start from 1 .. NUmPages().
**************************************************************/
BufPage *PageFile::GetPage(int pageNum, Boolean isPrefetch){
	if (pageNum < 1 || pageNum > _header.numPages){
		fprintf(stderr,"PageFile::GetPage(%d,): invalid page number\n",pageNum);
		fprintf(stderr,"\tOnly %d pages available\n", _header.numPages);
		Exit::DoExit(1);
	}

	/* get page from buffer manager */
	return  _mgr->GetAndFixPage(_diskFile,pageNum, isPrefetch);
}

Boolean PageFile::PageInMem(int pageNum){
	return _mgr->PageInMem(_diskFile, pageNum);
}

/*********************************************************************
Create a new page and fix it in memory.
Return NULL if max # of pages exceeded.
**********************************************************************/
BufPage *PageFile::CreatePage(int &pageNum){
	if (_header.maxAlloc > 0 && _header.pagesAlloc >= _header.maxAlloc)
		return NULL;
	pageNum = ++_header.numPages;
	_header.pagesAlloc++;
	return _mgr->CreateAndFixPage(_diskFile, pageNum);
}



/******************************************************************
Mark page dirty 
*****************************************************************/
void PageFile::DirtyPage(BufPage *bpage, Boolean flush){
	_mgr->DirtyPage(bpage, flush);
}

/*************************************************************************
Unfix page
************************************************************************/
void PageFile::UnfixPage(BufPage *bpage, BufHint hint){
	_mgr->UnfixPage(bpage, hint);
}

/***************************************************************
Get page numbers of pages in memory.
Skip page 0, which is the header page.
****************************************************************/
void PageFile::PagesInMem(int &num, int *&pages) {
	_mgr->PagesInMem(_diskFile, num, pages, true, 0);
}

/***********************************************************
Read file header
************************************************************/
void PageFile::ReadHeader(void *data, int length){
	if (length >= DISKFILE_PAGESIZE - sizeof(_header)){
		fprintf(stderr,"PageFile::ReadHeader: length %d too big\n", length);
		Exit::DoExit(1);
	}

	/* read page 0 */
	BufPage *bpage = _mgr->GetAndFixPage(_diskFile,0);
	/* copy data */
	char *start = ((char *)bpage->PageData()) + sizeof(_header);
	bcopy(start, (char *)data, length);

	_mgr->UnfixPage(bpage, Current);
}

/*************************************************************
Write file header
**************************************************************/
void PageFile::WriteHeader(void *data, int length){
	if (length >= DISKFILE_PAGESIZE - sizeof(_header)){
		fprintf(stderr,"PageFile::WriteHeader: length %d too big\n", length);
		Exit::DoExit(1);
	}

	/* read page 0 */
	BufPage *bpage = _mgr->GetAndFixPage(_diskFile,0);
	/* copy data */
	char *start = ((char *)bpage->PageData()) + sizeof(_header);
	bcopy((char *)data, start, length);

	_mgr->DirtyPage(bpage);
	_mgr->UnfixPage(bpage, Current);
}

