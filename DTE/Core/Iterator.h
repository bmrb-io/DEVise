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

  Iterator(const DteTupleAdt& resAdt) : resultAdt(resAdt) {}
  
  // resultAdt must be set by derived Iterator constructor
  Iterator() {}

  virtual ~Iterator();

  virtual void initialize() = 0;

  virtual const Tuple* getNext() = 0;

  virtual IteratorType GetIteratorType();

  const Tuple* getFirst() { initialize(); return getNext(); }

  //virtual const DteTupleAdtRef getTypes() = 0;
  const DteTupleAdt& getAdt() { return resultAdt; }

  int getNumFields() { return getAdt().getNumFields(); }

protected:

  DteTupleAdt resultAdt;

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

  RandomAccessIterator(const DteTupleAdt& resAdt) : Iterator(resAdt) {}

  // resultAdt must be set by derived class
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

#if 0
//kb: delete this stuff
class Stats;

class PlanOp {
public:
	virtual ~PlanOp() {}
	virtual int getNumFlds() = 0;
  //virtual const DteTupleAdtRef getTypes() = 0;
  virtual const DteTupleAdt& getAdt() = 0;
  virtual const vector<string>& getAttributeNames() = 0;
  virtual const vector<string>& getOrderingAttrib(){
    assert(0);
    //return vector<string>();
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

#endif
