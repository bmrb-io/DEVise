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
  Revision 1.9  1997/06/21 22:48:03  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.8  1997/06/16 16:04:45  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.7  1997/06/03 16:17:50  arvind
  Removed the bug regarding strdup and strcat when naming temp files.

  Revision 1.6  1997/05/30 15:44:36  arvind
  External Sorting code using Qsort for in-memory sorting and priority queues
  for merging.

  Revision 1.5  1997/04/08 01:47:34  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef SORT_H
#define SORT_H

#include <fstream.h>

#include "site.h"
#include "Iterator.h"
#include "StandardRead.h"
#include "types.h"
#include "PQueue.h"

#define MAX_MEM         800000 
#define NUM_RUNS       	1000

class Inserter;

class SortExec : public Iterator {
	Iterator* inpIter;
	TupleLoader* tupleLoader;
	TypeID* typeIDs;
	SortOrder order;          // Ascending or Descending
	int       Nruns;          // Number of runs
	PQueue *Q;                // Internal Priority Queue
	Iterator**  input_buf;  // Used to read in the sorted runs while merging
	ifstream** temp_files;    // Temporary files that hold the runs
	Inserter*  output_buf;    // Output buffer in which current run is placed
	ofstream  *out_temp_file; // File connected with output_buf
	char      temp_filename[20]; // Name of temporary file to hold a run

	int       *sortFlds;     // Index of fields to be sorted on 
	int numSortFlds;    // Number of fields on which tuple is to be sorted
	int numFlds;
	GeneralPtr **comparePtrs; // Comparison operators
	Node* node_ptr;

	void generate_runs();
	Tuple *find_pivot(Tuple **, int, int);
	inline void swap(Tuple *&, Tuple*&);
	void qsort(Tuple **, int, int);
	void insert_sort(Tuple **, int);
	void sort_and_write_run(Tuple **, int);
public:
	SortExec(Iterator* inpIter, TupleLoader* tupleLoader, TypeID* typeIDs,
		SortOrder order,
		int* sortFlds, int numSortFlds, GeneralPtr **comparePtrs,
		int numFlds)
		: inpIter(inpIter), tupleLoader(tupleLoader), typeIDs(typeIDs),
		order(order), Nruns(0), Q(NULL),
		sortFlds(sortFlds), 
		numSortFlds(numSortFlds), numFlds(numFlds), 
		comparePtrs(comparePtrs), node_ptr(NULL) {}

	virtual ~SortExec();
     virtual void initialize();         // To be called once at the start
     virtual const Tuple* getNext(); // returns NULL when no more tuples
};

class Sort : public Site {
private:
  
  List<BaseSelection*>* orderBy;
  Site* input;              // The iterator

  TypeID    *attrTypes;     // Attribute types of the tuple fields
  SortOrder order;          // Ascending or Descending

public:

    Sort(String nm, List<BaseSelection*>* orderBy, Site* input): /*SortOrder*/
       Site(nm), orderBy(orderBy), input(input),
	  order(Ascending)
       {}

	~Sort(){
    		// do not delete attrTypes, passed to Exec;
	}
     virtual int getNumFlds(){
		return numFlds;
     }
     virtual const String *getTypeIDs(){
                return attrTypes;
     }  
     virtual const String *getAttributeNames(){
                return input->getAttributeNames();
     }
     virtual String *getOrderingAttrib(){
          assert(0);
     }

	virtual Stats* getStats(){
		return input->getStats();
	}
	virtual void typify(String option){
		List<Site*>* tmpL = new List<Site*>;
		tmpL->append(input);
		TRY(typifyList(mySelect, tmpL), );
		numFlds = mySelect->cardinality();
		attrTypes = getTypesFromList(mySelect);
	}

	virtual Iterator* createExec();
};

#endif
 
