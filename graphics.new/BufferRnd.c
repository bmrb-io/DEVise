/* BufferRnd.g.c */
#include <stdio.h>
#include "RangeInfoArray.h"
#include "BufferRnd.h"
#include "RangeInfo.h"
#include "TDataAttr.h"


BufferRnd::BufferRnd(){
	_phase = BufferPolicy::ScanPhase;
	_hasConverted = false;
}

void BufferRnd::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
	int &arrayNum, int &pos){
	arrayNum = 0;
	pos = rangeArrays->Size(0);
}

Boolean BufferRnd::PickVictim(RangeInfoArrays *rangeArrays,
	int &arrayNum,int &pos){
	int i;
	/* do Rnd */
	int maxTimes, size;
	size = maxTimes = rangeArrays->Size(0);
	for ( ; maxTimes > 0; maxTimes--){
		int testPos = random()%  size;
		RangeInfo *rangeInfo = rangeArrays->GetRange(0,testPos);
		if ( !rangeInfo->InUse()){
			/* found one */
			arrayNum = 0;
			pos = testPos;
			return true;
		}
	}

	/* still can't find a good one. Go through array one by one to look for
	a good one */
	for (i=0; i < rangeArrays->Size(0); i++){
		RangeInfo *rangeInfo = rangeArrays->GetRange(0,i);
		if ( !rangeInfo->InUse()){
			/* found one */
			arrayNum = 0;
			pos = i;
			return true;
		}
	}
	/* can't find anything as victim */
	return false;
}

void BufferRnd::PhaseHint(BufferPolicy::Phase phase){ 
	_phase = phase; 
	_hasConverted = false;
}

void BufferRnd::FocusHint(RecId focus, TData *, GData *){}

void BufferRnd::Clear(){
	_phase = BufferPolicy::ScanPhase;
	_hasConverted = false;
}
