/* TDataMmap.h: memory mapped file*/
#ifndef TDataMmap_h
#define TDataMmap_h
#include "TDataAppend.h"


typedef char Page[UNIXDISKFILE_PAGESIZE];
class TData: public TData {
public:
    TDataMmap(char *name, int recSize);

	/********************************************************************
	Page Oriented Interface to get records.
	*********************************************************************/
	/* Return # of pages in the Tdata */
	virtual int NumPages();

	/* Return page number of 1st page */
	virtual int FirstPage();

	/* Return page number of last page */
	virtual int LastPage();

	/* Get page numbers of pages currently in memory */
	virtual void PagesInMem(int &numPages, int *&pageNums);

	/* Fetch page into memory and return all records
	residing in the page. numRecs == # of records in the page.
	startRid == starting record id of the records on the page.
	recs == array of pointers to records.
	isPrefetch == true if doing prefetch. Used for keeping statistics.
	Not: FreePage() must be called to free the page.
	Note: static internal data is used to keep the pointers to records
	== must be saved before same method can be called again.*/
	virtual void GetPage(int pageNum, int &numRecs, RecId &startRid,
			void **&recs, Boolean isPrefetch = false);
		
	/* Fetch the page containing the specified record id.
	isPrefetch: set to true if doing prefetch.
	Returns:
		pageNum: page number of the page
		rec: pointer to record data.
	FreePage() must be called to free the page */
	virtual void GetRecPage(RecId recId, int &pageNum, void *&rec,
		Boolean isPrefetch = false);

	/* Called when page is no longer needed.
	BufHint gives hint about which list to store the page.*/
	virtual void FreePage(int pageNum, BufHint hint= Stay);
	
	/******************************************************************
	Info about records in the file.	
	*******************************************************************/
	/* Return # of records */
	virtual int NumRecords();

	/* Return Record size */
	virtual int RecSize();

	/* Insert a record into the page */
	virtual void InsertRec(void *rec);

private:
	struct FileHeader *_header;
	long _len;
	caddr_t _addr;
	int _numPages;
	Page *_start;
};
#endif
