/* BufferFifo.c */
#include "RangeInfoArray.h"
#include "BufferFifo.h"
#include "RangeInfo.h"
#include "TDataAttr.h"


BufferFifo::BufferFifo(){
	_phase = BufferPolicy::ScanPhase;
	_hasConverted = false;
}

void BufferFifo::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
	int &arrayNum, int &pos){
	arrayNum = 0;
	pos = rangeArrays->Size(0);
}

Boolean BufferFifo::PickVictim(RangeInfoArrays *rangeArrays,
	int &arrayNum,int &pos){
	int i;
	/* do FIFO */
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

void BufferFifo::PhaseHint(BufferPolicy::Phase phase){ 
	_phase = phase; 
	_hasConverted = false;
}

void BufferFifo::FocusHint(RecId focus, TData *, GData *){}

void BufferFifo::Clear(){
	_phase = BufferPolicy::ScanPhase;
	_hasConverted = false;
}
