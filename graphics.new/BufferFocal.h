/* Fifo Buffer policy*/
#ifndef BufferFocal_h
#define BufferFocal_h
#include "BufferPolicy.h"
class TData;
class GData;

class BufferFocal: public BufferPolicy{
public:
	BufferFocal();

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
	virtual void FocusHint(RecId focus,TData *tdata, GData *gdata);

private:
    /* Return the next victim using FIFO policy */
	virtual Boolean PickFifo(RangeInfoArrays *rangeArrays,
		int &arrayNum,int &pos);

    BufferPolicy::Phase _phase;
	Boolean _hasFocus; /* has the center of focus */
	RecId _focusId;	/* id that's the center of focus */
	TData *_focusTData;
	GData *_focusGData;
};

#endif
