#include "Aggregates.h"
#include "MemoryMgr.h"
#include "SeqSimVecAggregate.h"


//---------------------------------------------------------------------------

AggFn::AggFn(const string& fn_name, const Field& field)
{
  _pos = field.getPos();
  const TypeID& type = field.getType();
  if( fn_name == "sum" ) {
    _fn = new ExecSum(type);
  } else if( fn_name == "count" ) {
    _fn = new ExecCount();
  } else if( fn_name == "avg" ) {
    _fn = new ExecAverage(type);
  } else if( fn_name == "min" ) {
    _fn = new ExecMinMax(type, ExecMinMax::MIN_AGG);
  } else if( fn_name == "max" ) {
    _fn = new ExecMinMax(type, ExecMinMax::MAX_AGG);
  } else if( fn_name == "seq_sum" ) {
    _fn = new ExecMovSum(type);
  } else if( fn_name == "seq_count" ) {
    _fn = new ExecMovCount();
  } else if( fn_name == "seq_avg" ) {
    _fn = new ExecMovAverage(type);
  } else if( fn_name == "seq_min" ) {
    _fn = new ExecMovMinMax(type, ExecMinMax::MIN_AGG);
  } else if( fn_name == "seq_max" ) {
    _fn = new ExecMovMinMax(type, ExecMinMax::MAX_AGG);
  } else {
    cerr << "unknown aggregate function: " << fn_name << endl;
    exit(1);
  }
}


//---------------------------------------------------------------------------


StandAggsExec::StandAggsExec(Iterator* input, AggList* aggs)
: inputIter(input), aggExecs(*aggs), types(aggs->getTypes())
{
  assert(input);
  assert(aggs);
  numFlds = aggs->size();
  retTuple = new Tuple[numFlds];
}


StandAggsExec::~StandAggsExec()
{
  delete [] retTuple;
  delete &aggExecs;
  delete inputIter;
}


void StandAggsExec::initialize()
{
  inputIter->initialize();
}

const Tuple* StandAggsExec::getNext()
{
  const Tuple* currt = inputIter->getNext();
  int i;
  if(!currt){
    return NULL;
  }
  for(i = 0; i < numFlds; i++){
    aggExecs[i]->initialize(currt);
  }
  while((currt = inputIter->getNext())){
    for(i = 0; i < numFlds; i++){
      aggExecs[i]->update(currt);
    }
  }
  
  for(i = 0; i < numFlds; i++){
    retTuple[i] = aggExecs[i]->getValue();
  }
  return retTuple;
}


const TypeIDList& StandAggsExec::getTypes()
{
  return types;
}


//---------------------------------------------------------------------------


static void makeGroupBy(AggList& aggs, const FieldList& fields)
{
  int n = fields.size();
  for(int i = 0 ; i < n ; i++) {
    const TypeID& type = fields[i].getType();
    int pos = fields[i].getPos();
    ExecGroupAttr* x = new ExecGroupAttr(type);
    aggs.push_back(AggFn(x, pos));
  }
}


static void makeSeqBy(AggList& aggs, const FieldList& fields)
{
  int n = fields.size();
  for(int i = 0 ; i < n ; i++) {
    const TypeID& type = fields[i].getType();
    int pos = fields[i].getPos();
    ExecSeqAttr* x = new ExecSeqAttr(type);
    aggs.push_back(AggFn(x, pos));
  }
}


//---------------------------------------------------------------------------


StandGroupByExec::StandGroupByExec(Iterator* _input, FieldList* group_fields,
                                   AggList* _aggs)
: inputIter(_input), aggExecs(*_aggs)
{
  assert(_input);
  assert(_aggs);
  assert(group_fields);

  makeGroupBy(groupby, *group_fields);
  delete group_fields;

  types = groupby.getTypes();
  const TypeIDList& aggtypes = aggExecs.getTypes();
  types.append(aggtypes);
  numGrpBys = groupby.size();
  numAggs = aggExecs.size();
  retTuple = new Tuple[numGrpBys+numAggs];
}


StandGroupByExec::~StandGroupByExec()
{
  delete [] retTuple;
  delete &aggExecs;
  delete inputIter;
}


void StandGroupByExec::initialize()
{
  inputIter->initialize();
  currInTup = inputIter->getNext();
}


bool StandGroupByExec::sameGroup(const Tuple* tup)
{
  for (int i = 0; i < numGrpBys ; i++) {
    if(groupby[i]->isDifferent(tup)) {
      return false;
    }
  }
  return true;
}


