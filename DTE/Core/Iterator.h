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
 */

#ifndef DTE_ITERATOR_H
#define DTE_ITERATOR_H

#include <assert.h>

#include "types.h"

#ifndef __GNUG__
using namespace std;
#endif


//---------------------------------------------------------------------------


class Iterator
{
public:

  enum IteratorType {
    SIMPLE_ITERATOR,
    RANDOM_ITERATOR
  };

  Iterator() {}
  
  virtual ~Iterator();

  virtual void initialize() = 0;

  virtual const Tuple* getNext() = 0;

  virtual const TypeIDList& getTypes() = 0;

  virtual IteratorType GetIteratorType();

  const Tuple* getFirst() { initialize(); return getNext(); }

  int getNumFlds() { return getTypes().size(); }

private:

  Iterator(const Iterator& x);
  Iterator& operator=(const Iterator& x);
};


//kb: these should be in a .c file:
inline
Iterator::~Iterator()
{
}


inline
Iterator::IteratorType Iterator::GetIteratorType()
{
  return SIMPLE_ITERATOR;
}


//---------------------------------------------------------------------------


class RandomAccessIterator : public Iterator
{
public:

  RandomAccessIterator() {}

  virtual const Tuple* getThis(Offset offset) = 0;

  virtual Offset getOffset() = 0;

  virtual IteratorType GetIteratorType();

protected:

private:

  RandomAccessIterator(const RandomAccessIterator& x);
  RandomAccessIterator& operator=(const RandomAccessIterator& x);
};


inline
Iterator::IteratorType RandomAccessIterator::GetIteratorType()
{
  return RANDOM_ITERATOR;
}


//---------------------------------------------------------------------------


class Stats;

class PlanOp {
public:
	virtual ~PlanOp() {}
	virtual int getNumFlds() = 0;
	virtual const TypeID* getTypeIDs() = 0;
	virtual const string* getAttributeNames() = 0;
	virtual string *getOrderingAttrib(){
		assert(0);
		return NULL;
	}
	virtual Stats* getStats();
	virtual void open(istream* in){
		assert(!"open not implemented on some reader");
	}
	virtual ostream& display(ostream& out){
		out << "display not implemented for this reader" << endl;
		return out;
	}
	virtual Iterator* createExec() = 0;
};

#endif
