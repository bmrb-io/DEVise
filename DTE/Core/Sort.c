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
  Revision 1.7  1997/05/30 15:44:35  arvind
  External Sorting code using Qsort for in-memory sorting and priority queues
  for merging.

  Revision 1.6  1997/04/08 01:47:33  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.5  1997/04/04 23:10:25  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.4  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <assert.h>
#include "Sort.h"
  

void Sort::initialize()
{

   input->initialize();
   tuple_size = numFlds * sizeof(Type *); 
   Nruns = 0;
   temp_filename = new char[20];
   strcpy(temp_filename, "sort_temp_file");   

   // Creates the runs and sets up the Q for merging the runs
   comparePtrs  = new (GeneralPtr *)[numFlds];
   order = Ascending;
 
   for (int i=0; i < numFlds; i++)
    {
       TypeID retVal;  // is a dummy	       
       TRY(comparePtrs[i] = getOperatorPtr("comp",attrTypes[i], 
                                            attrTypes[i],retVal),);
    }
  
   generate_runs(); 

   // Create a priority queue in memory to merge runs
   Q = new PQueue(numFlds,attrTypes,numSortFlds,sortFlds, order, Nruns);
   Tuple *tuple_area = new Tuple[tuple_size*Nruns];

   // Open up all the temp files
   input_buf = new StandardRead[Nruns];
   temp_files = new ifstream[Nruns];

   char filename[20];
   char run_num[5];
   for (int i = 0; i < Nruns; i++)  // Enqueue first element of the Nruns
    {
      // open each temp file
      strcpy(filename, temp_filename); 
      sprintf(run_num, "%d", i+1);
      strcat(filename, run_num);
      temp_files[i].open(filename);
      input_buf[i].open(&temp_files[i], numFlds, attrTypes);

      // Read the next tuple
      Tuple *next_tuple = tuple_area + tuple_size*i;
      if  (!input_buf[i].getNext(next_tuple))  // Empty run??
        continue;
      
      // Create a new node to be inserted into Q
      Node* node_ptr    = new Node;
      node_ptr->tuple   = new Tuple[numFlds];
      node_ptr->run_num = i;    // setup run number
      
      memcpy(node_ptr->tuple, next_tuple, tuple_size);
      Q->enq(node_ptr);     
   }

   delete [] tuple_area;
   return;
}

bool Sort::getNext(Tuple* tuple)
{
  // Return the next value from the prority queue
  // Also insert into the queue the next value from the appropriate input 
  // buffer - the one corresponding to the value just dequeued.

  if (Q->num_elems() == 0) 
     return false; 
  
  // Dequeue min/max from priority queue
  Node* node_ptr = Q->deq();
  memcpy(tuple, node_ptr->tuple, tuple_size);  

  // Retrieve next value from appropriate input buffer 
  Tuple* new_tuple = new Tuple[numFlds];

  if (input_buf[node_ptr->run_num].getNext(new_tuple))        
   {
      // Enqueue the retrieved tuple 
      Node *new_node = new Node;
      new_node->tuple = new Tuple [numFlds];
      memcpy(new_node->tuple, new_tuple, numFlds * sizeof(Type *));  

      new_node->run_num = node_ptr->run_num;
      Q->enq(new_node);
   }

  delete [] new_tuple;
  return true;
}


void Sort::generate_runs()
{

   // Generate the runs for subsequent merging
   // Use quick sort to perform the sorting

   Tuple *table[MAX_MEM]; // array of pointers to Tuple
   int count = 0;
   int offset = 0;
   output_buf = new Inserter();	
   Tuple *tuple_area = new Tuple[numFlds*MAX_MEM]; 

   // Read in tuples and set pointers in the table 
   while(input->getNext(tuple_area+offset)) 
   {
     table[count] = tuple_area + offset;
     offset += tuple_size;
     count++;

     if (count >= MAX_MEM/(int)sizeof(Tuple)) //Number of tuples exceeds memory
       {
         sort_and_write_run(table, count);
         count = 0;
         offset = 0;
       }
   }
   
   if (count) // there are tuples left to be written out
     sort_and_write_run(table, count);

   delete []table;
   delete []tuple_area;
   return;
}

void Sort::sort_and_write_run(Tuple **table, int length)
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
     output_buf->open(out_temp_file, numFlds, attrTypes);

     for (int i = 0; i < length; i++)
       output_buf->insert(table[i]);                
   
     out_temp_file->close();
     return;
}

void Sort::qsort(Tuple **A, int left, int right)
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

      swap(A[i], A[right-1]);

      qsort(A, left, i-1);
      qsort(A, i+1, right);
    }
}

Tuple* Sort::find_pivot(Tuple **A,int left, int right)
{
  // Pivot is median of left, right and center values

  int center = (left + right)/2 ;  

  if (tupleCompare(sortFlds, numSortFlds, comparePtrs, A[center],A[left]) < 0)
    swap(A[left], A[center]);
  if (tupleCompare(sortFlds, numSortFlds, comparePtrs, A[right], A[left]) < 0)
    swap(A[left], A[right]);
  if (tupleCompare(sortFlds, numSortFlds, comparePtrs, A[right], A[center])< 0)
    swap(A[center], A[right]);

  swap(A[center], A[right-1]);
  return A[right-1];
}

inline void Sort::swap(Tuple *&left, Tuple *&right) 
{
  Tuple *temp = left;
  left = right;
  right = temp;
  return;
} 

void Sort::insert_sort(Tuple **A, int length)
{
  Tuple *temp;
  
  for (int i = 1; i < length; i++)
    {
      temp = A[i];
      int j;
      for (j = i; j > 0 && tupleCompare(sortFlds, numSortFlds, 
                               comparePtrs, temp, A[j-1]) < 0; j--)
        A[j] = A[j-1];
      A[j] = temp;
  }

  return;
}

