#ifndef EXECOP_H
#define EXECOP_H

#include "queue.h"
#include "Array.h"
#include "ExecExpr.h"
#include "Iterator.h"	 // for Iterator

class RTreeReadExec;

class SelProjExec : public Iterator {
protected:
	Iterator* inputIt;
	Array<ExecExpr*>* mySelect;
	Array<ExecExpr*>* myWhere;
	Tuple* next;
public:
	SelProjExec(Iterator* inputIt, 
		Array<ExecExpr*>* mySelect, Array<ExecExpr*>* myWhere) :
		inputIt(inputIt), mySelect(mySelect), myWhere(myWhere) {
		assert(mySelect);
		assert(myWhere);
		next = new Tuple[mySelect->length];
	}
	virtual ~SelProjExec(){
		delete inputIt;
		destroyArray(*mySelect);
		delete mySelect;
		destroyArray(*myWhere);
		delete myWhere;
		delete [] next;
	}
     virtual void initialize(){
          assert(inputIt);
          inputIt->initialize();
     }

     virtual const Tuple* getNext();
     virtual Offset getOffset(){
          assert(inputIt);
          return inputIt->getOffset();
     }
     virtual const Tuple* getThis(Offset offset, RecId recId);
};

class IndexScanExec : public Iterator {
	RTreeReadExec* index;
	Array<ExecExpr*>* mySelect;
	Array<ExecExpr*>* myWhere;
protected:
	Iterator* inputIt;
	Tuple* next;
public:
	IndexScanExec(RTreeReadExec* index, Iterator* inputIt, 
		Array<ExecExpr*>* mySelect, Array<ExecExpr*>* myWhere) :
		index(index),
		mySelect(mySelect), myWhere(myWhere), inputIt(inputIt) {
		next = new Tuple[mySelect->length];
	}
	virtual ~IndexScanExec();
     virtual void initialize();
     virtual const Tuple* getNext();
     virtual Offset getOffset(){
          assert(inputIt);
          return inputIt->getOffset();
     }
};

class NLJoinExec : public Iterator {
	Iterator* left;
	Iterator* right;
	Array<ExecExpr*>* mySelect;
	Array<ExecExpr*>* myWhere;
	Tuple* next;
     List<Tuple*> innerRel;
     bool firstEntry;
     bool firstPass;
     const Tuple* outerTup;
	int innerNumFlds;
public:
	NLJoinExec(Iterator* left, Iterator* right, 
		Array<ExecExpr*>* mySelect, Array<ExecExpr*>* myWhere,
		int innerNumFlds) :
		left(left), right(right), mySelect(mySelect), myWhere(myWhere),
		innerNumFlds(innerNumFlds) {
          firstEntry = true;
          firstPass = true;
          outerTup = NULL;
		next = new Tuple[mySelect->length];
	}
	~NLJoinExec(){
		delete left;
		delete right;
		destroyArray(*mySelect);
		delete mySelect;
		destroyArray(*myWhere);
		delete myWhere;
		delete [] next;

		assert(!"incomplete impl");
		// add more stuff here
	}
     virtual void initialize(){
          if (left){
               left->initialize();
          }
          if(right){
               right->initialize();
          }
     }
	virtual const Tuple* getNext();
};

class UnionExec : public Iterator {
	Iterator* iter1;
	Iterator* iter2;
	bool runningFirst;
public:
	UnionExec(Iterator* iter1, Iterator* iter2) :
		iter1(iter1), iter2(iter2), runningFirst(true) {}
	~UnionExec(){
		delete iter1;
		delete iter2;
	}
     virtual void initialize(){
          iter1->initialize();
     }
	virtual const Tuple* getNext();
};

class ViewEngineExec : public Iterator {
	Iterator* inp;
	int numFlds;
	Tuple* tuple;
	int counter;
public:
	ViewEngineExec(Iterator* inp, int numFlds) :
		inp(inp), numFlds(numFlds) {

		tuple = new Tuple[numFlds];
		counter = 0;
	}
	~ViewEngineExec(){
		delete inp;
		delete [] tuple;
	}
     virtual void initialize(){
          inp->initialize();
     }
	virtual const Tuple* getNext(){
		const Tuple* inputTup = inp->getNext();
		if(!inputTup){
			return NULL;
		}
		tuple[0] = (void*) counter++;
		for(int i = 1; i < numFlds; i++){
			tuple[i] = inputTup[i - 1];
		}
		return tuple;
	}
     virtual Offset getOffset(){
          assert(inp);
		// cerr << "Passed ViewEngineExec\n";
          return inp->getOffset();
     }
     virtual const Tuple* getThis(Offset offset, RecId recId){
          assert(inp);
          const Tuple* inputTup = inp->getThis(offset, recId);
		assert(inputTup);
		tuple[0] = (void*) recId;
		// cerr << "tuple[0] set to " << int(tuple[0]) << endl;
		for(int i = 1; i < numFlds; i++){
			tuple[i] = inputTup[i - 1];
		}
		return tuple;
	}
};
#endif
