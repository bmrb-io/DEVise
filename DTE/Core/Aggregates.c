#include "Aggregates.h"

bool Aggregates::isApplicable(){
	
	// Is there any aggregates in the selectClause?

	selList->rewind();
	while(!selList->atEnd()){
		BaseSelection* curr = selList->get();
		if(curr->isGlobal() && curr->getPath()->isFunction()){
			String* name = curr->getPath()->getPathName();
			cout << "Detected global function: " << *name << endl;
			return true;
			List<BaseSelection*>* args = curr->getPath()->getArgs();
		}
		selList->step();
	}
	return false;
}
