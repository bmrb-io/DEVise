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

#include "RangeInfo.h"

    // given a low/high closed interval, return the records in this interval.
    // limit the number of records to BMFULL_RECS_PER_BATCH.
    // set numRecs to 0 if no such record found
    // return false only if something is wrong
    // nextVal is the right adjacent value of endVal or endVal+1 if endVal is
    // 		already the largest possible value of this attr of this TData 
Boolean RangeInfo::ReturnRecs(Coord lowVal, Coord highVal, char *&buf,
			Coord &startVal, Coord &endVal, int &numRecs, 
			Coord nextVal)
    {
	if ( (lowVal > highVal) || (highVal > interval.High)
	   || (lowVal < interval.Low) ) 
	{
		numRecs = -1;
		printf("Invalid RangeInfo ReturnRecs request.\n");
		return false;
	}

	// I put the following if/else because it seems recId is not part
	// of the AttrList
	if (!strcmp(interval.AttrName, "recId"))
	{
		startVal = lowVal;
		numRecs = (int)(highVal - lowVal + 1);
		if (numRecs > BMFULL_RECS_PER_BATCH)
			numRecs = BMFULL_RECS_PER_BATCH;
		endVal = startVal + numRecs - 1;
		nextVal = endVal + 1;
		buf = data + (int)((startVal - interval.Low)*(tdata->RecSize()));

		return true;
	}
	else 
	{
	double result;

	AttrList *attrlist = tdata->GetAttrList();
	AttrInfo *attrinfo = attrlist->Find(interval.AttrName);
	AttrType attrtype = attrinfo->type;
	int length = attrinfo->length;
	int offset = attrinfo->offset;
	int recSize = tdata->RecSize();

	numRecs = 0;
	char *iterater = (char*)GetData(); 

	Boolean noMoreRecs = false; // true if iterater will begin to check 
				    // records with attr values greater 
				    // than highVal;

	while( (!noMoreRecs) && (numRecs < BMFULL_RECS_PER_BATCH) )
	{
		Boolean found = GetAttrVal(iterater, interval.AttrName, result);
		DOASSERT(found, "can not find attrVal given attrName and pointer to record.");



		if ((result >= lowVal) && (result <= highVal) )
		{
			// the current record can be returned
			numRecs++;

			if (numRecs == 1) // the first record returned
			{
				buf = iterater;
				startVal = result;
			}
		}

		if ( result >= highVal )     // >= should be used instead of >
			noMoreRecs = true;

		iterater += recSize;
		DOASSERT(( iterater < (char *)GetData() + DataSize()) || noMoreRecs,
			"end of T/GData buffer for this range but wants more");
	}

	if (numRecs > 0)
		endVal = result;

	// the following code get nextVal
	if (numRecs > 0)
	{
		if (result == interval.High)
		{
			if (HasRight())
				nextVal = RightAdjacent();
			else
				nextVal = interval.High + 1;
		}
		else
		{
			Boolean found = GetAttrVal(iterater, interval.AttrName, nextVal);
			DOASSERT(found, "can not find attrVal given attrName");
		}
	 }

	return true;
	}
    }

    // given pointer to a record and AttrName, find the value of this attribute
Boolean RangeInfo::GetAttrVal(char *record, char *attrName, double &value)
    {

	// I put the following if/else because it seems recId is not part
	// of the AttrList
	if (!(strcmp(attrName, interval.AttrName)))
	{
		value = interval.Low + (record - GetData())/(tdata->RecSize());
		return true;
	}
	else
	{
	AttrList *attrlist = tdata->GetAttrList();
        AttrInfo *attrinfo = attrlist->Find(attrName);
        AttrType attrtype = attrinfo->type;
        int length = attrinfo->length;
        int offset = attrinfo->offset;
    
	switch(attrtype)
                {
                        case IntAttr:
                             int itmp;
                             DOASSERT(sizeof(int) == length, "size conflict");
                             memcpy(&itmp, record + offset, length);
                             value = itmp;
                             break;

                        case FloatAttr:
                             float ftmp;
                             DOASSERT(sizeof(float) == length, "size conflict");
                             memcpy(&ftmp, record + offset, length);
                             value = ftmp;
                             break;

                        case DoubleAttr:
                             double dtmp;
                             DOASSERT(sizeof(double) == length,"size conflict");
                             memcpy(&dtmp, record + offset, length);
                             value = dtmp;
                             break;
			default:
                             DOASSERT(0, "unrecoganized AttrType");
                        }
	return true;
	}
    }

void RangeInfo::CheckIntegrity()
{
	if (!((interval.High - interval.Low + 1) == interval.NumRecs))
		DOASSERT(0, "RangeInfo: (High - Low + 1) != NumRecs");
}
