/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.23  1998/07/07 19:18:23  beyer
  Made large stack object into a large heap object to keep stack size small.
  A big stack is bad for threads.

  Revision 1.22  1998/06/28 21:47:42  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.21  1998/06/04 23:26:27  donjerko
  *** empty log message ***

  Revision 1.20  1998/06/04 23:06:50  donjerko
  Added DataReader.

  Revision 1.19  1998/04/09 20:26:20  donjerko
  *** empty log message ***

  Revision 1.18  1998/03/17 17:19:01  donjerko
  Added new namespace management through relation ids.

  Revision 1.17  1997/11/08 21:02:27  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.16  1997/10/02 02:27:28  donjerko
  Implementing moving aggregates.

  Revision 1.15  1997/09/17 02:35:43  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.14  1997/09/05 22:20:12  donjerko
  Made changes for port to NT.

  Revision 1.13  1997/08/15 00:17:34  donjerko
  Completed the Iterator destructor code.

  Revision 1.12  1997/08/14 02:08:53  donjerko
  Index catalog is now an independent file.

  Revision 1.11  1997/07/30 21:39:18  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.10  1997/06/21 22:48:02  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.9  1997/06/16 16:04:45  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.8  1997/06/03 16:17:45  arvind
  Removed the bug regarding strdup and strcat when naming temp files.

  Revision 1.7  1997/05/30 15:44:35  arvind
  External Sorting code using Qsort for in-memory sorting and priority queues
  for merging.

  Revision 1.6  1997/04/08 01:47:33  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.4  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <assert.h>
#include "Sort.h"

#include "StandardRead.h"
#include "Inserter.h"
#include "types.h"
#include "MemoryMgr.h"

inline void SortExec::swap(Tuple *&left, Tuple *&right) 
{
  Tuple *temp = left;
  left = right;
  right = temp;
  return;
} 

void SortExec::initialize()
{
   inpIter->initialize();
   int tuple_size = numFlds * sizeof(Type *); 
   Nruns = 0;
   strcpy(temp_filename, "sort_temp_file");   

   // Creates the runs and sets up the Q for merging the runs
   generate_runs(); 

   // Create a priority queue in memory to merge runs
   Q = new PQueue(numFlds,typeIDs,numSortFlds,sortFlds, order, Nruns);

   // Open up all the temp files
   input_buf = new Iterator*[Nruns];
   temp_files = new ifstream*[Nruns];

   char filename[20];
   char run_num[5];
   for (int i = 0; i < Nruns; i++)  // Enqueue first element of the Nruns
    {
      // open each temp file
      strcpy(filename, temp_filename); 
      sprintf(run_num, "%d", i+1);
      strcat(filename, run_num);
	 temp_files[i] = new ifstream(filename);
	 StandardRead sr;
	 sr.open(temp_files[i], numFlds, typeIDs);
      input_buf[i] = sr.createExec();

      // Read the next tuple
      const Tuple *next_tuple = NULL;
      if  (!(next_tuple = input_buf[i]->getNext()))  // Empty run??
        continue;
      
      // Create a new node to be inserted into Q
      Node* node_ptr    = new Node;
      node_ptr->tuple   = next_tuple;
      node_ptr->run_num = i;    // setup run number
      
      Q->enq(node_ptr);     
   }

   return;
}

const Tuple* SortExec::getNext()
{
  // Return the next value from the prority queue
  // Also insert into the queue the next value from the appropriate input 
  // buffer - the one corresponding to the value just dequeued.

  if(node_ptr){
    
	  // Retrieve next value from appropriate input buffer 
	  const Tuple* tuple;

	  if ((tuple = input_buf[node_ptr->run_num]->getNext()))
	   {
		 // Enqueue the retrieved tuple 

		 node_ptr->tuple = tuple;
		 Q->enq(node_ptr);
	   }

  }
  
  if (Q->num_elems() == 0) 
     return NULL; 

  // Dequeue min/max from priority queue
  node_ptr = Q->deq();
  assert(node_ptr);
  return node_ptr->tuple;
}


void SortExec::generate_runs()
{

   // Generate the runs for subsequent merging
   // Use quick sort to perform the sorting

   int count = 0;
   //kb: made table dynamic to keep stack size small
   //kb: a big stack is bad for threads!
   Tuple** table = new Tuple*[MAX_MEM]; // array of pointers to Tuple
   output_buf = new Inserter();	

   const Tuple* currTup;

   // Read in tuples and set pointers in the table 
   while((currTup = inpIter->getNext())) 
   {
     table[count] = tupleLoader->insert(currTup);
     count++;

     if (count >= MAX_MEM) //Num of tuples exceeds memory
       {
         sort_and_write_run(table, count);
         count = 0;
	    tupleLoader->reset();
       }
   }
   
   if (count) // there are tuples left to be written out
     sort_and_write_run(table, count);

   delete [] table;
}

void SortExec::sort_and_write_run(Tuple **table, int length)
{
     Nruns++;       
     qsort(table,0,length-1); 
     insert_sort(table,length);
		   
     char filename[20];
     strcpy(filename, temp_filename);
     char run_num[5];
     sprintf(run_num, "%d", Nruns);
     strcat(filename, run_num);
     unlink(filename);
     
     out_temp_file = new ofstream;   
     out_temp_file->open(filename);
     output_buf->open(out_temp_file, numFlds, typeIDs);

     for (int i = 0; i < length; i++)
       output_buf->insert(table[i]);                
   
     out_temp_file->close();
     return;
}

