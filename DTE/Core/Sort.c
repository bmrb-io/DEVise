#include "Sort.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "types.h"
#include "DTE/comm/TupleLoader.h"
#include "DTE/util/Del.h"
#include "DTE/am/HeapInserter.h" // kb: move this
#include "DTE/am/HeapRangeReader.h" // kb: move this
#include "DTE/util/SortAlgo.h"


// ascDesc = true -> ascdending; false -> descending
SortExec::SortExec(Iterator* inpIter, const vector<bool>& ascDesc, bool dedup)
: Iterator(inpIter->getAdt()), inpIter(inpIter), dedup(dedup),
  tupleEqual(getAdt(), ascDesc.size()), tupleLess(getAdt(), ascDesc),
  Q(tupleLess)  //Q()
{
  //kb: should probably check to see if all the types canCompare
}



SortExec::~SortExec()
{
  delete_all(runIters);
  close(tempFd);
  delete inpIter;
}


void SortExec::initialize()
{
  assert(runIters.size() == 0 && "Sort cannot rewind right now...");
  inpIter->initialize();

  //kb: fix this path; use environment var for path to temp space?
  //char tempFilename[100] = "/local.loon.1/sort_temp.XXXXXX";
  char tempFilename[100] = "./sort_temp.XXXXXX";
  mktemp(tempFilename);
  assert(tempFilename[0] && "error creating temp file name");
  //char tempFilename[L_tmpnam];
  //tmpnam(tempFilename);

  tempFd = ::open(tempFilename, O_RDWR|O_CREAT|O_EXCL, 0600);
  assert(tempFd >= 0 && "error opening sort temp file");
  unlink(tempFilename);

  HeapInserter* tempFile = new HeapInserter(tempFd, getAdt(), ios::trunc);

  // Creates the runs and sets up the Q for merging the runs
  generate_runs(tempFile);

  int Nruns = runStart.size();
  runStart.push_back(tempFile->getOffset()); // gets eof position
  delete tempFile;              // close temp file writer
  Q.reserve(Nruns);

  // Open up all the temp files and enqueue first element of the runs
  for (int i = 0; i < Nruns; i++) {
    HeapRangeReader* run = new HeapRangeReader(getAdt(), tempFd, 
                                               runStart[i], runStart[i+1]);
    runIters.push_back(run);    // save for deleting
    const Tuple* tup = run->getFirst();
    assert(tup && "how did an empty run get in here??");
    Q.push(QNode(tup, run));
  }
  curNode.tuple = NULL;
}

void SortExec::addNext()
{
  // Retrieve next value from appropriate run
  curNode.tuple = curNode.run->getNext();
  if( curNode.tuple ) { // If not end of run, enqueue the tuple.
    Q.push(curNode);
  }
}

const Tuple* SortExec::getNext()
{
  // Return the next value from the prority queue
  // Also insert into the queue the next value from the appropriate input 
  // buffer - the one corresponding to the value just dequeued.

  if(curNode.tuple) {
    addNext();
  }

  while( Q.size() > 0 ) {
    // Dequeue min/max from priority queue
    curNode = Q.top(); Q.pop();
    const Tuple* tup = curNode.tuple;
    if( !dedup || Q.size() == 0 || !tupleEqual(tup, Q.top().tuple) ) {
      // not doing dedup or only one run left or its is not a duplicate value
      return tup;
    }
    // duplicate value; skip it
    addNext();
  }

  // no more data
  return NULL; 
}

#include "DTE/util/Timeval.h"