const Tuple* StandGroupByExec::getNext()
{
  int i;
  if(!currInTup){
    return NULL;
  }

  for(i = 0; i < numGrpBys ; i++){
    groupby[i]->initialize(currInTup);
  }
  for(i = 0; i < numAggs ; i++){
    aggExecs[i]->initialize(currInTup);
  }

  currInTup = inputIter->getNext();
  while( currInTup && sameGroup(currInTup) ) {
    for(i = 0; i < numAggs; i++){
      aggExecs[i]->update(currInTup);
    }
    currInTup = inputIter->getNext();
  }

  int k = 0;
  for(i = 0; i < numGrpBys ; i++){
    retTuple[k++] = groupby[i]->getValue();
  }
  for(i = 0; i < numAggs ; i++){
    retTuple[k++] = aggExecs[i]->getValue();
  }

  return retTuple;
}


const TypeIDList& StandGroupByExec::getTypes()
{
  return types;
}


//---------------------------------------------------------------------------


SeqAggsExec::SeqAggsExec(Iterator* _input,
                         FieldList* group_fields, FieldList* seq_fields,
                         AggList* _aggs, int winLow, int winHigh)
: inputIter(_input), aggExecs(*_aggs), windowLow(winLow), windowHigh(winHigh)
{
  assert(_input);
  assert(_aggs);
  assert(seq_fields);

  if( group_fields ) {
    makeGroupBy(groupby, *group_fields);
    delete group_fields;
  }

  makeSeqBy(seqby, *seq_fields);
  delete seq_fields;

  types = groupby.getTypes();
  types.append(seqby.getTypes());
  types.append(aggExecs.getTypes());

  numGrpBys = groupby.size();
  numSeqBys = seqby.size();
  numAggs = aggExecs.size();
  retTuple = new Tuple[numGrpBys+numSeqBys+numAggs];

  currInTup = NULL;

  fullWindowHeight = windowHigh - windowLow;
  assert (fullWindowHeight > 0);

  numTuplesToBeDropped.init(fullWindowHeight);
}


SeqAggsExec::~SeqAggsExec()
{
  delete [] retTuple;
  delete &aggExecs;
  delete inputIter;
}


bool SeqAggsExec::sameGroup(const Tuple* tup)
{
  for (int i = 0; i < numGrpBys ; i++) {
    if(groupby[i]->isDifferent(tup)) {
      return false;
    }
  }
  return true;
}


bool SeqAggsExec::isNewSeqVal(const Tuple* tup)
{
  for (int i = 0; i < numSeqBys; i++) {
    if(seqby[i]->isDifferent(tup)) {
      return true;
    }
    return false;
  }
}


void SeqAggsExec::initialize()
{
  inputIter->initialize();
  currInTup = inputIter->getNext();
  endOfGroup = true;
  currWindowHeight = 0;
}


void SeqAggsExec::setupFirst()
{
  for(int i = 0; i < numGrpBys ; i++){
    groupby[i]->initialize(currInTup);
  }
  for(int i = 0; i < numSeqBys ; i++){
    seqby[i]->initialize(currInTup);
  }
  for(int i = 0; i < numAggs ; i++){
    aggExecs[i]->initialize(currInTup);
  }
  endOfGroup = false;
  currWindowHeight = 1;
  numTuplesToBeDropped.push(1);
  currInTup = inputIter->getNext();
}


void SeqAggsExec::addData()
{
  // update aggs with currInTup and read in tuples till next value
  endOfGroup = true;
  while(currInTupIsValid()) {
    if( isNewSeqVal(currInTup) ) {
      if( currWindowHeight == fullWindowHeight ) {
        endOfGroup = false;
        break; // end of window
      }
      currWindowHeight++;
      for(int i = 0; i < numSeqBys; i++) {
        seqby[i]->update(currInTup);
      }
      numTuplesToBeDropped.push(0);
    }
    for(int i = 0; i < numAggs; i++){
      aggExecs[i]->update(currInTup);
    }
    (numTuplesToBeDropped.back())++;
    currInTup = inputIter->getNext();
  }
}


void SeqAggsExec::getResult()
{
  int k = 0;
  for(int i = 0; i < numGrpBys ; i++){
    retTuple[k++] = groupby[i]->getValue();
  }
  for(int i = 0; i < numSeqBys ; i++){
    retTuple[k++] = seqby[i]->getValue();
  }
  for(int i = 0; i < numAggs ; i++){
    retTuple[k++] = aggExecs[i]->getValue();
  }
}


