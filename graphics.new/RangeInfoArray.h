/*
  $Id$

  $Log$*/

#ifndef RangeInfoArray_h
#define RangeInfoArray_h
#include "DeviseTypes.h"

class RangeInfo;
class MultiArray;
class RangeInfoArrays {
public:
	/* constructor */
	RangeInfoArrays(int numArrays, int maxSize);

	/* Clear the arrays so that contain nothing.
	DO NOT dlete the RangeInfo */
	void Clear();

	/* Return # of arrays */
	int NumArrays();

	/* size of each array */
	int Size(int arrayNum);

	/* iterator for an array */
	void InitIterator(int arrayNum);
	Boolean More(int arrayNum);
	RangeInfo *Next(int arrayNum);

	/* indexing */
	RangeInfo *GetRange(int arrayNum, int pos);

	/* moving elements */
	void Move(int fromArray, int fromPos, int toArray, int toPos);

	/* print statistics */
	void PrintStat();
protected:
	friend class BufMgrFull;
	/* insertion */
	void Insert(int arrayNum, int pos, RangeInfo *rangeInfo);

	/* delete and return the element */
	RangeInfo *Delete(int arrayNum, int pos);
private:
	MultiArray *_arrays;
	int _curBuf;		/* # of bytes for buffer */
	int _curData;		/* # of bytes in buffer containing data */
	int _maxBuf;		/* max buffer size */
	int _maxData;		/* max data */
	int _minBuf;		/* minimum buffer */
	int _minData;		/* minimum data */

	int _numInsert;		/* # of times inisert called */
	int _numDelete;		/* # of times deleted called */
	double _totalBuf;		/* for calculating average size of each 
							buffer inserted */
	double _totalData;		/* for average size of each data insert */
	double _totalBufInUse; /* for average size of buffer space in use */
	double _totalDataInUse; /* for average size of data in use */
};
#endif
