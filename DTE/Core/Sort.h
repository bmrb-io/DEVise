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
  Revision 1.5  1997/04/08 01:47:34  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.4  1997/04/04 23:10:26  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef SORT_H
#define SORT_H

#include <fstream.h>

#include "site.h"
#include "Iterator.h"
#include "Inserter.h"
#include "StandardRead.h"
#include "types.h"
#include "PQueue.h"

#define MAX_MEM         800000 
#define NUM_RUNS       	1000

class Sort : public Site {
private:
  
  List<BaseSelection*>* orderBy;
  Site* input;              // The iterator
  PQueue *Q;                // Internal Priority Queue

  StandardRead *input_buf;  // Used to read in the sorted runs while merging
  ifstream  *temp_files;    // Temporary files that hold the runs
  Inserter  *output_buf;    // Output buffer in which current run is placed
  ofstream  *out_temp_file; // File connected with output_buf
  char      *temp_filename; // Name of temporary file
    
  int       Nruns;          // Number of runs
  TypeID    *attrTypes;     // Attribute types of the tuple fields
  int       tuple_size;     // Size of a tuple
   
  int       numSortFlds;    // Number of fields on which tuple is to be sorted
  int       *sortFlds;     // Index of fields to be sorted on 
  SortOrder order;          // Ascending or Descending
  GeneralPtr **comparePtrs; // Comparison operators
    
  void generate_runs();
//  void PQ1_generate_runs();
//  void PQ2_generate_runs();

  Tuple *find_pivot(Tuple **, int, int);
  inline void swap(Tuple *&, Tuple*&);
  void qsort(Tuple **, int, int);
  void insert_sort(Tuple **, int);
  void sort_and_write_run(Tuple **, int);

public:

    Sort(String nm, List<BaseSelection*>* orderBy, Site* input): /*SortOrder*/
       Site(nm), orderBy(orderBy), input(input) /*, order(order) */ 
       {}

    ~Sort()
      { 
         if (temp_files) delete [] temp_files; 
         if (out_temp_file) delete out_temp_file; 
         if (output_buf) delete output_buf; 
         if (input_buf) delete [] input_buf;
         if (Q) delete Q;
         if (attrTypes) delete attrTypes;
         if (comparePtrs) delete comparePtrs;

         char *filename;
         for (int i =0; i < Nruns; i++)
	   {
            filename= strdup(temp_filename);
            char run_num[5];
            sprintf(run_num, "%d", i+1);
            strcat(filename, run_num);
            unlink(filename);
	  }
     }    

     virtual inline int getNumFlds(){
		return numFlds;
     }
     virtual inline String *getTypeIDs(){
                return attrTypes;
     }  
     virtual inline String *getAttributeNames(){
                return input->getAttributeNames();
     }
     virtual inline String *getOrderingAttrib(){
          assert(0);
     }

     int getNumSortFlds() {
      return numSortFlds;
     }

     int *getSortFlds() {
      return sortFlds;
     }
  
     GeneralPtr **getComparePtrs()   {
      return comparePtrs;
     }
    
     virtual void initialize();         // To be called once at the start
     virtual bool getNext(Tuple* next); // returns false when no more tuples

	virtual Stats* getStats(){
		return input->getStats();
	}
	virtual void typify(String option){
		List<Site*>* tmpL = new List<Site*>;
		tmpL->append(input);
		TRY(typifyList(mySelect, tmpL), );
		numFlds = mySelect->cardinality();
		attrTypes = getTypesFromList(mySelect);
		TRY(sortFlds = findPositions(mySelect, orderBy), );
                numSortFlds = orderBy->cardinality();
		cout << "Sort fields are (index, type) = ";
		for(int i = 0; i < numSortFlds; i++){
			cout << "(" << sortFlds[i] << ", ";
			cout << attrTypes[sortFlds[i]] << ") ";
		}
		cout << endl;
	}

       virtual void enumerate(){
          assert(input);
          List<BaseSelection*>* baseSchema = input->getSelectList();
          TRY(enumerateList(mySelect, name, baseSchema), );
          input->enumerate();
      }

};

#endif
 
