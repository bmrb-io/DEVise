#include <string>

#include "ExecOp.h"
//#include "listop.h"
#include "RTreeRead.h"
#include "exception.h"
//#include "MemoryMgr.h"
#include "DTE/comm/TupleStream.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteBoolAdt.h"
#include "DTE/types/DteStringAdt.h"

//---------------------------------------------------------------------------
//kb: these should be put somewhere else...

static bool evalWhere(ExprList& where,
                      const Tuple* left, const Tuple* right = NULL)
{
  int n = where.size();
  for(int i = 0 ; i < n ; i++) {
    const Type* b = where[i]->eval(left, right);
    if( ! DteBoolAdt::cast(b) ) return false;
  }
  return true;
}

static void evalProject(STuple& ret, ExprList& proj,
                        const Tuple* left, const Tuple* right = NULL)
{
  int n = proj.size();
  for(int i = 0 ; i < n ; i++){
    ret[i] = proj[i]->eval(left, right);
  }
}

#if 0
static void evalProject(Type const ** ret, ExprList& proj,
                        const Tuple* left, const Tuple* right = NULL)
{
  int n = proj.size();
  for(int i = 0 ; i < n ; i++){
    ret[i] = proj[i]->eval(left, right);
  }
}
#endif

static DteTupleAdt getResultAdt(ExprList& e)
{
  DteTupleAdt adt;
  int n = e.size();
  for(int i = 0 ; i < n ; i++) {
    adt.push_back(e[i]->getAdt());
  }
  return adt;
}

//---------------------------------------------------------------------------


SelProjExec::SelProjExec(Iterator* inputIt, ExprList* where,
                         ExprList* project)
: inputIt(inputIt), myWhere(*where), myProject(*project), next(myProject.size())
{
  assert(inputIt);
  assert(project);
  assert(where);
  resultAdt = getResultAdt(myProject);
}


SelProjExec::~SelProjExec()
{
  delete inputIt;
  delete_all(myWhere);
  delete_all(myProject);
  delete &myWhere;
  delete &myProject;
}
  

void SelProjExec::initialize()
{
  inputIt->initialize();
}
  

const Tuple* SelProjExec::getNext()
{
  bool cond = false;
  const Tuple* input;
  while(!cond){
    TRY(input = inputIt->getNext(), NULL);
    if(!input){
      return NULL;
    }
    cond = evalWhere(myWhere, input);
  }
  evalProject(next, myProject, input);
  return next;
}


//---------------------------------------------------------------------------


TableLookupExec::TableLookupExec(Iterator* inputIt,
                                 RandomAccessIterator* file, int offset_field,
                                 ExprList* where, ExprList* project)
: inputIt(inputIt), file(file), offset_field(offset_field),
  myWhere(*where), myProject(*project), next(project->size())
{
  assert(inputIt);
  assert(file);
  assert(where);
  assert(project);
  resultAdt = getResultAdt(myProject);
}


TableLookupExec::~TableLookupExec()
{
  delete inputIt;
  delete file;
  delete &myWhere;
  delete &myProject;
}


void TableLookupExec::initialize()
{
  inputIt->initialize();
  file->initialize();
}


const Tuple* TableLookupExec::getNext()
{
  const Tuple* file_rec;
  const Tuple* input_rec;
  do {
    input_rec = inputIt->getNext();
    if( !input_rec ) {
      return NULL;
    }
    Offset offset( DteIntAdt::cast(input_rec[offset_field]) );
    file_rec = file->getThis(offset);
    assert(input_rec);
  } while( !evalWhere(myWhere, input_rec, file_rec) );
  evalProject(next, myProject, input_rec, file_rec);
  return next;
}


//---------------------------------------------------------------------------


NLJoinExec::NLJoinExec(Iterator* left, Iterator* right, 
                       ExprList* myWhere, ExprList* myProject)
: left(left), right(right), myProject(*myProject), myWhere(*myWhere),
  next(myProject->size()), tupleLoader(right->getAdt())
{
  assert(left);
  assert(right);
  assert(myWhere);
  assert(myProject);
  resultAdt = getResultAdt(*myProject);
  firstEntry = true;
  firstPass = true;
  outerTup = NULL;
}


NLJoinExec::~NLJoinExec()
{
  delete left;
  delete right;
  delete_all(myProject);
  delete_all(myWhere);
  delete &myProject;
  delete &myWhere;
}


void NLJoinExec::initialize()
{
  left->initialize();
  right->initialize();
}


