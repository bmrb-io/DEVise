// (C) 1999- by Kevin S. Beyer

#include "SortAlgo.h"
#include "../util/Timeval.h"
#include "../util/DteAlgo.h"
#include <stdlib.h>

template <class T>
struct ptr_less : public binary_function<T, T, bool> {
    bool operator()(const T* x, const T* y) const { assert(x); assert(y); return *x < *y; }
};

//typedef int Type;
template<int card, int keySize>
struct TypeT
{
  int a[card];
  TypeT() {
    for(int i = 0 ; i < card ; i++) {
      a[i] = (lrand48()%2);
    }
  }
#if 1
  bool operator<(const TypeT& x) const {
    //return a[0] < x.a[0];
    for(int i = 0 ; i < keySize - 1 ; i++) {
      if( a[i] < x.a[i] ) return true;
      if( a[i] > x.a[i] ) return false;
    }
    return a[keySize-1] < x.a[keySize-1];
  }
#endif
  //friend class tuple_less;
};

//typedef TypeT<10,1> Type;
typedef int Type;

struct tuple_less : public binary_function<Type, Type, bool>
{
  int keys;
  tuple_less(int keys) : keys(keys-1) {}
  bool operator()(const Type* a, const Type* b) const {
    for(int i = 0 ; i < keys ; i++) {
      if( a[i] < b[i] ) return true;
      if( a[i] > b[i] ) return false;
    }
    return a[keys] < b[keys];
  }
};

#if 0
struct ptr_tuple_less : public binary_function<Type*, Type*, bool>
{
  int keys;
  ptr_tuple_less(int keys) : keys(keys-1) {}
  bool operator()(const Type* x, const Type* y) const {
    const int* a = x->a;
    const int* b = y->a;
    for(int i = 0 ; i < keys ; i++) {
      if( a[i] < b[i] ) return true;
      if( a[i] > b[i] ) return false;
    }
    return a[keys] < b[keys];
  }
};
#endif


#if 0
int cmp(const void* a, const void*b) {
  if( *(Type*)a < *(Type*)b ) return -1;
  if( *(Type*)a > *(Type*)b ) return +1;
  return 0;
}

int ptrcmp(const void* a, const void*b) {
  if( **(Type**)a < **(Type**)b ) return -1;
  if( **(Type**)a > **(Type**)b ) return +1;
  return 0;
}
#endif

int main(int ac, char* av[])
{
  size_t N = 500000;
  size_t W = atoi(av[1]);
  vector<int> a; a.reserve(N*W);
  vector<int*> b; b.reserve(N);
  for(size_t i = 0 ; i < N ; i++) {
    b.push_back(a.end());
    for(size_t j = 0 ; j < W; j++) {
      int k = lrand48();
      a.push_back(k);
    }
  }
  //print_list(cout, a);  cout << endl;
  Timeval time;

  //qsort(a.begin(), a.size(), sizeof(Type), cmp);
  //qsort(b.begin(), b.size(), sizeof(Type*), ptrcmp);

  quickSort(a);
  //quickSort2Aux1(a.begin(), a.end(), W, tuple_less(1));
    //(b, ptr_less<Type>());
  //quickSort(b, ptr_tuple_less(1));

  //heapSort(a);
  //heapSort(b, ptr_less<Type>());

  cout << "sort time: " << time.ElapsedTime() << endl;
#if 1
  //assert(a.size() == N);
  //print_list(cout, b); cout << endl;
  for(size_t i = 1 ; i < N ; i++) {
    //cout << *(b[i-1]) << " < " << *(b[i]) << endl;
    //assert( *(b[i-1]) < *(b[i]) );
    //if( a[i-1] > a[i] ) {
    if( *b[i-1] > *b[i] ) {
      cout << "error at " << i << ": " << *b[i-1] << " > " << *b[i] << endl;
      //cout << "error at " << i << endl;
    }
  }
  //print_list(cout, a);  cout << endl;
#endif
}
