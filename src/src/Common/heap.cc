#include <stdio.h>
#include "heap.h"

#define PARENT(x) ((x) >> 1)
#define LEFT_CHILD(x) ((x) << 1)
#define RIGHT_CHILD(x) (((x) << 1) | 1)

ArrayHeap::ArrayHeap()
{
  size = 0;
}

ArrayHeap::~ArrayHeap(){}

int ArrayHeap::get_size()
{
  return size;
}

int ArrayHeap::top_value()
{
  return value[1];
}

int ArrayHeap::top_priority()
{
  return priority[1];
}

void ArrayHeap::insert(int v, int p)
{
  size++;
  value[size] = v;
  priority[size] = p;
  correct(size);
}

int ArrayHeap::pop()
{
  int v = value[1];
  value[1] = value[size];
  priority[1] = priority[size];
  size--;
  if(size > 1)
    correct(1);
  return v;
}

void ArrayHeap::correct(int place)
{
  int temp;
  /* go up as much as possible */
  while(place > 1 && priority[place] > priority[PARENT(place)])
    {
      temp = priority[place];
      priority[place] = priority[PARENT(place)];
      priority[PARENT(place)] = temp;
      temp = value[place];
      value[place] = value[PARENT(place)];
      value[PARENT(place)] = temp;
      place = PARENT(place);
    }
  /* go down the tree */
  while(1)
    {
      if(LEFT_CHILD(place) > size) return;
      if(RIGHT_CHILD(place) > size) 
	{
	  if(priority[place] < priority[LEFT_CHILD(place)])
	    {
              temp = priority[place];
              priority[place] = priority[LEFT_CHILD(place)];
              priority[LEFT_CHILD(place)] = temp;
              temp = value[place];
              value[place] = value[LEFT_CHILD(place)];
              value[LEFT_CHILD(place)] = temp;
	    }
          return;
	}
      if(priority[place] < priority[LEFT_CHILD(place)] || priority[place] < priority[RIGHT_CHILD(place)])
	{
	  int p = LEFT_CHILD(place);
          if(priority[LEFT_CHILD(place)] < priority[RIGHT_CHILD(place)]) 
	    p = RIGHT_CHILD(place); 
          temp = priority[place];
          priority[place] = priority[p];
          priority[p] = temp;
          temp = value[place];
          value[place] = value[p];
          value[p] = temp;
	  place = p;
	}
      else return; 
    }
}


void ArrayHeap::display()
{
   printf("place\tpr\tvalue\n");
   for(int i = 1; i <= size; i++)
     printf("%d\t%d\t%d\n",i,priority[i],value[i]);
}

void ArrayHeap::reset()
{
  size = 0;
}
