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
  Revision 1.23  1999/01/20 22:46:25  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.22  1998/11/06 17:27:08  beyer
  Eliminated getFirst from derived Iterators.

  Revision 1.21  1998/06/28 21:47:42  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.20  1998/06/04 23:06:50  donjerko
  Added DataReader.

  Revision 1.19  1998/04/09 20:26:21  donjerko
  *** empty log message ***

  Revision 1.18  1998/03/17 17:19:02  donjerko
  Added new namespace management through relation ids.

  Revision 1.17  1997/11/24 06:01:25  donjerko
  Added more odbc files.

  Revision 1.16  1997/11/08 21:02:28  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.15  1997/10/14 05:16:40  arvind
  Implemented a first version of moving aggregates (without group bys).

  Revision 1.14  1997/10/02 02:27:29  donjerko
  Implementing moving aggregates.

  Revision 1.13  1997/09/05 22:20:12  donjerko
  Made changes for port to NT.

  Revision 1.12  1997/08/21 21:04:26  donjerko
  Implemented view materialization

  Revision 1.11  1997/08/14 02:08:53  donjerko
  Index catalog is now an independent file.

  Revision 1.10  1997/07/30 21:39:19  donjerko
  Separated execution part from typchecking in expressions.

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

//#include <fstream.h>   erased for sysdep.h

#define _SYS_STREAM_H //kb: hack: why is sys/stream.h included?
#include "Iterator.h"
#include "types.h"
#include "sysdep.h"
#include "TupleCompare.h"       //kb: move this
#include <vector>
#include "DTE/util/PriorityQueue.h"


#ifndef __GNUG__
using namespace std;
#endif

class HeapRangeReader;
class HeapInserter;

template <class T>
struct AttrLess : public binary_function<T, T, bool> {
  bool operator()(const Tuple* x, const Tuple* y) const {
    return *(T*)x[0] < *(T*)y[0];
  }
};



class SortExec : public Iterator
{
  struct QNode {
    QNode() : tuple(0), run(0) {}
    QNode(const Tuple* t, HeapRangeReader* r) : tuple(t), run(r) {}
    const Tuple* tuple;
    HeapRangeReader* run;
    operator const Tuple*() const { return tuple; } //kb: hack to make TupleLess work
  };

  static const int MAX_RUN_SIZE = 100000; // number of tuples in each run

  Iterator* inpIter;

  bool dedup;

  TupleEqual tupleEqual;
  TupleLess tupleLess;

  int tempFd;                   // file descriptor of temporary file

  vector<Offset> runStart;
  vector<HeapRangeReader*> runIters;

  PriorityQueue<QNode, TupleLess> Q; // Internal Priority Queue
  //PriorityQueue<QNode, AttrLess<int> > Q; // Internal Priority Queue
  QNode curNode;                // last node taken from Q
  
  void addNext();

  void generate_runs(HeapInserter* tempFile);

  void sort_and_write_run(const Tuple* table[], int N,
                          HeapInserter* tempFile);

public:

  SortExec(Iterator* inpIter, const vector<bool>& ascDesc, bool dedup);

  virtual ~SortExec();

  virtual void initialize();         // To be called once at the start

  virtual const Tuple* getNext(); // returns NULL when no more tuples
};


//---------------------------------------------------------------------------

#if 0
//kb: class not needed any more: use SortExec(..., true);
class UniqueSortExec : public SortExec {
public:
  UniqueSortExec(Iterator* inpIter, FieldList* sort_fields, SortOrder order);
  virtual ~UniqueSortExec();
  virtual void initialize();
  virtual const Tuple* getNext(); // returns NULL when no more tuples
protected:
  Type** tuple;
  const Tuple* nextTuple;
};
#endif

//---------------------------------------------------------------------------

#if 0
class Sort : public Site {
private:
  
  List<BaseSelection*>* orderBy;
  Site* input;              // The iterator

  TypeID    *attrTypes;     // Attribute types of the tuple fields
  SortOrder order;          // Asc or Desc (enum SortOrder def in PQueue.h)
  int numFlds;

public:

    Sort(string nm, List<BaseSelection*>* orderBy, Site* input, 
	 string* sortOrder): 
      Site(nm), orderBy(orderBy), input(input)
       {
	 if ((!sortOrder) || (*sortOrder == "ascending")) {
	   order = Ascending;
	 }
	 else { 
	   order = Descending;
	 }
       }

	~Sort(){
    		// do not delete attrTypes, passed to Exec;
	}
     virtual int getNumFields(){
		return numFlds;
     }
     virtual const string *getTypeIDs(){
                return attrTypes;
     }  
     virtual const string *getAttributeNames(){
                return input->getAttributeNames();
     }
     virtual string *getOrderingAttrib(){
          assert(0);
		return NULL;
     }

	virtual Stats* getStats(){
		return input->getStats();
	}
	virtual void typify(string option){
		mySelect = new List<BaseSelection*>;
		mySelect->addList(input->getSelectList());
		numFlds = mySelect->cardinality();
		const TypeID* tmp = input->getTypeIDs();
		attrTypes = new TypeID[numFlds];
		for(int i = 0; i < numFlds; i++){
			attrTypes[i] = tmp[i];
//			cerr << "attrTypes[" << i << "] = " << attrTypes[i] << endl;
		}
	}

	virtual Iterator* createExec();
};
#endif

#endif
 
