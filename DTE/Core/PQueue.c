#include <stdlib.h>

#include "PQueue.h"

void PQueue::enq(Node *newitem)
{
  Items[num_of_elems] = newitem; // add new node to end of heap
  
  // trickle new item to its proper position
  int place = num_of_elems;
  int parent = (place ? (place-1)/2 : -1);

  while (parent >= 0)
    {
      if (tupleCompare(sort_flds, num_sort_flds, comparePtrs,
                       Items[place]->tuple, Items[parent]->tuple) < 0)
  	 break;
      
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

void PQueue::adjust(int root)
{  

  int child = 2*root +1;
  
  if (child < num_of_elems)
  {
    // Root is not a leaf
    int right_child = child + 1;
    
    int right = tupleCompare(sort_flds, num_sort_flds, comparePtrs,
                             Items[right_child]->tuple, Items[child]->tuple) ;

    if ((right_child < num_of_elems) && right < 0)
      child = right_child; // index of larger child
    
    int root_smaller = tupleCompare(sort_flds, num_sort_flds, comparePtrs,
                                Items[child]->tuple, Items[root]->tuple);
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
  
