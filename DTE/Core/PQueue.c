//#include <stdlib.h>   erased for sysdep.h

#include "sysdep.h"
#include "PQueue.h"

void PQueue::enq(Node *newitem)
{
  Items[num_of_elems] = newitem; // add new node to end of heap
  
  // trickle new item to its proper position
  int place = num_of_elems;
  int parent = (place ? (place-1)/2 : -1);

  while (parent >= 0)
    {
      if (order == Ascending){
	if (tupleCompare(fld_types, num_sort_flds, sort_flds,
			 Items[place]->tuple, Items[parent]->tuple) >= 0)
	  break;
      }
       else{
	if (tupleCompare(fld_types, num_sort_flds, sort_flds,
			 Items[place]->tuple, Items[parent]->tuple) <= 0)
	  break;
      }

      // move newitem up & parent down
      Node *temp = Items[place];
      Items[place] = Items[parent];
      Items[parent] = temp;

      place = parent;
      parent = (place ? (place-1)/2 : -1);
    }			

  num_of_elems++;
  return;
}  

Node *PQueue::deq()
{
  if (num_of_elems == 0)
    return NULL;
  
  Node *min_elem;

  min_elem = Items[0];
  Items[0] = Items[--num_of_elems];
  adjust(0);

  return min_elem;
}

Node *PQueue::head()
{
  if (num_of_elems == 0)
    return NULL;
  return Items[0];
}

void PQueue::adjust(int root)
{  

  int child = 2*root +1;
  
  if (child < num_of_elems)
  {
    // Root is not a leaf
    int right_child = child + 1;
    
    int right = tupleCompare(fld_types, num_sort_flds, sort_flds,
                             Items[right_child]->tuple, Items[child]->tuple);
    if (right_child < num_of_elems) {
      
      if (order == Ascending){
	if (right < 0)
	  child = right_child; // index of smaller child
      }
      else{
	if (right > 0)
	  child = right_child; // index of larger child
      } 
    }
    
    int root_smaller;
    if (order == Ascending)
      root_smaller = tupleCompare(fld_types, num_sort_flds, sort_flds,
                                  Items[child]->tuple, Items[root]->tuple);
    else
      root_smaller = tupleCompare(fld_types, num_sort_flds, sort_flds,
                                  Items[root]->tuple, Items[child]->tuple);
 
    if (root_smaller < 0)
     {	
       Node *temp = Items[root];
       Items[root] = Items[child];
       Items[child] = temp;
       
       adjust(child);
     }
  }

  return;
} 

