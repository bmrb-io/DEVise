#ifndef _SRQLEXEC_H_
#define _SRQLEXEC_H_

// (C) 1998- by Kevin S. Beyer

#include "Iterator.h"

class SrqlSequenceExec : public Iterator
{
public:

  SrqlSequenceExec(Iterator* input, 
                   const vector<int>& groupPos,
                   const vector<int>& seqPos);

  ~SrqlSequenceExec();

  virtual void initialize();

  virtual const Tuple* getNext();

protected:

  Iterator* input;

  const vector<int> groupPos;
  const vector<int> seqPos;

  int groupId;
  int ordinal;

  STuple resultTuple;

  PageBuf saveBuffer;

private:

  // N/A
  SrqlSequenceExec(const SrqlSequenceExec& x);
  SrqlSequenceExec& operator=(const SrqlSequenceExec& x);
};

#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteBoolAdt.h"

SrqlSequenceExec::SrqlSequenceExec(Iterator* input,
                                   const vector<int>& groupPos,
                                   const vector<int>& seqPos)
: input(input), groupPos(groupPos), seqPos(seqPos),
  resultTuple(input->getNumFields()+2)
{
  resultAdt.push_back(DteIntAdt()); // group-id
  resultAdt.push_back(DteIntAdt()); // ordinal (i.e. seq-id)
  const DteTupleAdt& inAdt = input->getAdt();
  resultAdt.append(inAdt);
  resultTuple[0] = DteIntAdt::getTypePtr(groupId);
  resultTuple[1] = DteIntAdt::getTypePtr(ordinal);
}


SrqlSequenceExec::~SrqlSequenceExec()
{
  delete input;
}


void SrqlSequenceExec::initialize()
{
  input->initialize();
  groupId = -1;
}


const Tuple* SrqlSequenceExec::getNext()
{
 tryAgain:
  const DteTupleAdt& inAdt = input->getAdt();
  const Tuple* tup = input->getNext();
  if( tup == NULL ) return NULL;

  if( groupId == -1 ) { // first tuple
    groupId = 1;
    ordinal = 1;
  } else {
    const Tuple* saveTup = (Tuple*)saveBuffer.data;
    const int S = seqPos.size();
    for(int i = 0 ; i < S ; i++) {
      int p = seqPos[i];
      const Type* attr = tup[p];
      const DteAdt& adt = inAdt.getAdt(p);
      if( adt.isNull(attr) ) {  // drop nulls
        goto tryAgain;
      }
      if( adt.not_equal_to(saveTup[p], attr) ) {
        ordinal++;
        break;
      }
    }
    const int G = groupPos.size();
    for(int i = 0 ; i < G ; i++) {
      int p = groupPos[i];
      const DteAdt& adt = inAdt.getAdt(p);
      if( adt.not_equal_to(saveTup[p], tup[p]) ) {
        groupId++;
        ordinal = 1;
        break;
      }
    }
  }

  // save tuple for next time
  inAdt.copyNonNull(tup, saveBuffer);

  int N = inAdt.getNumFields();
  for(int i = 0 ; i < N ; i++) {
    resultTuple[i+2] = tup[i];
  }
  return resultTuple;
}


//---------------------------------------------------------------------------


class SrqlAggregateExec : public Iterator
{
public:

  SrqlAggregateExec(Iterator* left,
                    RandomAccessIterator* right,
                    ExecExpr* startExpr,
                    ExecExpr* endExpr,
                    ExecAgg* agg);

  ~SrqlAggregateExec();

  virtual void initialize();

  virtual const Tuple* getNext();

protected:

  Iterator* left;
  RandomAccessIterator* right;
  ExecExpr* startExpr;
  ExecExpr* endExpr;
  ExecAgg* agg;
  STuple resultTuple;

  int prevGroupId;
  int tupsInAgg;
  Offset groupStart;
  Offset groupEnd;

private:

  // N/A
  SrqlAggregateExec(const SrqlAggregateExec& x);
  SrqlAggregateExec& operator=(const SrqlAggregateExec& x);
};



