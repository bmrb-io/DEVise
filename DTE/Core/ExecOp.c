#include <string>

#include "ExecOp.h"
#include "listop.h"
#include "RTreeRead.h"
#include "MemoryMgr.h"

//---------------------------------------------------------------------------
//kb: these should be put somewhere else...

static bool evalWhere(ExprList& where,
                      const Tuple* left, const Tuple* right = NULL)
{
  int n = where.size();
  for(int i = 0 ; i < n ; i++) {
    bool value = where[i]->evaluate(left, right) ? true : false;
    if( !value ) return false;
  }
  return true;
}

static void evalProject(Tuple* ret, ExprList& proj,
                        const Tuple* left, const Tuple* right = NULL)
{
  int n = proj.size();
  for(int i = 0 ; i < n ; i++){
    ret[i] = (Type*) proj[i]->evaluate(left, right);
  }
}


//---------------------------------------------------------------------------


SelProjExec::SelProjExec(Iterator* inputIt, ExprList* where,
                         ExprList* project)
: inputIt(inputIt), myWhere(*where), myProject(*project)
{
  assert(inputIt);
  assert(project);
  assert(where);
  next = new Tuple[myProject.size()];
}


SelProjExec::~SelProjExec()
{
  delete inputIt;
  delete &myWhere;
  delete &myProject;
  delete [] next;
}
  

void SelProjExec::initialize()
{
  inputIt->initialize();
}
  

const Tuple* SelProjExec::getNext()
{
  bool cond = false;
  const Tuple* input;
  assert(next);
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


const TypeIDList& SelProjExec::getTypes()
{
  return myProject.getTypes();
}


//---------------------------------------------------------------------------


TableLookupExec::TableLookupExec(Iterator* inputIt,
                                 RandomAccessIterator* file, int offset_field,
                                 ExprList* where, ExprList* project)
: inputIt(inputIt), file(file), offset_field(offset_field),
  myWhere(myWhere), myProject(myProject)
{
  assert(inputIt);
  assert(file);
  assert(where);
  assert(project);
  next = new Tuple[myProject.size()];
}


TableLookupExec::~TableLookupExec()
{
  delete inputIt;
  delete file;
  delete &myWhere;
  delete &myProject;
  delete [] next;
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
    Offset offset( IInt::getInt(input_rec[offset_field]) );
    file_rec = file->getThis(offset);
    assert(input_rec);
  } while( !evalWhere(myWhere, input_rec, file_rec) );
  evalProject(next, myProject, input_rec, file_rec);
  return next;
}


const TypeIDList& TableLookupExec::getTypes()
{
  return myProject.getTypes();
}


//---------------------------------------------------------------------------


NLJoinExec::NLJoinExec(Iterator* left, Iterator* right, 
                       ExprList* myWhere, ExprList* myProject)
: left(left), right(right), myWhere(*myWhere), myProject(*myProject),
  tupleLoader()
{
  assert(left);
  assert(right);
  assert(myWhere);
  assert(myProject);
  firstEntry = true;
  firstPass = true;
  outerTup = NULL;
  next = new Tuple[myProject->size()];
  //kb: tupleloader should take TypeIDList...
  int numFlds = right->getNumFlds();
  TypeID* t = makeArray(right->getTypes());
  tupleLoader.open(numFlds, t);
  delete [] t;
}


NLJoinExec::~NLJoinExec()
{
  delete left;
  delete right;
  delete &myProject;
  delete &myWhere;
  delete [] next;
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


const TypeIDList& NLJoinExec::getTypes()
{
  return myProject.getTypes();
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
  vec.push_back (iter1);
  vec.push_back (iter2);
}

UnionExec::UnionExec(vector<Iterator*>& _vec) : vec (_vec), runningCurrent (0)
{
  n_iter = vec.size();
}

UnionExec::~UnionExec()
{
  vector<Iterator*>::iterator i;
  for (i = vec.begin(); i != vec.end(); ++ i)
    delete (*i);
}

void UnionExec::initialize()
{
  if (n_iter > 0)
    vec[0]->initialize();
}

const Tuple* UnionExec::getNext()
{
  if (n_iter == 0)
    return NULL;
  
  const Tuple * next = vec[runningCurrent]->getNext();
  while (! next && runningCurrent < vec.size()-1)
    {
      vec[++ runningCurrent]->initialize();
      next = vec[runningCurrent]->getNext();
    }
  return next;
}
const TypeIDList& UnionExec::getTypes()
{
  assert (n_iter > 0);
  return vec[0]->getTypes ();
}


//---------------------------------------------------------------------------


RidAdderExec::RidAdderExec(Iterator* input)
: input(input)
{
  types = input->getTypes();
  types.push_front(INT_TP);
  numFlds = input->getNumFlds();
  tuple = new Tuple[numFlds+1];
}


RidAdderExec::~RidAdderExec()
{
  delete [] tuple;
  delete input;
}


void RidAdderExec::initialize()
{
  counter = 0;
  input->initialize();
}


const Tuple* RidAdderExec::getNext()
{
  const Tuple* inputTup = input->getNext();
  if(!inputTup) {
    return NULL;
  }
  tuple[0] = (void*) counter++;
  for(int i = 0; i < numFlds; i++) {
    tuple[i+1] = inputTup[i];
  }
  return tuple;
}


const TypeIDList& RidAdderExec::getTypes()
{
  return types;
}




//---------------------------------------------------------------------------


SingleAnswerIt::SingleAnswerIt(Type* arg, const TypeID& type) 
: done(false)
{
  types.push_back(type);
  destroyPtr = getDestroyPtr(type);
  retVal = arg;
}


SingleAnswerIt::~SingleAnswerIt()
{
  destroyPtr(retVal);
}


void SingleAnswerIt::initialize()
{
}


const Tuple* SingleAnswerIt::getNext()
{
  if(done) {
    return NULL;
  }
  done = true;
  return (const Tuple*) &retVal;
}


TypeIDList& SingleAnswerIt::getTypes()
{
  return types;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
