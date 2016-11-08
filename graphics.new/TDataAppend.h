/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.4.14.1  2005/09/06 21:20:18  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.4  2000/01/13 23:07:13  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.3  1997/03/28 16:10:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:15:48  jussi
  Added CVS header.
*/

/* Append-only T-Data */

#ifndef TDataAppend_h
#define TDataAppend_h
#include "TData.h"
#include "DiskFile.h"
#include "BufHint.h"
#include "PageSize.h"


struct FileHeader {
	int numRecs;	/* # of records in the file */
	int recSize;	/* Record size */
};
class BufMgr;
class PageFile;
class BufPage;
class TDataAppend: public TData {
public:
	
	/* Create a new T-Data. If BufMgr== NULL, a new one is
	automatically created */
	TDataAppend(char *name, BufMgr *mgr, int recSize);
	TDataAppend(char *name, int recSize);
	virtual ~TDataAppend();

	virtual const char *GetName() { return _name; }

	/********************************************************************
	Page Oriented Interface
	*********************************************************************/

	/* Return buffer manager, or NULL */
	virtual BufMgr *GetBufMgr() { return _mgr; }

	/* Return # of pages in the Tdata */
	virtual int NumPages();

	/* Return page number of 1st page */
	virtual int FirstPage();

	/* Return page number of last page */
	virtual int LastPage();

	/* Get page numbers of pages currently in memory */
	virtual void PagesInMem(int &numPages, int *&pageNums);

	/* Fetch page into memory and return all records
	isPrefetch == TRUE if doing prefetch.
	residing in the page. numRecs == # of records in the page.
	startRid == starting record id of the records.
	recs == array of pointers to records.
	Not: FreePage() must be called to free the page. */
	virtual void GetPage(int pageNum, int &numRecs, RecId &startRid,
			void **&recs, Boolean isPrefetch = false);
	
	/* return TRUE if page already in memory */
	virtual Boolean PageInMem(int pageNum);


    /* Fetch the page containing the specified record id.
	isPrefetch == TRUE if doing prefetch.
		Returns:
			pageNum: page number of the page
			rec: pointer to record data.
		FreePage() must be called to free the page */
	virtual void GetRecPage(RecId recId, int &pageNum, void *&rec, 
		Boolean isPrefetch = false);

	/* Called when page is no longer needed.*/
	virtual void FreePage(int pageNum, BufHint hint);
	
	/******************************************************************
	Record Oriented Interface
	*******************************************************************/
	/* Return # of records */
	virtual int NumRecords();

	/* Return Record size */
	virtual int RecSize();

	/* Insert a record into the page */
	virtual void InsertRec(void *rec);

private:
	/* initialize for constructors */
	void Initialize(char *name, BufMgr *mgr, int recSize);

	/* Check to make sure recId is OK */
	void CheckRecId(RecId id);

	/* # of records per page */
	int RecordsPerPage(){return DISKFILE_PAGESIZE/_header.recSize;};

	/* Get page # given record ID. Remember that page # start from 1 */
	int PageNum(RecId id){ return id/RecordsPerPage()+1; };

	/* Get slot number of record # within the page */
	int SlotNum(RecId id){ return id % RecordsPerPage(); };

	/* Get address record within the page */
	char *RecordAddress(RecId id, void *pageData){
		return (((char *)pageData)+ SlotNum(id)* _header.recSize);
	}

	/* Make a record id given page # and slot # of record within page */
	RecId MakeRecId(int page, int slot){
		return (page-1)*RecordsPerPage()+ slot;};

	FileHeader _header;
	PageFile *_pfile;	/* page file for this T-Data */
	BufMgr *_mgr;		/* buffer manager */
	Boolean _createdMgr; /* TRUE if we created the buffer manager */

	int _lastGetPageNum; /* page # of the last GetPage() or GetRecPage()
						called, or -1. */
	BufPage *_lastGetPageBuf; /* BufPage last gotten by GetPage() or
							GetRecPage().. */
	void **_returnRecs; /* buffer to store address of returned records */

	char *_name;

};
#endif
