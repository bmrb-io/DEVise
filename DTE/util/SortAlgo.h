// (C) 1999- by Kevin S. Beyer

#include <function.h> // for less
#include <stl_algobase.h> // for swap

template <class T, class Compare>
void insertSort(T* a, int N, const Compare& lessFn)
{
  for(int i = 1 ; i < N ; i++) {
    T v = a[i];
    int j;
    for( j = i ; j > 0 && lessFn(v, a[j-1]) ; j-- ) {
      a[j] = a[j-1];
    }
    a[j] = v;
  }
}

template <class T>
inline void insertSort(T* a, int N)
{
  insertSort(a, N, less<T>());
}

template <class RandContainer, class Compare>
void insertSort(RandContainer& container, const Compare& comp)
{
  insertSort(c.begin(), static_cast<int>(c.size()), comp);
}

template <class RandContainer>
void insertSort(RandContainer& c)
{
  insertSort(c.begin(), static_cast<int>(c.size()));
}

//---------------------------------------------------------------------------

const int QUICKSORT_MIN_PARTITION = 12;

template <class T, class Compare>
inline void sort3(T& x1, T& x2, T& x3, const Compare& comp)
{
  if( comp(x2, x1) ) swap(x1, x2);
  if( comp(x3, x1) ) swap(x1, x3);
  if( comp(x3, x2) ) swap(x2, x3);
}


// last is actual last value, not end() (ie, one past)
template <class T, class Compare>
inline T* partition(T* first, T* last, const Compare& comp)
{
  const int len = last - first; // len is actually length - 1
  if( QUICKSORT_MIN_PARTITION < 3 ) { // needed if not using min_part
    if( len == 1 ) {            // two element list
      if( comp(*last, *first) ) swap(*first, *last);
      return last;
    }
  }

  T* mid = first + len / 2;     // use median of 3 as pivot
  sort3(*first, *mid, *last, comp);
  --last;                       // keep last of the in last spot
  swap(*mid, *last);            // move pivot to 2nd last
  mid = last;
  while(true) {                 // partition list based on pivot
    ++first;
    --last;
    while( comp(*first, *mid) ) ++first;
    while( comp(*mid, *last) ) --last;
    if( first >= last ) break;  // list is now partitioned
    swap(*first, *last);
  }
  swap(*first, *mid);           // put pivot in its proper place
  return first;
}

#include <vector>

template <class T, class Compare>
void quickSort(T* first, int N, const Compare& comp)
{
  T* saveFirst = first;
  T* last = first + N - 1;
  if( N >= QUICKSORT_MIN_PARTITION ) {
    vector<T*> stk;
    stk.reserve(32*2); //(intlog2(N)+1)*2);
    stk.push_back(first);
    stk.push_back(last);
    
    while( !stk.empty() ) {
      last = stk.back(); stk.pop_back();
      first = stk.back(); stk.pop_back();
      while( last - first >= QUICKSORT_MIN_PARTITION ) {
        T* i = partition(first, last, comp);
        //cout << "partition value: " << *i << endl;
        if( i - first > last - i ) {
          stk.push_back(first); stk.push_back(i-1); first = i + 1;
        } else {
          stk.push_back(i+1); stk.push_back(last); last = i - 1;
        }
      }
    }
  }
  if( QUICKSORT_MIN_PARTITION > 1 ) {
    insertSort(saveFirst, N, comp);
  }
}

template <class T>
inline void quickSort(T* first, int N)
{
  quickSort(first, N, less<T>());
}

template <class RandContainer>
inline void quickSort(RandContainer& c)
{
  quickSort(c.begin(), static_cast<int>(c.size()));
}

template <class RandContainer, class Compare>
inline void quickSort(RandContainer& c, const Compare& comp)
{
  quickSort(c.begin(), static_cast<int>(c.size()), comp);
}

//---------------------------------------------------------------------------

#if 0
// this is for 2-d arrays or higher

template<class T>
inline void ksbSwap(T* a, T* b, const int width)
{
  T tmp;
  for(int i = 0 ; i < width ; i++) {
    tmp = a[i];
    a[i] = b[i];
    b[i] = tmp;
  }
}


template<class T>
inline void ksbCopy(T* a, const T* b, const int width)
{
  for(int i = 0 ; i < width ; i++) {
    a[i] = b[i];
  }
}

