/*
  $Id$

  $Log$*/

#include "RangeInfoArray.h"
#include "BufferFocal.h"
#include "RangeInfo.h"
#include "TDataAttr.h"
#include "TData.h"
#include "GData.h"

inline unsigned Distance(unsigned num1, unsigned num2){
	if (num1 > num2)
		return num1-num2;
	else return num2-num1;
}

BufferFocal::BufferFocal(){
	_phase = BufferPolicy::ScanPhase;
	_hasFocus = false;
}

void BufferFocal::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
	int &arrayNum, int &pos){
	arrayNum = 0;
	pos = rangeArrays->Size(0);
}


Boolean BufferFocal::PickFifo(RangeInfoArrays *rangeArrays,
	int &arrayNum,int &pos){
	/* do FIFO */
	int i;
	for (i=0; i < rangeArrays->Size(0); i++){
		RangeInfo *rangeInfo = rangeArrays->GetRange(0,i);
		if ( !rangeInfo->InUse()){
			/* found one */
			arrayNum = 0;
			pos = i;
			/*
			printf("BufferFocal: picked Fifo victim %d\n",rangeInfo->low);
			*/
			return true;
		}
	}
	/* can't find anything as victim */
	return false;
}

Boolean BufferFocal::PickVictim(RangeInfoArrays *rangeArrays,
	int &arrayNum,int &pos){

	if (!_hasFocus){
		/* no focus. Just pick the 1st one */
		return PickFifo(rangeArrays, arrayNum, pos);
	}

	int i;
	/* find 1st valid range */
	Boolean found = false;
	int index;
	RecId furthestId;
	for (i=0; i < rangeArrays->Size(0); i++){
		RangeInfo *rangeInfo = rangeArrays->GetRange(0,i);
		if (!rangeInfo->InUse() && 
			(rangeInfo->tdata ==_focusTData || rangeInfo->tdata ==_focusGData)) {
			index = i;
			found = true; 
			furthestId = rangeInfo->low;
			break;
		}
	}
	if (!found) return PickFifo(rangeArrays, arrayNum, pos);

		
	/* continue and find the one furthest from the focus */
	for (i=index+1; i < rangeArrays->Size(0); i++){
		RangeInfo *rangeInfo = rangeArrays->GetRange(0,i);
		if (!rangeInfo->InUse() &&
			(rangeInfo->tdata== _focusTData || rangeInfo->tdata== _focusGData)&&
			Distance(rangeInfo->low, _focusId) >
			Distance(furthestId, _focusId)) {
			index = i;
			furthestId = rangeInfo->low;
		}
	}
	/*
	printf("BufferFocal: picked victim %d\n",furthestId);
	*/

	arrayNum = 0;
	pos = index;
	return true;
}

void BufferFocal::PhaseHint(BufferPolicy::Phase phase){ 
	_phase = phase; 
}

void BufferFocal::FocusHint(RecId focusId, TData *tdata, GData *gdata){
/*
printf("BufferFocal: Focus Hint %d\n",focusId);
*/
	_hasFocus = true;
	_focusId = focusId;
	_focusTData = tdata;
	_focusGData = gdata;
}

void BufferFocal::Clear(){
	_phase = BufferPolicy::ScanPhase;
	_hasFocus = false;
}