const Tuple* NLJoinExec::getNext()
{
  const Tuple* innerTup = NULL;
  if(firstEntry) {
    outerTup = left->getNext();
    firstEntry = false;
  }

  do {
    if(firstPass){
      innerTup = right->getNext();
      if( innerTup ) {
        const Tuple* tmp = tupleLoader.insert(innerTup);
        innerRel.push_back(tmp);
      } 
      else {
        firstPass = false;
        innerIter = innerRel.begin();
        if(innerIter == innerRel.end()) {
          return NULL;
        }
        innerTup = *innerIter;
        ++innerIter;
        outerTup = left->getNext();
      }
    }
    else {
      if(innerIter == innerRel.end()) {
        innerIter = innerRel.begin();
        outerTup = left->getNext();
      }
      innerTup = *innerIter;
      ++innerIter;
    }
    assert(innerTup);
    if(!outerTup) {
      return NULL;
    }
  } while( !evalWhere(myWhere, outerTup, innerTup) );

  evalProject(next, myProject, outerTup, innerTup);
  return next;
}


//---------------------------------------------------------------------------

/* *** YL

UnionExec::UnionExec(Iterator* iter1, Iterator* iter2)
: iter1(iter1), iter2(iter2), runningFirst(true)
{
}


UnionExec::~UnionExec()
{
  delete iter1;
  delete iter2;
}


void UnionExec::initialize()
{
  iter1->initialize();
}


const Tuple* UnionExec::getNext()
{
  if( runningFirst ) {
    const Tuple* next = iter1->getNext();
    if( next ) {
      return next;
    }
    // end of first
    runningFirst = false;
    iter2->initialize();
  }
  // running second
  return iter2->getNext();
}


const TypeIDList& UnionExec::getTypes()
{
  return iter1->getTypes();
}

*/

UnionExec::UnionExec(Iterator* iter1, Iterator* iter2) : runningCurrent (0), n_iter (2)
{
  resultAdt = iter1->getAdt();
  vec.push_back (iter1);
  vec.push_back (iter2);
}

UnionExec::UnionExec(vector<Iterator*>& _vec) : vec (_vec), runningCurrent (0)
{
  // assert(vec[0]); //kb: can vec be empty? no? then why all the checks?
  n_iter = vec.size();
  if( n_iter > 0 ) {
    resultAdt = vec[0]->getAdt();
  }
}

UnionExec::~UnionExec()
{
  delete_all(vec);
}

void UnionExec::initialize()
{
  if (n_iter > 0) {
    runningCurrent = 0;
    vec[0]->initialize();
  }
}

const Tuple* UnionExec::getNext()
{
  if (runningCurrent == n_iter)
    return NULL;
  
  const Tuple * next = vec[runningCurrent]->getNext();
  while (! next && runningCurrent < vec.size()-1)
    {
      vec[++ runningCurrent]->initialize();
      next = vec[runningCurrent]->getNext();
    }
  return next;
}

//---------------------------------------------------------------------------


RidAdderExec::RidAdderExec(Iterator* input)
: input(input), numFlds(input->getNumFields()), tuple(numFlds + 1)
{
  resultAdt = input->getAdt();
  resultAdt.push_front(DteIntAdt());
}


RidAdderExec::~RidAdderExec()
{
  delete input;
}


void RidAdderExec::initialize()
{
  counter = -1;
  input->initialize();
}


const Tuple* RidAdderExec::getNext()
{
  const Tuple* inputTup = input->getNext();
  if(!inputTup) {
    return NULL;
  }
  counter++;
  tuple[0] = DteIntAdt::getTypePtr(counter);
  for(int i = 0; i < numFlds; i++) {
    tuple[i+1] = inputTup[i];
  }
  return tuple;
}


//---------------------------------------------------------------------------

//kb: make this take a tuple instead of a field?
SingleAnswerIt::SingleAnswerIt(Type* arg, const DteAdt& type) 
: done(false)
{
  resultAdt.push_back(type);
  retVal = arg;
}


SingleAnswerIt::SingleAnswerIt(const string& result)
: done(false)
{
  int len = result.length();
  DteStringAdt adt(len);
  resultAdt.push_back(adt);
  retVal = adt.allocateCopy(result.c_str());
}


SingleAnswerIt::~SingleAnswerIt()
{
  resultAdt.getAdt(0).deallocate(retVal);
}


void SingleAnswerIt::initialize()
{
  done = false;
}


const Tuple* SingleAnswerIt::getNext()
{
  if(done) {
    return NULL;
  }
  done = true;
  return (const Tuple*) &retVal;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