// Generate the runs for subsequent merging
void SortExec::generate_runs(HeapInserter* tempFile)
{
  // made table dynamic to keep stack size small
  // a big stack is bad for threads! (kb)
  DelArray<const Tuple*> table = new const Tuple*[MAX_RUN_SIZE];
  TupleLoader tupleLoader(getAdt());

  // Read in tuples and set pointers in the table 
  //Timeval time;
  int N = 0;                    // num recs in run
  const Tuple* tup;
  while( (tup = inpIter->getNext()) != NULL ) {
    //kb: make runs based on memory, rather than num tuples
    table[N++] = tupleLoader.insert(tup);
    if( N == MAX_RUN_SIZE ) {
      //cerr << "read time: " << time.ElapsedTime() << endl;
      sort_and_write_run(table, N, tempFile);
      //time.SetToCurrentTime();
      N = 0;
      tupleLoader.rewrite();
    }
  }
  //cerr << "read time: " << time.ElapsedTime() << endl;
  //time.SetToCurrentTime();
   
  if( N > 0 ) {                 // there are tuples left to be written out
    sort_and_write_run(table, N, tempFile);
  }
}


void SortExec::sort_and_write_run(const Tuple* table[], int N,
                                  HeapInserter* tempFile)
{
  //Timeval time;
  //AttrLess<int> tupleLess;
  quickSort(table, N, tupleLess);
  //cerr << "sort time: " << time.ElapsedTime() << endl;
  runStart.push_back(tempFile->getOffset());

  //time.SetToCurrentTime();

  if( dedup ) {                 // perform duplicate elimination
    tempFile->insert(table[0]);  // length is guaranteed to be > 0
    for (int i = 1; i < N ; i++) {
      if( !tupleEqual(table[i-1], table[i]) ) { // if not dup
        tempFile->insert(table[i]); // write rec
      }
    }
  } else {                      // don't dedup
    for (int i = 0 ; i < N ; i++) {
      tempFile->insert(table[i]);
    }
  }
  //cerr << "write time: " << time.ElapsedTime() << endl;
}



//---------------------------------------------------------------------------

#if 0
UniqueSortExec::UniqueSortExec(Iterator* inpIter, FieldList* sort_fields,
                               SortOrder order)
: SortExec(inpIter, sort_fields, order)
{
  int i = 0;
  //CON_TRY(;);	// Base class may have thrown exception

  copyPtrs = new ADTCopyPtr[numFlds];
  destroyPtrs = new DestroyPtr[numFlds];
  tuple = new Type*[numFlds];
  for(i = 0; i < numFlds; i++){
    CON_TRY(copyPtrs[i] = getADTCopyPtr(typeIDs[i]));
    CON_TRY(destroyPtrs[i] = getDestroyPtr(typeIDs[i]));
    tuple[i] = allocateSpace(typeIDs[i]);
  }
  
 CON_END:;
}

UniqueSortExec::~UniqueSortExec()
{
  destroyTuple(tuple, numFlds, destroyPtrs);
}

void UniqueSortExec::initialize()
{
  SortExec::initialize();
  nextTuple = SortExec::getNext();
}

const Tuple* UniqueSortExec::getNext()
{
  if( nextTuple == NULL ) return NULL;

  copyTuple(nextTuple, tuple, numFlds, copyPtrs);

  do {
    nextTuple = SortExec::getNext();
  } while(nextTuple &&
          !tupleCompare(getAdt(), numSortFlds, sortFlds, nextTuple, tuple));

  return tuple;
}

//---------------------------------------------------------------------------

Iterator* Sort::createExec(){
	assert(input);

	TRY(Iterator* inpIter = input->createExec(), NULL);
	TRY(int* sortFlds = findPositions(mySelect, orderBy), NULL);
	int numSortFlds = orderBy->cardinality();

#if defined(DEBUG)
	cerr << "Sort fields are (index, type) = ";
	for(int i = 0; i < numSortFlds; i++){
		cerr << "(" << sortFlds[i] << ", ";
		cerr << attrTypes[sortFlds[i]] << ") ";
	}
	cerr << endl;
#endif

        FieldList* sort_fields = new FieldList;
	for(int i = 0; i < numSortFlds; i++){
          sort_fields->push_back(Field(attrTypes[sortFlds[i]], sortFlds[i]));
        }
        // SortExec used to delete these...
        delete [] attrTypes;
        delete [] sortFlds;

	TRY(Iterator* retVal = new SortExec(inpIter, sort_fields, order), 0);

	return retVal;
}


#endif
