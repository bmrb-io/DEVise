#ifndef _FIXEDLENQUEUE_H_
#define _FIXEDLENQUEUE_H_

#include <assert.h>


template< class T >
class FixedLenQueue
{
public:

  FixedLenQueue();              // Init() *must* be called (used for arrays)

  FixedLenQueue(int maxlen);

  ~FixedLenQueue();

  void init(int maxlen);        // can only be called once, after def. const.

  int size();

  void push(const T& x);

  T& front();

  T& back();

  void pop();

  bool empty();

protected:

  int _maxlen;
  int _len;
  int _head;
  int _tail;
  T* _q;

private:
  FixedLenQueue(const FixedLenQueue& x);
  FixedLenQueue& operator=(const FixedLenQueue& other);
};


template< class T > inline
void FixedLenQueue<T>::init(int maxlen)
{
  assert(_q == (T*)0);
  _maxlen = maxlen;
  _len = 0;
  _head = 0;
  _tail = _maxlen;              // put first in [0]
  _q = new T[_maxlen];
}


template< class T > inline
FixedLenQueue<T>::FixedLenQueue()
{
  _q = (T*)0;
}


template< class T > inline
FixedLenQueue<T>::FixedLenQueue(int maxlen)
{
  _q = (T*)0;
  init(maxlen);
}


template< class T > inline
FixedLenQueue<T>::~FixedLenQueue()
{
  delete _q;
}


template< class T > inline
int FixedLenQueue<T>::size()
{
  return _len;
}


template< class T > inline
bool FixedLenQueue<T>::empty()
{
  return _len == 0;
}


template< class T > inline
void FixedLenQueue<T>::push(const T& x)
{
  if( ++_tail >= _maxlen ) _tail = 0;
  _q[_tail] = x;
  _len++;
  assert(!empty());             // overflow
}


template< class T > inline
T& FixedLenQueue<T>::front()
{
  assert(!empty());             // underflow
  return _q[_head];
}


template< class T > inline
T& FixedLenQueue<T>::back()
{
  assert(!empty());             // underflow
  return _q[_tail];
}


template< class T > inline
void FixedLenQueue<T>::pop()
{
  assert(!empty());             // underflow
  _len--;
  if( ++_head >= _maxlen ) _head = 0;
}


#endif // _FIXEDLENQUEUE_H_
