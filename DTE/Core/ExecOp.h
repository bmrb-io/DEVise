#ifndef EXECOP_H
#define EXECOP_H

#include "queue.h"
#include "Iterator.h"	 // for Iterator

class RTreeReadExec;

class SelProjExec : public Iterator {
protected:
	Iterator* inputIt;
	List<BaseSelection*>* mySelect;
	List<BaseSelection*>* myWhere;
	Tuple* next;
public:
	SelProjExec(Iterator* inputIt, 
		List<BaseSelection*>* mySelect, List<BaseSelection*>* myWhere) :
		inputIt(inputIt), mySelect(mySelect), myWhere(myWhere) {
		next = new Tuple[mySelect->cardinality()];
	}
	virtual ~SelProjExec(){
		delete inputIt;	// and everythng else
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
	List<BaseSelection*>* mySelect;
	List<BaseSelection*>* myWhere;
protected:
	Iterator* inputIt;
	Tuple* next;
public:
	IndexScanExec(RTreeReadExec* index, Iterator* inputIt, 
		List<BaseSelection*>* mySelect, List<BaseSelection*>* myWhere) :
		index(index),
		mySelect(mySelect), myWhere(myWhere), inputIt(inputIt) {
		next = new Tuple[mySelect->cardinality()];
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
	List<BaseSelection*>* mySelect;
	List<BaseSelection*>* myWhere;
	Tuple* next;
     List<Tuple*> innerRel;
     bool firstEntry;
     bool firstPass;
     const Tuple* outerTup;
	int innerNumFlds;
public:
	NLJoinExec(Iterator* left, Iterator* right, 
		List<BaseSelection*>* mySelect, List<BaseSelection*>* myWhere,
		int innerNumFlds) :
		left(left), right(right), mySelect(mySelect), myWhere(myWhere),
		innerNumFlds(innerNumFlds) {
          firstEntry = true;
          firstPass = true;
          outerTup = NULL;
		next = new Tuple[mySelect->cardinality()];
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
     virtual void initialize(){
          iter1->initialize();
     }
	virtual const Tuple* getNext();
};

#endif
