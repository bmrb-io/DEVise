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
  Revision 1.2  1995/09/05 22:15:45  jussi
  Added CVS header.
*/

/* Textual data virtual base class */

#ifndef TData_h
#define TData_h

#include "DeviseTypes.h"
#include "RecId.h"
#include "RecOrder.h"
#include "VisualArg.h"
#include "DList.h"

class AttrList;
const int MAX_TDATA_ATTRS = 256; /* max number of TData attributes allowed */

class TData {
public:
	virtual ~TData();

	/**** MetaData about TData ****/
	/* Return attribute info */
	virtual AttrList *GetAttrList() = 0;

	/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	virtual int Dimensions(int *sizeDimension)=0;

	/* Return record size, or -1 if variable record size */
	virtual int RecSize()=0;

	/* Return page size of TData, or -1 if no paging structure */
	virtual int PageSize()= 0;

	/* Return true if TData deletes records from the beginning
	due to limited disk/memory buffer. */
	virtual Boolean HasDeletion()=0;

	/* Return true if TData appends records */
	virtual Boolean HasAppend()= 0;

	/* Use this to get user defined attributes.
	We reserve attributes 0-SysAttrNum for internal use.
	A -1 is returned for none-existing attrNum*/
	virtual int UserAttr(int attrNum){ return -1;};

	/* Get name */
	virtual char *GetName() = 0;

	/* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices)=0;

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId)=0;

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId)=0;


	/**** Getting Records ****/

	/**************************************************************
	Init getting records.
	***************************************************************/
	virtual void InitGetRecs(RecId lowId, RecId highId,RecordOrder order)=0;

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
	virtual Boolean GetRecs(void *buf, int bufSize, RecId &startRid,
		int &numRecs, int &dataSize, void **recPtrs)=0;

	virtual void DoneGetRecs() = 0;

	/* Given buffer space and RecId, set the array "recPtrs" to
	the address of individual records. For varialbe size records. */
	virtual void GetRecPointers(RecId startId, int numRecs,
				void *buf, void **recPtrs)=0;

	
	/* For writing records. Default: not implemented. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	/* get the time file is modified. We only require that
	files modified later has time > files modified earlier. */
	virtual int GetModTime()= 0;

	/* Do a checkpoint */
	virtual void Checkpoint()=0;
};

#endif
