#ifndef _PRIORITYQUEUE_H_
#define _PRIORITYQUEUE_H_

// (C) 1999- by Kevin S. Beyer

#include <function.h> // for less
#include "DTE/util/Heap.h"


template<class T, class Compare = less<T> >
class PriorityQueue
{
  T* c;
  int N;
  Compare comp;
public:
  // call reserve() later
  PriorityQueue() : c(NULL), N(0), comp(comp) {}
  explicit PriorityQueue(Compare& comp) : c(NULL), N(0), comp(comp) {}
  explicit PriorityQueue(int maxSize) {
    N = 0;
    c = new T[maxSize+1];
  }
  explicit PriorityQueue(int maxSize, const Compare& comp) : comp(comp) {
    N = 0;
    c = new T[maxSize+1];
  }
  ~PriorityQueue() { delete [] c; }
  bool empty() const { return N == 0; }
  int size() const { return N; }
  const T& top() const { return c[1]; }
  void push(const T& x) {
    c[++N] = x;
    upHeap(c, comp, N);
  }
  void pop() {
    //c[1] = c[N--];
    //downHeap(c, N, comp, 1);
    popHeap(c, N, comp);
    N--;
  }
  void reserve(int maxSize) {
    delete [] c;
    c = new T[maxSize+1];
  }
};


#if 0
#include <vector.h>
// this version uses a dynamic vector
template<class T, class Compare = less<T> >
class PriorityQueue
{
public:
  typedef vector<T> Sequence;
  typedef typename Sequence::value_type value_type;
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::reference reference;
  typedef typename Sequence::const_reference const_reference;
protected:
  vector<T> c;
  Compare comp;
public:
  PriorityQueue() : c() {}
  explicit PriorityQueue(const Compare& x)
    : c(), comp(x) {}
  explicit PriorityQueue(const Compare& x, int res)
    : c(), comp(x) { c.reserve(res); }
  bool empty() const { return c.empty(); }
  size_type size() const { return c.size(); }
  const_reference top() const { return c.front(); }
  void push(const value_type& x) {
    c.push_back(x); 
    upHeap(c.begin(), c.size(), comp, c.size());
  }
  void pop() {
    c[0] = c.back();
    c.pop_back();
    downHeap(c.begin(), c.size(), comp, 1);
  }
  void reserve(size_type N) { c.reserve(N); }
};
#endif

#endif // _PRIORITYQUEUE_H_
