#ifndef _HEAP_H_
#define _HEAP_H_

// (C) 1999- by Kevin S. Beyer

// indexes start at 1
template<class T, class Compare>
void upHeap(T a[], const Compare& comp, int k)
{
  T v = a[k];
  while( k > 1 ) {
    int parent = k/2;
    if( comp(a[parent], v) ) break;
    a[k] = a[parent];
    k = parent;
  }
  a[k] = v;
}


// indexes start at 1
template<class T, class Compare>
void downHeap(T a[], int N, const Compare& comp, int k)
{
  T v = a[k];
  int last = N/2;
  while( k <= last ) {
    int j = 2 * k;
    if( j < N && comp(a[j+1], a[j]) ) j++;
    if( comp(v, a[j]) ) break;
    a[k] = a[j];
    k = j;
  }
  a[k] = v;
}

// indexes start at 1
template<class T, class Compare>
void popHeap(T a[], int N, const Compare& comp)
{
  int hole = 1;
  int child = 2; // * hole;
  while(child < N) {
    if( !comp(a[child], a[child+1]) ) {	// promote right child
      child++;
    }
    a[hole] = a[child];
    hole = child;
    child *= 2;
  }
  if( hole < N ) {
    a[hole] = a[N];
    upHeap(a, comp, hole);
  }
}


#endif // _HEAP_H_