SrqlAggregateExec::SrqlAggregateExec(Iterator* left,
                                     RandomAccessIterator* right,
                                     ExecExpr* startExpr,
                                     ExecExpr* endExpr,
                                     ExecAgg* agg)
  : left(left), right(right), startExpr(startExpr), endExpr(endExpr), agg(agg)
{
  assert(left);
  assert(right);
  assert(startExpr);
  assert(endExpr);
  assert(agg);

  assert(startExpr->getAdt().getTypeID() == DteBoolAdt::typeID);
  assert(endExpr->getAdt().getTypeID() == DteBoolAdt::typeID);

  const DteTupleAdt& inAdt = left->getAdt();
  assert(inAdt.getAdt(0).getTypeID() == DteIntAdt::typeID); // groupId
  assert(inAdt.getAdt(1).getTypeID() == DteIntAdt::typeID); // ordinal
  resultAdt = inAdt;
  resultAdt.push_back(agg->getAdt());
  resultTuple.resize(resultAdt.getNumFields());
}



SrqlAggregateExec::~SrqlAggregateExec()
{
  delete startExpr;
  delete endExpr;
  delete agg;
  delete left;
  delete right;
}


void SrqlAggregateExec::initialize()
{
  left->initialize();
  right->initialize();
  prevGroupId = -1;
  groupEnd = right->getOffset(); //kb: could save last rtup instead
}

//#define KDB(x) x
#define KDB(x)
const Tuple* SrqlAggregateExec::getNext()
{
  KDB(cerr << "getNext" << endl);
  const Tuple* ltup = left->getNext();
  if( ltup == NULL ) return NULL;
  
  const Tuple* rtup;
  int groupId = DteIntAdt::cast(ltup[0]);

  KDB(cerr << "group: " << groupId << "  ordinal: " << DteIntAdt::cast(ltup[1]) << endl);
  KDB(int r);
  if( groupId != prevGroupId ) {
    KDB(cout << "new group: " << groupId << ' ' << prevGroupId << endl);
    // find this group on right
    KDB(r = 0);
    rtup = right->getThis(groupEnd); // <-- this was in before
    //cout << "group end: " << groupEnd.offset << " right pos: " << right->getOffset().offset << endl;
    //rtup = right->getNext(); // <-- instead of this
    while( rtup && DteIntAdt::cast(rtup[0]) < groupId ) {
      KDB(r++);
      //  groupStart = right->getOffset(); // kb: delete me later
      rtup = right->getNext();
    }
    KDB(cout << "group skipped: " << r << endl);
    tupsInAgg = 0;
    prevGroupId = groupId;
  } else {
    // remove old records
    rtup = right->getThis(groupStart);
    KDB(cerr << "group start: " << groupStart.offset << " group2: " << DteIntAdt::cast(rtup[0]) << "  ordinal2: " << DteIntAdt::cast(rtup[1]) << endl);
    KDB(r = 0);
    //kb: BUG: handle nulls in predicate
    while( tupsInAgg > 0 && DteIntAdt::cast(rtup[0]) == groupId 
          && !DteBoolAdt::cast(startExpr->eval(ltup, rtup)) ) {
      KDB(r++);
      if( agg->remove(rtup) ) { // needs to scan
        tupsInAgg = 0;
      } else {
        tupsInAgg--;
	//  groupStart = right->getOffset(); // kb: delete me later
        rtup = right->getNext();
      }
    }
    KDB(cout << "removed: " << r << endl);
  }

  KDB(r = 0);
  //kb: BUG: handle nulls in predicate
  // find first record to include
  while( rtup && DteIntAdt::cast(rtup[0]) == groupId 
        && !DteBoolAdt::cast(startExpr->eval(ltup, rtup)) ) {
    KDB(r++);
    //  groupStart = right->getOffset(); // kb: delete me; is getOffset() before or after? should offset be part of output tuple??
    rtup = right->getNext();
  }
  groupStart = right->getOffset(); // kb: add me
  KDB(cout << "skipped: " << r << " group start: " << groupStart.offset << endl);

  if( tupsInAgg == 0 ) {
    // scanning from first to last
    KDB(cout << "skanning" << endl);
    agg->init();
    // groupEnd = groupStart; // kb: delete me too
  } else {			
    // find one past last record to include
    rtup = right->getThis(groupEnd);
  }
  KDB(r = 0);
  //kb: BUG: handle nulls in predicate
  while( rtup && DteIntAdt::cast(rtup[0]) == groupId 
	 && DteBoolAdt::cast(endExpr->eval(ltup, rtup)) ) {
    KDB(r++);
    agg->add(rtup);
    tupsInAgg++;
    //groupEnd = right->getOffset(); // kb: delete me too
    rtup = right->getNext();
  }
  KDB(cout << "added: " << r << endl);
  groupEnd = right->getOffset(); // kb: add me

  const int N = left->getNumFields();
  for(int i = 0 ; i < N ; i++) {
    //KDB(cerr << "copy: " << i << ' ' << ltup[i] << endl);
    resultTuple[i] = ltup[i];
  }
  //KDB(cerr << "copy: agg " << agg->value() << endl);
  resultTuple[N] = agg->getValue();
  return resultTuple;
}

