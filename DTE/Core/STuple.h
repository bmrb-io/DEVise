#ifndef _STUPLE_H_
#define _STUPLE_H_

// (C) 1999- by Kevin S. Beyer

// This is a shallow tuple (ie, it doesn't own its fields, but it
// does own the pointers to the fields.  The pointers can be set to
// NULL.
class STuple
{
  Type** tuple;
  int numFields;

public:

  // resize() must be called later
  STuple() : tuple(0), numFields(0) {}

  STuple(int numFields) : tuple(0), numFields(0) { resize(numFields); }

  ~STuple() { delete [] tuple; }

  const Type*& operator[](int i) { return tuple[i]; }

  operator const Tuple*() { return tuple; }

  void resize(int numFields) {
    delete tuple;
    this->numFields = numFields;
    tuple = new Type*[numFields];
    for(int i = 0 ; i < numFields ; i++) tuple[i] = NULL;
  }

private:

  STuple(const STuple&);
  STuple& operator=(const STuple&);
};

#endif // _STUPLE_H_