void SortExec::qsort(Tuple **A, int left, int right)
{
  // This sorts the array almost completely
  // A call to this function must be succeeded by 
  // a run of insertion sort to complete the sort
 
  int cutoff =1;

  if (left + cutoff < right)
    {
      Tuple *pivot = find_pivot(A, left, right);
      int i = left;
      int j = right-1;
 
      if (order == Ascending) {
	while (true)
	  {
	    while (tupleCompare(sortFlds, numSortFlds, comparePtrs,
				A[++i], pivot) < 0);  // A[++i] < pivot
	    while (tupleCompare(sortFlds, numSortFlds, comparePtrs, 
				A[--j], pivot) > 0);
	    if (i < j)     
	      swap (A[i], A[j]);
	    else
	      break;
	  }
      }
      else { 
	while (true)
	  {
	    while (tupleCompare(sortFlds, numSortFlds, comparePtrs,
				A[++i], pivot) > 0);  // pivot < A[++i]
	    while (tupleCompare(sortFlds, numSortFlds, comparePtrs, 
				A[--j], pivot) < 0);
	    if (i < j)     
	      swap (A[i], A[j]);
	    else
	      break;
	  }
      }
      swap(A[i], A[right-1]);

      qsort(A, left, i-1);
      qsort(A, i+1, right);
    }
}

Tuple* SortExec::find_pivot(Tuple **A,int left, int right)
{
  // Pivot is median of left, right and center values

  int center = (left + right)/2 ;  
  
  if (order == Ascending) {
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[center],A[left]) < 0)
      swap(A[left], A[center]);
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[right], A[left]) < 0)
      swap(A[left], A[right]);
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[right],A[center])< 0)
      swap(A[center], A[right]);
  }
  else {
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[center],A[left]) > 0)
      swap(A[left], A[center]);
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[right], A[left]) > 0)
      swap(A[left], A[right]);
    if (tupleCompare(sortFlds,numSortFlds,comparePtrs, A[right],A[center])> 0)
      swap(A[center], A[right]);
  }

  swap(A[center], A[right-1]);
  return A[right-1];
}


void SortExec::insert_sort(Tuple **A, int length)
{
  Tuple *temp;
  
  for (int i = 1; i < length; i++)
    {
      temp = A[i];
      int j;
      for (j = i; j > 0 ; j--) {
	if (order==Ascending && tupleCompare(sortFlds,numSortFlds,comparePtrs,
					     temp, A[j-1]) >= 0) {
	  break;
	}
	else if (order==Descending && tupleCompare(sortFlds,numSortFlds,comparePtrs, temp, A[j-1]) <= 0) {
	  break;
	}

	A[j] = A[j-1];
      }

      A[j] = temp;
  }

  return;
}


SortExec::SortExec(Iterator* inpIter, FieldList* sort_fields, SortOrder order)
: inpIter(inpIter), order(order)
{
  Nruns = 0;
  Q = NULL;
  numFlds = inpIter->getNumFlds();

  numSortFlds = sort_fields->size();
  sortFlds = new int[numSortFlds];
  for(int i = 0 ; i < numSortFlds ; i++) {
    sortFlds[i] = (*sort_fields)[i].getPos();
  }
  delete sort_fields;

  node_ptr = NULL;

  typeIDs = makeArray(inpIter->getTypes());

  comparePtrs  = new GeneralPtr*[numFlds];
  for (int i=0; i < numFlds; i++) {
    comparePtrs[i] = NULL;
  }
  TypeID retVal;  // is a dummy	       
  for (int i=0; i < numSortFlds; i++) {
    const TypeID& tp = typeIDs[sortFlds[i]];
    CON_TRY(comparePtrs[sortFlds[i]] = getOperatorPtr("comp", tp, tp, retVal)); 
    assert(retVal == INT_TP);
  }

  tupleLoader = new TupleLoader;
  CON_TRY(tupleLoader->open(numFlds, typeIDs));

 CON_END:;
}



SortExec::~SortExec()
{ 
  delete inpIter;
  delete [] typeIDs;
  delete tupleLoader;
  delete Q;
  
  delete [] temp_files;
	
  // individual streams of temp_files are deleted by input_buf destructors

  if(input_buf){
    for(int i = 0; i < Nruns; i++){
      delete input_buf[i];
    }
  }
  delete [] input_buf;
  delete output_buf;    // out_temp_file deleted too; 
  delete [] sortFlds;
  delete [] comparePtrs;
  delete node_ptr;
  
  char filename[20];
  for (int i =0; i < Nruns; i++){
    strcpy(filename,temp_filename);
    char run_num[5];
    sprintf(run_num, "%d", i+1);
    strcat(filename, run_num);
    // cerr << "check out " << filename << endl;
    unlink(filename);
  }
}


const TypeIDList& SortExec::getTypes()
{
  return inpIter->getTypes();
}


//---------------------------------------------------------------------------


UniqueSortExec::UniqueSortExec(Iterator* inpIter, FieldList* sort_fields,
                               SortOrder order)
: SortExec(inpIter, sort_fields, order)
{
  int i = 0;
  CON_TRY(;);	// Base class may have thrown exception

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
  } while(nextTuple && !tupleCompare(sortFlds, numSortFlds,
                                     comparePtrs, nextTuple, tuple));
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


