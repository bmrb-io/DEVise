/*
  $Id$

  $Log$*/

#ifndef BuferFifo_h
#define BuferFifo_h
#include "BufferPolicy.h"

class BufferFifo: public BufferPolicy {
public:
	BufferFifo();

	virtual void Clear();

	/* Return the info about this buffer policy */
	virtual void Info(int &numArrays, int &policyFlag){
		numArrays = 1;
		policyFlag = ReportVictim | ReportPlacement;
	}

    /* Return the next victim */
	virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
		int &arrayNum,int &pos);

	/* Report information about usage */
	virtual void Usage(RangeInfoArrays *rangeArrays, int arrayNum, int pos){}

	/* A new range has been read into memory. Decide where to put it
	by setting listNum and pos*/
	virtual void Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			int &arrayNum, int &pos);

	virtual void PhaseHint(BufferPolicy::Phase phase);
	virtual void FocusHint(RecId focus, TData *tdata, GData *gdata);

private:
	BufferPolicy::Phase _phase;
	RecId _convertedLast; /* last ID to be converted */
	Boolean _hasConverted; 
};

#endif
