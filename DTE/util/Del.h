#ifndef _DEL_H_
#define _DEL_H_

// (C) 1999- by Kevin S. Beyer

template<class T>
class Del
{
  T* ptr;

public:

  Del() : ptr(NULL) {}

  Del(T* p) : ptr(p) {}

  // for use by containers; not really for copying
  Del(const Del& x) : ptr(NULL) { assert(x.ptr == NULL); }

  ~Del() { delete ptr; }

  T* steal() { T* p = ptr; ptr = NULL; return p; }

  operator T*() { return ptr; }
  operator const T*() const { return ptr; }

  //operator const T&() { return *ptr; }

  T* operator->() { return ptr; }

  const T* operator->() const  { return ptr; }

  T& operator*() { return *ptr; }

  const T& operator*() const { return *ptr; }

  void operator=(T* p) { delete ptr; ptr = p; }

private:

  Del& operator=(const Del&);
};


//---------------------------------------------------------------------------


template<class T>
class DelArray
{
  T* ptr;

public:

  DelArray() : ptr(NULL) {}

  DelArray(T* p) : ptr(p) {}

  // for use by containers; not really for copying
  DelArray(const DelArray& x) : ptr(NULL) { assert(x.ptr == NULL); }

  ~DelArray() { delete [] ptr; }

  T* steal() { T* p = ptr; ptr = NULL; return p; }

  operator T*() { return ptr; }
  operator const T*() const { return ptr; }

  //operator const T&() { return *ptr; }

  T* operator->() { return ptr; }

  const T* operator->() const  { return ptr; }

  T& operator*() { return *ptr; }

  const T& operator*() const { return *ptr; }

  T& operator[](int i) { return ptr[i]; }

  const T& operator[](int i) const { return ptr[i]; }

  void operator=(T* p) { delete [] ptr; ptr = p; }

private:

  DelArray& operator=(const DelArray&);
};



#endif // _DEL_H_
