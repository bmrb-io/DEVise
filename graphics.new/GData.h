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
  Revision 1.6  1996/11/23 21:17:56  jussi
  Removed failing support for variable-sized records.

  Revision 1.5  1996/11/12 17:19:35  jussi
  Removed unnecessary methods which are already provided in the
  base class (TData).

  Revision 1.4  1996/06/24 19:41:54  jussi
  Cleaned up and removed unused code.

  Revision 1.3  1996/03/05 23:27:52  jussi
  Minor fix.

  Revision 1.2  1995/09/05 22:14:47  jussi
  Added CVS header.
*/

/* graphical data virtual base class*/

#ifndef GData_h
#define GData_h

#include "TData.h"
#include "DList.h"

class RecFile;
class GDataRangeMap;
class GDataRangeMapRec;
class TData;

/* Callback for GData conversion*/
class GDataCallback {
public:
  virtual void Converted(RecId low, RecId high) = 0;
};

DefinePtrDList(GDataCallbackList, GDataCallback *)

class GDataRequest: public TDataRequest {
  public:
    GDataRangeMapRec *rec;
    RecId numRecs;
};

class GData: public TData {
public:
	virtual AttrList *GetAttrList();

	/* callback registration */
	void RegisterCallback(GDataCallback *c);

	/* maxBuf: max amount of space to be used to store GData */
	GData(TData *tdata, char *fname, int recSize, int maxBuf);
	virtual ~GData();

	/* Return # of dimensions and the size of each dimension,
	   or -1 if unknown */
	virtual int Dimensions(int *sizeDimension);

	int UserAttr(int attrNum);

	/* Return true if TData deletes records from the beginning
           due to limited disk/memory buffer. */
	virtual Boolean HasDeletion() { return false; }

	virtual Boolean HasAppend() { return true; }

	/* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices);

	virtual char *GetName();

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId);

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId);

	/**** Getting Records ****/

        typedef GDataRequest *GDHandle;

	/**************************************************************
	Init getting records.
	***************************************************************/
	virtual TDHandle InitGetRecs(RecId lowId, RecId highId,
                                     Boolean asyncAllowed,
                                     ReleaseMemoryCallback *callback);

	/**************************************************************
	Get next batch of records, as much as fits into buffer. 
	Return false if no more.
	input:
		buf: where to start putting records.
		bufSize: size of buffer being supplied.
	output:
		startRid : starting record ID of this batch 
		numRecs: number of records.
		dataSize: # of bytes taken up by data.
		recPtrs: pointer to records for variable size records.
	**************************************************************/
	virtual Boolean GetRecs(TDHandle handle, void *buf, int bufSize,
                                RecId &startRid, int &numRecs, int &dataSize);

	virtual void DoneGetRecs(TDHandle handle);

	virtual int GetModTime();

	/* Do a checkpoint */
	virtual void Checkpoint() {}

	/* For writing records. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	/* return number of records left to convert, or -1 if no limit */
	int RecsLeftToConvert() { return _recsLeft;}

	/* Update info about which TData records have been converted into
	   GData .
	   (tLow, tHigh): new range that has been converted.
	   firstRec: GData record for tLow
	   lastRec: GDataRec for tHigh. */
	void UpdateConversionInfo(RecId tLow, RecId tHigh,
		void *firstRec, void *lastRec);
	

	/* Get next unconverted range <= id.
	Return true if open ended (no high limit)*/
	Boolean NextUnConverted(RecId id, RecId &low, RecId &high);

	/* Return # of records left in buffer to store GData*/
	int BufRecsLeft() { return _recsLeft; }

	/* Iterator for what's converted, returned in ascending order */
	void InitConvertedIterator();
	Boolean NextRange(RecId &lowId, RecId &highId);
	void DoneConvertedIterator();

	/* Print what has been converted */
	void PrintConverted();

private:
	RecFile *_recFile;

	GDataRangeMap *_rangeMap;
	int _recsLeft; /* # of records left to convert */
	int _totalRecs; /* # of recs converted */

	GDataCallbackList _callbacks;

	TData *_tdata;
};

#endif