#if 0
//---------------------------------------------------------------------------

#include "mql_dte_util.h"
#include "TupleStream.h"
#include "thread_page_pipe.h"


Type** allocateNullTuple(const TypeIDList types)
{
  Type** tup = allocateTuple(types);
  for(int i = 0 ; i < types.size() ; i++) {
    const TypeID& t = types[i];
    if( t == INT_TP ) {
      intCopy((Type*)-1, tup[i]);
    } else if( t == DOUBLE_TP ) {
      double d = -1.0;
      doubleCopy(&d, tup[i]);
    } else if( strncmp(types[i].c_str(), "string", 6) == 0 ) {
      //kb: Hey! need to worry about enough space in string!!
      const char* c = "null";
      size_t sz = 5;
      stringCopy(c, tup[i], sz);
    } else {
      cerr << "warning: unknown type in allocateNullTuple: " << t << endl;
    }
  }
  return tup;
}
#endif

//kb: move this & combine with one in ExecOp.c
static DteTupleAdt getResultAdt(ExprList& e)
{
  DteTupleAdt adt;
  int n = e.size();
  for(int i = 0 ; i < n ; i++) {
    adt.push_back(e[i]->getAdt());
  }
  return adt;
}


class DteJoinStyle
{
public:

  enum JoinStyle {
    INNER       = 0x00,
    LEFT_OUTER  = 0x01,
    RIGHT_OUTER = 0x02,
    FULL_OUTER  = 0x03,
  };

  DteJoinStyle() { joinStyle = INNER; }

  DteJoinStyle(JoinStyle js) { joinStyle = js; }

  DteJoinStyle(int js) { 
    assert(js >= INNER && js <=FULL_OUTER);
    joinStyle = (JoinStyle)js;
  }

  bool isLeftOuter() const { return joinStyle & LEFT_OUTER; }
  
  bool isRightOuter() const { return joinStyle & RIGHT_OUTER; }

  //~DteJoinStyle();

  // defaults
  //DteJoinStyle(const DteJoinStyle& x);
  //DteJoinStyle& operator=(const DteJoinStyle& x);

protected:

  JoinStyle joinStyle;
};



#if 0
class MergeJoinProc
: public Dte_thread_t
{
public:


  MergeJoinProc(int pipePages,
                Iterator* left, RandomAccessIterator* right,
                ExprList* compareExpr, ExprList* project,
                DteJoinStyle joinStyle );

  ~MergeJoinProc();

  virtual void run();

  thread_page_pipe_t* getOutputPipe() { return outputPipe; }

protected:

  Iterator* left;

  RandomAccessIterator* right;

  thread_page_pipe_t* outputPipe;

  ExprList& compareExpr;

  ExprList& project;

  DteTupleDesc tupleDesc;

  DteJoinStyle joinStyle;

  Type** nullLeft;

  Type** nullRight;

  void output(OTupleStream& ts, const Tuple* l, const Tuple* r);

  int compare(const Tuple* ltup, const Tuple* rtup);

private:

  // N/A
  MergeJoinProc(const MergeJoinProc& x);
  MergeJoinProc& operator=(const MergeJoinProc& x);
};


MergeJoinProc::MergeJoinProc(int pipePages,
                             Iterator* left, RandomAccessIterator* right,
                             ExprList* compareExpr, ExprList* project,
                             DteJoinStyle joinStyle )
: compareExpr(*compareExpr), project(*project)
{
  this->left = left;
  this->right = right;
  tupleDesc = convert_to_Dte_types(project->getTypes());
  outputPipe = new thread_page_pipe_t(tupleDesc, pipePages);
  this->joinStyle = joinStyle;

  //kb: need nulls for outer joins...
  nullLeft = allocateNullTuple(left->getTypes());
  nullRight = allocateNullTuple(right->getTypes());
}