void SeqAggsExec::dropData()
{
  // drop an ordinal
  currWindowHeight--;
  int toDeque = numTuplesToBeDropped.front();
  numTuplesToBeDropped.pop();
  for(int i = 0; i < numSeqBys; i++){
    seqby[i]->dequeue(1);
  }
  for(int i = 0; i < numAggs; i++){
    aggExecs[i]->dequeue(toDeque);  
  }
}


const Tuple* SeqAggsExec::getNext()
{
  if( currInTup == NULL ) return NULL; // eof
  if( !numTuplesToBeDropped.empty() ) { // drop data in cache
    dropData();
  }
  if( numTuplesToBeDropped.empty() ) { // no data (left) in cache, init
    setupFirst();
  }
  
  if( !endOfGroup ) {           // need to add data to cache
    addData();
  }

  getResult();

  return retTuple;
}


const TypeIDList& SeqAggsExec::getTypes()
{
  return types;
}



//---------------------------------------------------------------------------


bool Aggregates::isApplicable(){
	return isApplicableValue;
}

void Aggregates::typeCheck(TypeCheck& typeCheck){
	
	// Are there any aggregates in the selectClause?

	assert(!filteredSelList);

	isApplicableValue = false;

	vector<BaseSelection*>::iterator it;
	filteredSelList = new List<BaseSelection*>();

	if(!groupBy.empty()){
	  numGrpByFlds = groupBy.size();
	  grpByPos = new int[numGrpByFlds];
	}
	
	if (!sequenceBy.empty()) {
	  numSeqByFlds = sequenceBy.size();
	  seqByPos = new int[numSeqByFlds];
	}

	numAggs = numFlds - numGrpByFlds - numSeqByFlds;
	assert (numAggs >= 0);
	aggPos = new int[numAggs];

	bool hasMoving = false; //to check moving and normal aggs are not mixed
	int i = 0;
	numGrpByFlds = numSeqByFlds = numAggs = 0; // use them as counters
	for(it = selList.begin(); it != selList.end(); ++it){
		
		BaseSelection* curr = *it;
		bool resolved = false;

		if(curr->selectID() == CONSTRUCTOR_ID){
			Constructor* function = (Constructor*) curr;
			List<BaseSelection*>* args = function->getArgs();
			int numArgs = args->cardinality();
			const string* name = function->getName();
			args->rewind();
			if (numArgs == 1){
			  // simple aggregation
			  assert(!hasMoving);
			  isApplicableValue = true;
			  curr = args->get();
			  curr = typeCheck.resolve(curr);
			  args->replace(curr);
			  if (*name == "min"){
			    aggFuncs[i] = new MinAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "max"){
			    aggFuncs[i] = new MaxAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "avg"){
			    aggFuncs[i] = new AvgAggregate();
			    function->setTypeID(DOUBLE_TP);
			    resolved = true;
			  }
			  else if (*name == "sum"){
			    aggFuncs[i] = new SumAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "count"){
			    aggFuncs[i] = new CountAggregate();
			    function->setTypeID(INT_TP);
			    resolved = true;
			  }
			}
			else if (numArgs == 3) {
			  // moving aggregates
			  isApplicableValue = true;
			  hasMoving = true;			  
			  ConstantSelection *windowArg;

			  curr = args->get();
			  curr = typeCheck.resolve(curr);
			  args->replace(curr);
			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowLow = (int) windowArg->getValue();

			  args->step();
			  windowArg = (ConstantSelection *)args->get();
			  windowHigh = (int)windowArg->getValue();

			  assert(windowHigh > windowLow);

			  if (*name == "min"){
			    aggFuncs[i] = new MovMinAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "max"){
			    aggFuncs[i] = new MovMaxAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "avg"){
			    aggFuncs[i] = new MovAvgAggregate();
			    function->setTypeID(DOUBLE_TP);
			    resolved = true;
			  }
			  else if (*name == "sum"){
			    aggFuncs[i] = new MovSumAggregate();
			    function->setTypeID(curr->getTypeID());
			    resolved = true;
			  }
			  else if (*name == "count"){
			    aggFuncs[i] = new MovCountAggregate();
			    function->setTypeID(INT_TP);
			    resolved = true;
			  }
			  else if (*name == "sv")
			  {
			    aggFuncs[i] = new MovSeqSimVecAggregate(windowHigh);
			    function->setTypeID(SEQSV_TP);
			    resolved = true;
				sim_query = true;
			  }
			}
			aggPos[numAggs++] = i;
		}
		else { 
		  // must be a group by or sequence by

		  if(!groupBy.empty()){
		    vector<BaseSelection*>::iterator it;
		    for(it = groupBy.begin(); it != groupBy.end(); ++it){
		      if (curr->match(*it)){
			isApplicableValue = true;
			assert(numGrpByFlds < groupBy.size());
			grpByPos[numGrpByFlds++] = i;
			aggFuncs[i] = new GroupAttribute();
			break;
		      }
		    }
		  }

		  if (!sequenceBy.empty()) { 
		  	vector<BaseSelection*>::iterator it;
			for(it = sequenceBy.begin(); it != sequenceBy.end(); ++it){
		      if (curr->match(*it)){
			isApplicableValue = true;
			assert(numSeqByFlds < sequenceBy.size());
			seqByPos[numSeqByFlds++] = i;
			aggFuncs[i] = new SequenceAttribute();  
			break;
		      }
		    }
		  }

		}
		if(!resolved){
			curr = typeCheck.resolve(curr);
			*it = curr;
		}
		filteredSelList->append(curr);
		i++;
	}

	if (isApplicableValue) {
//	  assert( numAggs + numSeqByFlds + numGrpByFlds == numFlds);
	}
}

