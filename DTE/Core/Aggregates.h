#ifndef AGGREGATES_H
#define AGGREGATES_H

#include "types.h"
#include "myopt.h"
#include "site.h"

class Aggregates : public Site {
	List<BaseSelection*>* selList;
public:
	Aggregates(
		List<BaseSelection*>* selectClause,	// queries select clause
		List<BaseSelection*>* groupBy = NULL,	// group by clause
		BaseSelection* having = NULL			// having clause
	) : Site(), selList(selectClause) {}
	~Aggregates(){
		// do not delete selList;
	}
	bool isApplicable();
	List<BaseSelection*>* getSelectList(){
		
		// returns selectClause with aggregate operators removed

		return NULL;
	}
	void typify(Site* inputIterator){
		
		// Figure out all the types and opertions that need to be done
	}
	Tuple* getNext(){

		// execution, return NULL when done

		return NULL;

	}
};

#endif