MergeJoinProc::~MergeJoinProc()
{
  delete outputPipe;
  deleteTuple(nullLeft, left->getTypes());
  deleteTuple(nullRight, right->getTypes());
  delete left;
  delete right;
  delete &compareExpr;
  delete &project;
}


void MergeJoinProc::output(OTupleStream& ts, const Tuple* l, const Tuple* r)
{
  int n = project.size();
  for(int i = 0 ; i < n ; i++) {
    uint type = tupleDesc.GetAttrType(i);
    const Type* attr = project[i]->eval(l, r);
    switch( type ) {
    case Dte::INT4_TYPE:
      ts.WriteInt4((int4)attr);
      break;
    case Dte::BOOL_TYPE:
      ts.WriteBool((int4)attr);
      break;
    case Dte::REAL8_TYPE:
      ts.WriteReal8(*(real8*)attr);
      break;
    case Dte::STRING_TYPE: {
      char* cp = (char*)attr;
      ts.WriteString(cp, strlen(cp)+1);
      break;
    }
    case Dte::VARSTRING_TYPE: {
      char* cp = (char*)attr;
      ts.WriteVarString(cp, strlen(cp)+1);
      break;
    }
    default:
      assert(!"unknown type");
    }
  }
  ts.EndTuple();
}


int MergeJoinProc::compare(const Tuple* ltup, const Tuple* rtup)
{
  int n = compareExpr.size();
  for(int i = 0 ; i < n ; i++) {
    int c = DteBoolAdt::cast(compareExpr[i]->eval(ltup, rtup));
    if( c ) return c;
  }
  return 0;
}


void MergeJoinProc::run()
{
  thread_page_pipe_writer_t page_stream(*outputPipe);
  OTupleStream ts(tupleDesc, page_stream);

  left->initialize();
  right->initialize();

  const Tuple* ltup = left->getNext();
  const Tuple* rtup = right->getNext();

  while( ltup && rtup ) {
    int cmp = compare(ltup, rtup);
    if( cmp < 0 ) {
      if( joinStyle.isLeftOuter() ) {
        output(ts, ltup, nullRight);
      }
      ltup = left->getNext();
    } else if( cmp > 0 ) {
      if( joinStyle.isRightOuter() ) {
        output(ts, nullLeft, rtup);
      }
      rtup = right->getNext();
    } else {
      Offset save = right->getOffset();
      Offset skip;
      do {
        do {
          output(ts, ltup, rtup);
          rtup = right->getNext();
        } while( rtup && compare(ltup, rtup) == 0 );
        ltup = left->getNext();
        skip = right->getOffset();
        rtup = right->getThis(save);
      } while( ltup && compare(ltup, rtup) == 0 );
      rtup = right->getThis(skip);
    }
  }
  if( joinStyle.isLeftOuter() ) {
    while( ltup ) {
      output(ts, ltup, nullRight);
      ltup = left->getNext();
    }
  }
  if( joinStyle.isRightOuter() ) {
    while( rtup ) {
      output(ts, nullLeft, rtup);
      rtup = right->getNext();
    }
  }
}
#endif

//---------------------------------------------------------------------------


class MergeJoinDedupRightExec
: public Iterator
{
public:

  MergeJoinDedupRightExec(Iterator* left,
                          Iterator* right,
                          ExprList* startExpr,
                          ExprList* endExpr,
                          ExprList* project,
                          DteJoinStyle joinStyle);

  ~MergeJoinDedupRightExec();

  virtual void initialize();

  virtual const Tuple* getNext();

protected:

  Iterator* left;

  Iterator* right;

  ExprList startExpr;

  ExprList endExpr;

  ExprList project;

  DteJoinStyle joinStyle;

  const Tuple* rtup;

  STuple resultTuple;

  STuple nullRight;

  //int compare(const Tuple* ltup, const Tuple* rtup);
  bool less(const Tuple* ltup, const Tuple* rtup);
  bool greater(const Tuple* ltup, const Tuple* rtup);

private:

  // N/A
  MergeJoinDedupRightExec(const MergeJoinDedupRightExec& x);
  MergeJoinDedupRightExec& operator=(const MergeJoinDedupRightExec& x);
};


