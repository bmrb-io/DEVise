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
 */

#include "Aggregates.h"

bool Aggregates::isApplicable(){
	
	// Is there any aggregates in the selectClause?

	if(selList == NULL){
		return false;
	}
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
