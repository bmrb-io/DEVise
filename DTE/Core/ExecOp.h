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

#endif