MergeJoinDedupRightExec::MergeJoinDedupRightExec(Iterator* left,
                                                 Iterator* right,
                                                 ExprList* startExpr,
                                                 ExprList* endExpr,
                                                 ExprList* project,
                                                 DteJoinStyle joinStyle)
  : Iterator(getResultAdt(*project)),
    left(left), right(right), startExpr(*startExpr), endExpr(*endExpr),
    project(*project), joinStyle(joinStyle), resultTuple(project->size()),
    nullRight(right->getNumFields())
{
  //kb: BUG: need to set nullable for outer joins!!
  delete startExpr;
  delete endExpr;
  delete project;
  assert( !joinStyle.isRightOuter() );
}



MergeJoinDedupRightExec::~MergeJoinDedupRightExec()
{
  delete_all(startExpr);
  delete_all(endExpr);
  delete_all(project);
  delete left;
  delete right;
}

/*
int MergeJoinDedupRightExec::compare(const Tuple* ltup, const Tuple* rtup)
{
  int n = startExpr.size();
  for(int i = 0 ; i < n ; i++) {
    //    if( DteBoolAdt::cast(lessExpr[i]->eval(ltup, rtup)) ) return -1;
    //    if( DteBoolAdt::cast(greaterExpr[i]->eval(ltup, rtup)) ) return 1;
    if( ! DteBoolAdt::cast(startExpr[i]->eval(ltup, rtup)) ) return -1;
    if( ! DteBoolAdt::cast(endExpr[i]->eval(ltup, rtup)) ) return +1;
  }
  return 0;
}
*/


bool MergeJoinDedupRightExec::less(const Tuple* ltup, const Tuple* rtup)
{
  int n = startExpr.size();
  for(int i = 0 ; i < n ; i++) {
    //kb: BUG: handle nulls in predicate
    if( ! DteBoolAdt::cast(startExpr[i]->eval(ltup, rtup)) ) return true;
  }
  return false;
}


bool MergeJoinDedupRightExec::greater(const Tuple* ltup, const Tuple* rtup)
{
  int n = endExpr.size();
  for(int i = 0 ; i < n ; i++) {
    //kb: BUG: handle nulls in predicate
    if( ! DteBoolAdt::cast(endExpr[i]->eval(ltup, rtup)) ) return true;
  }
  return false;
}


void MergeJoinDedupRightExec::initialize()
{
  left->initialize();
  right->initialize();
  rtup = right->getNext();
}


const Tuple* MergeJoinDedupRightExec::getNext()
{
  do {
    const Tuple* ltup = left->getNext();
    if( ltup == NULL ) {
      return NULL;
    }
    
    // find next right tuple
    while( rtup && greater(ltup, rtup) ) {
      rtup = right->getNext();
    }
    if( rtup && !less(ltup, rtup) ) { // match!
      int n = project.size();
      for(int i = 0 ; i < n ; i++) {
        resultTuple[i] = project[i]->eval(ltup, rtup);
      }
      return resultTuple;
    }
    
    // no right match, if this is a left outer join, return (ltup,null)
    if( joinStyle.isLeftOuter() ) {
      int n = project.size();
      for(int i = 0 ; i < n ; i++) {
        resultTuple[i] = project[i]->eval(ltup, nullRight);
      }
      return resultTuple;
    }

    // not left outer join, so try again with the next ltup
  } while( rtup );

  // no more rtup's, and it's not a left outer join, so we're done
  // even though there are more ltup's
  return NULL;
}


//---------------------------------------------------------------------------


class MergeJoinExec
: public Iterator
{
public:

  MergeJoinExec(Iterator* left,
                RandomAccessIterator* right,
                ExprList* startExpr,
                ExprList* endExpr,
                ExprList* project,
                DteJoinStyle joinStyle);

  ~MergeJoinExec();

  virtual void initialize();

  virtual const Tuple* getNext();

protected:

  Iterator* left;

  RandomAccessIterator* right;

  ExprList startExpr;

  ExprList endExpr;

  ExprList project;

  DteJoinStyle joinStyle;

  bool foundMatch;

  bool rightAlreadyMatched;

  bool nextLeft;

  const Tuple* ltup;

  const Tuple* rtup;

  STuple resultTuple;

  STuple nullLeft;

  STuple nullRight;

  Offset dupStart;

  Offset dupEnd;

  void evalProject(const Tuple* ltup, const Tuple* rtup);
  bool less(const Tuple* ltup, const Tuple* rtup);
  bool greater(const Tuple* ltup, const Tuple* rtup);

private:

  // N/A
  MergeJoinExec(const MergeJoinExec& x);
  MergeJoinExec& operator=(const MergeJoinExec& x);
};


