#ifndef _REFCOBJECT_H_
#define _REFCOBJECT_H_

// (C) 1998- by Kevin S. Beyer

#include <stdlib.h>
#include <assert.h>

/*
   Refcerenced classes must publicly derive from RefcObject.
   The class must have a typedef "parent class" super defined.

   Try not to use use Refc<foo>& or Refc<foo>* (simply use Refc<foo>)
   because they can cause uncaught type errors:

       Refc<child1> c1;
       Refc<child2> c2 = new child2;
       Refc<parent>& p = c1;
       p = c2;               <== this works! now c1 points to a child2
                                 object, even though it's a Refc<child1>
       c1->child1method();   <== this compiles but does wierd stuff at runtime

   Also, do not explicitly call parent assignment operators:

       c1.Refc<RefcObject>::operator=(c2); <== c1 now points to a child2

   instead do:

       Refc<parent> p = c1;
       p = c2;               <== this is proper behavior; p can Refcerence
                                 either a child1 or a child2
       c1->child1method()    <== this works; c1 still Refcers to a child1

*/


template<class T> class Refc;


//---------------------------------------------------------------------------


class RefcObject
{
  int RefcCount;

public:

  RefcObject() : RefcCount(0) {}

  virtual ~RefcObject() { assert(RefcCount == 0); }

  RefcObject(const RefcObject& x) : RefcCount(0) {}

  RefcObject& operator=(const RefcObject& x) { return *this; }

  friend class Refc<class RefcObject>;
};


//---------------------------------------------------------------------------


template<class T>
class Refc
: public Refc<typename T::super>
{
public:

  typedef Refc<typename T::super> super;
  typedef Refc<RefcObject> base;
  
  Refc(T* x = NULL) : super(x) {}
  
  ~Refc() {}

  Refc(const Refc& x) : super(x) {}

  Refc& operator=(const Refc& x) {
    base::operator=(x);
    return *this;
  }

  Refc& operator=(T* x) {
    base::operator=(x);
    return *this;
  }

  T* operator->() { return (T*)base::operator->(); }

  const T* operator->() const  { return (T*)base::operator->(); }

  T& operator*() { return *(operator->()); }

  const T& operator*() const { return *(operator->()); }

};


//---------------------------------------------------------------------------


class Refc<RefcObject>
{
  RefcObject* object;

  void firstRef() {
    if( object ) {
      assert( object->RefcCount == 0 );
      object->RefcCount = 1;
    }
  }

  void addRef() {
    if( object ) {
      object->RefcCount++;
    }
  }

  void removeRef() {
    if( object ) {
      object->RefcCount--;
      //cout << "rc: " << object->RefcCount << '\n';
      assert(object->RefcCount >= 0);
      if( object->RefcCount == 0 ) {
        delete object;
      }
      object = NULL;            //kb: not really needed
    }
  }

public:

  Refc(RefcObject* x = NULL) : object(x) {
    firstRef();
  }

  Refc(const Refc& x) : object(x.object) {
    addRef();
  }

  ~Refc() { removeRef(); }

  Refc& operator=(const Refc& x) {
    removeRef();
    object = x.object;
    addRef();
    return *this;
  }

  Refc& operator=(RefcObject* x) {
    removeRef();
    object = x;
    firstRef();
    return *this;
  }

  void cast(const Refc& x) { operator=( x.object ); }

  RefcObject* operator->() {
    assert(object);
    return object;
  }

  const RefcObject* operator->() const {
    assert(object);
    return object;
  }

  bool operator==(const Refc& x) const {
    return object == x.object;
  }

  bool operator==(const RefcObject* x) const {
    return object == x;
  }

  // used for tests like: if( ref )
  operator void*() const { return object; }

  // used for tests like: if( !ref )
  bool operator!() const { return object == NULL; }
};



#endif // _REFCOBJECT_H_
