#ifndef _CLONEOBJECT_H_
#define _CLONEOBJECT_H_

// (C) 1999- by Kevin S. Beyer


template< class T >
class CloneObject
{
  T* object;

public:

  CloneObject() : object(0) {}

  ~CloneObject() { delete object; }

  CloneObject(const T* x) {
    object = x ? x->clone() : 0;
  }

  CloneObject(const T& x) {
    object = x.clone();
  }

  CloneObject(const CloneObject& x) {
    object = x.object ? x.object->clone() : 0;
  }

  CloneObject& operator=(const T* x) {
    delete object;
    object = x ? x->clone() : 0;
    return *this;
  }

  CloneObject& operator=(const T& x) {
    delete object;
    object = x.clone();
    return *this;
  }

  CloneObject& operator=(const CloneObject& x) { return operator=(x.object); }

  T* operator->() { return object; }
  const T* operator->() const { return object; }

  T& operator*() { return *object; }
  const T& operator*() const { return *object; }

  bool operator==(const T* x) const { return object == x; }
  bool operator==(const CloneObject& x) const { return object == x.object; }
};


#endif // _CLONEOBJECT_H_