MergeJoinExec::MergeJoinExec(Iterator* left,
                             RandomAccessIterator* right,
                             ExprList* startExpr,
                             ExprList* endExpr,
                             ExprList* project,
                             DteJoinStyle joinStyle)
  : left(left), right(right), startExpr(*startExpr), endExpr(*endExpr),
    project(*project), joinStyle(joinStyle), resultTuple(project->size()),
    nullLeft(left->getNumFields()), nullRight(right->getNumFields())
{
//    if( joinStyle.isLeftOuter() ) {
//    }
//    if( joinStyle.isRightOuter() ) {
//    }
  //kb: BUG: need to set nullable for outer joins!!
  resultAdt = getResultAdt(*project);
  delete startExpr;
  delete endExpr;
  delete project;
}



MergeJoinExec::~MergeJoinExec()
{
  delete_all(startExpr);
  delete_all(endExpr);
  delete_all(project);
  delete left;
  delete right;
}

bool MergeJoinExec::less(const Tuple* ltup, const Tuple* rtup)
{
  int n = startExpr.size();
  for(int i = 0 ; i < n ; i++) {
    const Type* pred = startExpr[i]->eval(ltup, rtup);
    if( pred && ! DteBoolAdt::cast(pred) ) return true;
  }
  return false;
}


bool MergeJoinExec::greater(const Tuple* ltup, const Tuple* rtup)
{
  int n = endExpr.size();
  for(int i = 0 ; i < n ; i++) {
    const Type* pred = endExpr[i]->eval(ltup, rtup);
    if( pred && ! DteBoolAdt::cast(pred) ) return true;
  }
  return false;
}


void MergeJoinExec::initialize()
{
  left->initialize();
  right->initialize();
  foundMatch = false;
  rightAlreadyMatched = false;
  nextLeft = true;
  rtup = right->getNext();
}


void MergeJoinExec::evalProject(const Tuple* ltup, const Tuple* rtup)
{
  int n = project.size();
  for(int i = 0 ; i < n ; i++) {
    resultTuple[i] = project[i]->eval(ltup, rtup);
  }
}

const Tuple* MergeJoinExec::getNext()
{
  if( nextLeft ) {
    ltup = left->getNext();
  } else {
    rtup = right->getNext();
  }

  while( ltup ) {

    if( right->getOffset().offset == dupEnd.offset ) {
      //cout << "clearing found " << dupEnd.offset << endl;
      rightAlreadyMatched = false;
    }

    if(!rtup || less(ltup, rtup) ) {

      if( foundMatch ) {        // rewind right for dups
        dupEnd = right->getOffset();
        //cout << "rewinding to " << dupStart.offset << " from " << dupEnd.offset << endl;
        ltup = left->getNext();
        rtup = right->getThis(dupStart);
        foundMatch = false;
        rightAlreadyMatched = true;
      } else {                  // all dups considered
        if( joinStyle.isLeftOuter() ) {
          nextLeft = true;
          evalProject(ltup, nullRight);
          return resultTuple;
        }
        ltup = left->getNext();
      }

    } else if( greater(ltup, rtup) ) {

      if(joinStyle.isRightOuter() && !rightAlreadyMatched) {
        nextLeft = false;
        evalProject(nullLeft, rtup);
        return resultTuple;
      }
      rtup = right->getNext();

    } else { // match

      if( !foundMatch ) {
        foundMatch = true;
        dupStart = right->getOffset();
        //cout << "saving start " << dupStart.offset << endl;
      }
      nextLeft = false;
      evalProject(ltup, rtup);
      return resultTuple;

    }
  }

  if( rightAlreadyMatched ) {
    rightAlreadyMatched = false;
    rtup = right->getThis(dupEnd);
  }

  if( rtup && joinStyle.isRightOuter() ) {
    nextLeft = false;
    evalProject(nullLeft, rtup);
    return resultTuple;
  }

  return NULL;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


#endif // _SRQLEXEC_H_
