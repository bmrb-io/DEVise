/* Buffer policy*/
#ifndef BufferPolicy_h
#define BufferPolicy_h
#include "DeviseTypes.h"
#include "RecId.h"
class RangeInfoArrays;
class RangeInfo;
class TData;
class GData;

/* flags for policy */
const int ReportVictim = 1;
const int ReportUsage = 0x2;
const int ReportPlacement = 0x4;


class BufferPolicy {
public:
	enum Phase { BinSearchPhase,
		ScanPhase, MemConvertPhase, DiskConvertPhase, PrefetchPhase};
	
	virtual void Clear() = 0;
	virtual void PhaseHint(Phase phase)= 0;

	virtual void FocusHint(RecId focus, TData *tdata, GData *gdata)=0; 

	/* Return the info about this buffer policy */
	virtual void Info(int &numArrays, int &policyFlag)= 0;

    /* Pick the next victim. Return false if a victim can not be found.*/
	virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
		int &arrayNum,int &pos)= 0;

	/* Report information about usage */
	virtual void Usage(RangeInfoArrays *rangeArrays, int arrayNum, int pos)= 0;

	/* A new range has been read into memory. Decide where to put it
	by setting listNum and pos*/
	virtual void Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			int &arrayNum, int &pos)= 0;

};

#endif