void Aggregates::typify(const string&, Site* inputPlanOp){
	this->inputPlanOp = inputPlanOp;
	int numFlds = inputPlanOp->getNumFlds();
	const TypeID* inptypes = inputPlanOp->getTypeIDs();
	for(int i = 0; i < numFlds; i++){
		typeIDs[i] = aggFuncs[i]->typify(inptypes[i]);
	}
}

Iterator* Aggregates::createExec(){
  assert(inputPlanOp);
  TRY(Iterator* inputIter = inputPlanOp->createExec(), NULL);
  assert(inputIter);

  AggList* aggExecs = new AggList;
  for(int i = 0; i < numAggs; i++) {
    assert(aggFuncs[aggPos[i]]);
    ExecAggregate* e = aggFuncs[aggPos[i]]->createExec();
    aggExecs->push_back(AggFn(e, aggPos[i]));
  }

  FieldList* grpBys = NULL;
  if( numGrpByFlds > 0 ) {
    grpBys = new FieldList;
    for(int i = 0; i < numGrpByFlds; i++) {
      assert(aggFuncs[grpByPos[i]]);
      grpBys->push_back( Field(typeIDs[grpByPos[i]], grpByPos[i]) );
    }
  }

  FieldList* seqBys = NULL;
  if( numSeqByFlds > 0 ) {
    seqBys = new FieldList;
    for(int i = 0; i < numSeqByFlds; i++) {
      assert(aggFuncs[seqByPos[i]]);
      seqBys->push_back( Field(typeIDs[seqByPos[i]], seqByPos[i]) );
    }
  }
  
  if (numSeqByFlds > 0) {
    // seq by with or without group by
    if (sim_query) cerr << "kb: not sure this works anymore...\n";
    return new SeqAggsExec(inputIter, grpBys, seqBys,
                           aggExecs, windowLow, windowHigh);
  }

  if(numGrpByFlds > 0){
    // just regular old group by
    return new StandGroupByExec(inputIter, grpBys, aggExecs);
  }

  // No sequence by or group by -- simple aggregate
  return new StandAggsExec(inputIter, aggExecs);
}


//---------------------------------------------------------------------------


void ExecMovMinMax::dequeue(int n){
	bool recalculate = false;
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		Type* boolVal;
		eqPtr(front[0], minMax, boolVal);
		if(boolVal){
			recalculate = true;
		}
		tupLoad->pop_front();
	}
	if(!tupLoad->empty() && recalculate){
		TupleLoader::iterator it = tupLoad->begin();
		ExecMinMax::initialize((*it)[0]);
		++it;
		while(!(it == tupLoad->end())){
			ExecMinMax::update((*it)[0]);
			++it;
		}
	}
}

void ExecMovSum::dequeue(int n){
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		subPtr(sum, front[0], sum);
		tupLoad->pop_front();
	}
}

void ExecMovAverage::dequeue(int n){
	for(int i = 0; i < n; i++){
		assert(!tupLoad->empty());
		const Tuple* front = tupLoad->front();
		subPtr(sum, front[0], sum);
		count--;
		tupLoad->pop_front();
	}
}

