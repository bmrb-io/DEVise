#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

//#include <iostream.h>   erased for sysdep.h
#include "types.h"
#include "sysdep.h"

typedef enum {Ascending, Descending } SortOrder;

#ifndef __GNUG__
using namespace std;
#endif

struct Node{
  const Tuple *tuple;
  int run_num;
};

class PQueue {
private:
  Node **Items;      // array of pointer to tuples
  int num_of_elems;  // Number of elements in Q

  int num_flds;      // Number of fields of Tuple
  TypeID *fld_types; // Attributes of the Tuple 
  int num_sort_flds;
  int *sort_flds;    // Array of col numbers on which to sort
  SortOrder order;

  GeneralPtr **comparePtrs; // Pointers to comparison operators

  void adjust(int root);

public:
  
  PQueue(int num_flds, TypeID *fld_types, int num_sort_flds, 
         int *sort_flds, SortOrder sort_order, int size) 
    : num_flds(num_flds), fld_types(fld_types), num_sort_flds(num_sort_flds), 
      sort_flds(sort_flds), order(sort_order)
    {
       Items = new Node* [size+1]; 
       num_of_elems = 0;
       
       comparePtrs  = new GeneralPtr*[num_flds];
       
       for (int i=0; i < num_flds; i++)
         {
           TypeID retVal;  // is a dummy	           
           TRY(comparePtrs[i] = getOperatorPtr("comp", fld_types[i],
					       fld_types[i],retVal), NVOID);
         }
    }
  
  ~PQueue(){ if (Items) delete [] Items;}
  
  void enq(Node*);
  Node* deq(); 
  inline int num_elems() { return num_of_elems; }  
};

#endif
