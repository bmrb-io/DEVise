#ifndef EXECOP_H
#define EXECOP_H

#include <vector>

#include "ExecExpr.h"
#include "Iterator.h"
#include "MemoryMgr.h"


#ifndef __GNUG__
using namespace std;
#endif

class RTreeReadExec;
class TupleLoader;


class SelProjExec : public Iterator
{
public:

  SelProjExec(Iterator* inputIt, ExprList* myWhere, ExprList* myProject);
  
  virtual ~SelProjExec();
  
  virtual void initialize();
  
  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  Iterator* inputIt;
  ExprList& myWhere;
  ExprList& myProject;
  Tuple* next;

private:

  SelProjExec(const SelProjExec& x);
  SelProjExec& operator=(const SelProjExec& x);
};


//---------------------------------------------------------------------------


class TableLookupExec : public Iterator
{
public:

  TableLookupExec(Iterator* input, RandomAccessIterator* file, int offset_field,
                  ExprList* where, ExprList* project);

  virtual ~TableLookupExec();

  virtual void initialize();

  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  Iterator* inputIt;

  RandomAccessIterator* file;

  int offset_field;

  ExprList& myWhere;

  ExprList& myProject;

  Tuple* next;

private:

  TableLookupExec(const TableLookupExec& x);
  TableLookupExec& operator=(const TableLookupExec& x);
};


//---------------------------------------------------------------------------



class NLJoinExec : public Iterator
{
public:

  NLJoinExec(Iterator* left, Iterator* right, 
             ExprList* myWhere, ExprList* myProject);

  ~NLJoinExec();

  virtual void initialize();

  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  Iterator* left;
  Iterator* right;
  ExprList& myProject;
  ExprList& myWhere;
  Tuple* next;
  vector<const Tuple*> innerRel;
  vector<const Tuple*>::const_iterator innerIter;
  bool firstEntry;
  bool firstPass;
  const Tuple* outerTup;
  int innerNumFlds;
  TupleLoader tupleLoader;

private:

  NLJoinExec(const NLJoinExec& x);
  NLJoinExec& operator=(const NLJoinExec& x);

};


//---------------------------------------------------------------------------

// Changed the structure of UnionExec  *** YL
class UnionExec : public Iterator
{
public:
  UnionExec(Iterator* iter1, Iterator* iter2);
  UnionExec(vector<Iterator*>& vec); // *** YL
  
  ~UnionExec();

  virtual void initialize();

  virtual const Tuple* getNext();
  
  virtual const TypeIDList& getTypes();

protected:

  vector<Iterator*> vec;   // *** YL
  int runningCurrent;
  int n_iter;
  
  // Iterator* iter1;      // *** YL
  // Iterator* iter2;
  // bool runningFirst;

private:

  UnionExec(const UnionExec& x);
  UnionExec& operator=(const UnionExec& x);
};


//---------------------------------------------------------------------------


class RidAdderExec : public Iterator
{
public:

  RidAdderExec(Iterator* input);

  ~RidAdderExec();

  virtual void initialize();

  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  Iterator* input;
  int numFlds;
  Tuple* tuple;
  int counter;
  TypeIDList types;

private:

  RidAdderExec(const RidAdderExec& x);
  RidAdderExec& operator=(const RidAdderExec& x);
};


//---------------------------------------------------------------------------


class SingleAnswerIt
: public Iterator
{
public:
  SingleAnswerIt(Type* arg, const TypeID& type);

  virtual ~SingleAnswerIt();

  virtual void initialize();

  virtual const Tuple* getNext();

  virtual TypeIDList& getTypes();
  
protected:

  bool done;
  DestroyPtr destroyPtr;
  Type* retVal;
  TypeIDList types;

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#endif
