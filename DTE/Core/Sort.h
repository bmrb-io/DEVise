/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.4  1997/04/04 23:10:26  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef SORT_H
#define SORT_H

#include "site.h"

class Sort : public Site {
	List<BaseSelection*>* orderBy;
	Site* input;
	int* orderByNumbers;
public:
	Sort(String nm, List<BaseSelection*>* orderBy, Site* input) :
		Site(nm), orderBy(orderBy), input(input) {
		orderByNumbers = NULL;
	}
     virtual ~Sort() {}
	/*
     virtual int getNumFlds(){
		return input->getNumFlds();
	}
     virtual String *getTypeIDs() = 0;
     virtual String *getAttributeNames() = 0;
     virtual WritePtr* getWritePtrs(){
          assert(!"getWritePtrs not implemented");
          return NULL;
     }
     virtual String *getOrderingAttrib(){
          assert(0);
     }
	*/
	virtual Stats* getStats(){
		return input->getStats();
	}
	virtual void typify(String option){
		List<Site*>* tmpL = new List<Site*>;
		tmpL->append(input);
		TRY(typifyList(mySelect, tmpL), );
		numFlds = mySelect->cardinality();
		TypeID* types = getTypesFromList(mySelect);
		TRY(orderByNumbers = findPositions(mySelect, orderBy), );
		cout << "Order by not implemented yet, (index, type) = ";
		for(int i = 0; i < orderBy->cardinality(); i++){
			cout << "(" << orderByNumbers[i] << ", ";
			cout << types[orderByNumbers[i]] << ") ";
		}
		cout << endl;
		delete [] types;
	}
     virtual void enumerate(){
          assert(input);
          List<BaseSelection*>* baseSchema = input->getSelectList();
          TRY(enumerateList(mySelect, name, baseSchema), );
          input->enumerate();
     }
	virtual void initialize();
     virtual bool getNext(Tuple* next);
};

#endif