template <class T, class Compare>
inline void sort3v2(T* x1, T* x2, T* x3,
                    const int width, const Compare& comp)
{
  if( comp(x2, x1) ) ksbSwap(x1, x2, width);
  if( comp(x3, x1) ) ksbSwap(x1, x3, width);
  if( comp(x3, x2) ) ksbSwap(x2, x3, width);
}

template <class RandIterator, class Compare, class T>
void insertSort2Aux(RandIterator first, RandIterator last,
                    const int width, const Compare& comp, T*)
{
  T* v = new T[width];
  for( RandIterator i = first + width ; i != last ; i += width ) {
    ksbCopy(v, i, width);
    RandIterator j = i;
    RandIterator prev = j - width;
    while( j > first && comp(v, prev) ) {
      ksbCopy(j, prev, width);
      j = prev;
      prev -= width;
    }
    ksbCopy(j, v, width);
  }
  delete [] v;
}

// last is actual last value, not end() (ie, one past)
template <class RandIterator, class Compare>
inline RandIterator partition2(RandIterator first, RandIterator last,
                               const int width, const Compare& comp)
{
  const int len = (last - first) / width;
  if( QUICKSORT_MIN_PARTITION < 3 ) { // needed if not using min_part
    if( len == 1 ) {
      if( comp(last, first) ) ksbSwap(first, last, width);
      return last;
    }
  }

  // use median of 3 as pivot
  RandIterator mid = first + (len / 2) * width;
  sort3v2(first, mid, last, width, comp);
  last -= width;                // keep last of the in last spot
  ksbSwap(mid, last, width);    // move pivot to 2nd last
  mid = last;
  while(true) {                 // partition list based on pivot
    first += width;
    last -= width;
    while( comp(first, mid) ) first += width;
    while( comp(mid, last) ) last -= width;
    if( first >= last ) break;  // list is now partitioned
    ksbSwap(first, last, width);
  }
  ksbSwap(first, mid, width);   // put pivot in its proper place
  return first;
}

#include <vector>

template <class RandIterator, class Compare>
void quickSort2Aux1(RandIterator first, RandIterator last,
                    const int width, const Compare& comp)
{
  RandIterator saveFirst(first);
  RandIterator saveLast(last);
  last -= width;                // last was one past, now actually last
  int N = (last - first) / width;
  if( N >= QUICKSORT_MIN_PARTITION ) {
    vector<RandIterator> stk;
    //stk.reserve((intlog2(N)+1)*2);
    stk.push_back(first);
    stk.push_back(last);
    
    while( !stk.empty() ) {
      last = stk.back(); stk.pop_back();
      first = stk.back(); stk.pop_back();
      N = (last - first) / width; // N = count - 1
      while( N >= QUICKSORT_MIN_PARTITION ) {
        RandIterator i = partition2(first, last, width, comp);
        if( i - first > last - i ) {
          stk.push_back(first); stk.push_back(i-width); first = i + width;
        } else {
          stk.push_back(i+width); stk.push_back(last); last = i - width;
        }
        N = (last - first) / width;
      }
    }
  }
  if( QUICKSORT_MIN_PARTITION > 1 ) {
    insertSort2Aux(saveFirst, saveLast, width, comp, value_type(first));
  }
}
#endif

//---------------------------------------------------------------------------

template <class T, class Compare> 
void downHeap(T* heap, const int N, int k, const Compare& comp)
{
  T v = heap[k];
  // make heap[0] -> heap[1]
  k++;
  --heap;
  const int nonLeaf = N/2;
  while( k <= nonLeaf ) {
    int j = k * 2;
    if( j < N && comp(heap[j], heap[j+1]) ) j++;
    if( comp(heap[j], v) ) break;
    heap[k] = heap[j];
    k = j;
  }
  heap[k] = v;
}

template <class T, class Compare>
void heapSort(T* heap, int N, const Compare& comp)
{
  for(int k = N/2 - 1 ; k >= 0 ; k--) {
    downHeap(heap, N, k, comp);
  }
  while( N > 0 ) {
    --N;
    swap(heap[0], heap[N]);
    downHeap(heap, N, 0, comp);
  }
}

template <class T>
inline void heapSort(T* heap, int N)
{
  heapSort(heap, N, less<T>());
}

template <class RandContainer>
inline void heapSort(RandContainer& c)
{
  heapSort(c.begin(), static_cast<int>(c.size()));
}

template <class RandContainer, class Compare>
inline void heapSort(RandContainer& c, const Compare& comp)
{
  heapSort(c.begin(), c.size(), comp);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
