#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include <assert.h>

class Range
{
public:

  // lo and hi now belong to Range.  lo.getType() must equal hi.getType
  // if lo and/or hi is null, +/- infinity will be used.
  Range(const TypeID& type, Type* lo, Type* hi)
    : _type(type), _lo(lo), _hi(hi) {
      if( type != INT_TP ) assert(lo && hi);
  }

  // +infinity : -infinity
  Range(const TypeID& type) : _type(type) {
    _lo = createNegInf(_type);
    _hi = createPosInf(_type);
  }

  ~Range() {
    DestroyPtr del = getDestroyPtr(_type);
    del(_lo);
    del(_hi);
  }

  const TypeID& getType() const { return _type; }

  Type* getLow() { return _lo; }

  Type* getHigh() { return _hi; }

protected:

  TypeID _type;

  Type* _lo;

  Type* _hi;

  Range(const Range& x);
  Range& operator=(const Range& x);
};


//---------------------------------------------------------------------------


class BoundingBox
{
public:

  BoundingBox() {}

  ~BoundingBox();

  int dims();

  // range now belongs to BoundingBox
  void push_back(Range* r);

  const TypeIDList& getTypes();

  string rtreeTypes();

  char* rtreePack();            // must be deleted by caller

protected:

  TypeIDList _types;
  vector<Range*> _range;

private:

  BoundingBox(const BoundingBox& x);
  BoundingBox& operator=(const BoundingBox& x);
};


//kb: these should probably be moved to a .c file...

inline
int BoundingBox::dims()
{
  return _types.size();
}


inline
BoundingBox::~BoundingBox()
{
  int n = dims();
  for(int i = 0 ; i < n ; i++) {
    delete _range[i];
  }
}


inline
void BoundingBox::push_back(Range* r)
{
  _types.push_back(r->getType());
  _range.push_back(r);
}


inline
const TypeIDList& BoundingBox::getTypes()
{
  return _types;
}


inline
string BoundingBox::rtreeTypes()
{
  string s;
  int d = dims();
  for(int i = 0 ; i < d ; i++) {
    s += rTreeEncode(_types[i]);
  }
  return s;
}


inline
char* BoundingBox::rtreePack()
{
  int d = dims();
  vector<MarshalPtr> marshal = getMarshalPtrs(_types);
  int sz = 0;
  vector<int> sizes(d);
  for(int i = 0; i < d ; i++) {
    sizes[i] = packSize(_types[i]);
    sz += sizes[i] * 2;
  }
  char* data = new char[sz+1];  // +1 for check
  memset(data, 0, sz+1);
  int offset = 0;
  for(int i = 0; i < d ; i++) {
    Type* t = _range[i]->getLow();
    marshal[i](t, data + offset);
    offset += sizes[i];
    assert(data[offset] == '\0');
  }
  for(int i = 0; i < d ; i++) {
    Type* t = _range[i]->getHigh();
    marshal[i](t, data + offset);
    offset += sizes[i];
    assert(data[offset] == '\0');
  }
  return data;
}



#endif // _BOUNDINGBOX_H_
