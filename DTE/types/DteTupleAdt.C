// (C) 1999- by Kevin S. Beyer


#include "./DteTupleAdt.h"


void DteTupleAdt::precompute()
{
  int n = types.size();
  pointerSize = n * sizeof(Type*);
  maxSize = sizeof(int4) + pointerSize;
    
  allNonNull = true;
  allFixedSize = true;
  allCanCompare = true;
  allCanHash = true;
    
  for(int i = 0 ; i < n ; i++ ) {
    maxSize += types[i]->getMaxSize();
    if( types[i]->isNullable() ) {
      allNonNull = false;
    }
    if( types[i]->isVariableSize() ) {
      allFixedSize = false;
    }
    if( ! types[i]->canCompare() ) {
      allCanCompare = false;
    }
    if( ! types[i]->canHash() ) {
      allCanHash = false;
    }
  }
  maxSize = Align(maxSize, POINTER_ALIGN);
}


DteTupleAdt::DteTupleAdt()
  : DteAdt(false, POINTER_ALIGN)
{
  pointerSize = 0;
  maxSize = 0;
  allNonNull = allFixedSize = allCanCompare = allCanHash = true;
}


DteTupleAdt::DteTupleAdt(const DteTupleAdt& x)
  : DteAdt(x), types(x.types)
{
  pointerSize = x.pointerSize;
  maxSize = x.maxSize;
  allNonNull = x.allNonNull;
  allFixedSize = x.allFixedSize;
  allCanCompare = x.allCanCompare;
  allCanHash = x.allCanHash;
}


//kb: should alignment be max alignment?
//kb: not a problem with everything being variable length...
DteTupleAdt::DteTupleAdt(const DteAdtList& types, bool nullable = false)
  : DteAdt(nullable, POINTER_ALIGN), types(types)
{
  precompute();
}


DteTupleAdt::~DteTupleAdt()
{
}


DteTupleAdt& DteTupleAdt::operator=(const DteTupleAdt& x)
{
  DteAdt::operator=(x);
  types = x.types;
  pointerSize = x.pointerSize;
  maxSize = x.maxSize;
  allNonNull = x.allNonNull;
  allFixedSize = x.allFixedSize;
  allCanCompare = x.allCanCompare;
  allCanHash = x.allCanHash;
  return *this;
}


void DteTupleAdt::operator=(const DteAdtList& types)
{
  nullable = false;
  this->types = types;
  precompute();
}

    
void DteTupleAdt::push_front(const DteAdt& x)
{
  types.insert(types.begin(), x);
  precompute();
}


void DteTupleAdt::push_back(const DteAdt& x)
{
  types.push_back(x);
  precompute();
}


void DteTupleAdt::append(const DteTupleAdt& x)
{
  types.insert(types.end(), x.types.begin(), x.types.end());
  precompute();
}


int DteTupleAdt::getTypeID() const
{
  return typeID;
}


string DteTupleAdt::getTypeName() const
{
  return typeName;
}


string DteTupleAdt::getTypeSpec() const
{
  string s(typeName);
  s += "(";
  int n = types.size();
  if( n > 0 ) {
    s += types[0]->getTypeSpec();
    for(int i = 1 ; i < n ; i++) {
      s += ", ";
      s += types[i]->getTypeSpec();
    }
  }
  s += ")";
  return s;
}


DteAdt* DteTupleAdt::clone() const
{
  return new DteTupleAdt(*this);
}


int DteTupleAdt::copyNonNull(const Type* x, char* to, int size) const
{
  int n = types.size();
  const Tuple* tup = pointers(x);

  char* endTo = to + size;

  Tuple* ptrs = (Tuple*)to;
  to += pointerSize;

  if( to >= endTo ) {
    return -1;
  }
    
  for(int i = 0 ; i < n ; i++ ) {
    assert(to <= endTo);
    if( tup[i] == NULL ) {
      ptrs[i] = NULL;
    } else {
      to = types[i]->align(to);
      ptrs[i] = to;
      int u = types[i]->copyNonNull(tup[i], to, endTo - to);
      if( u == -1 ) return -1;
      to += u;
    }
  }
  return to - (char*)ptrs;
}


void DteTupleAdt::deallocate(void* x) const
{
  assert(!"NYI");
}


Type* DteTupleAdt::allocateCopy(const void* x) const
{
  assert(!"NYI");
  return NULL;
}


bool DteTupleAdt::canCompare() const
{
  return allCanCompare;
}


int DteTupleAdt::compareNonNull(const Type* x, const Type* y) const
{
  int n = types.size();
  const Tuple* xtup = pointers(x);
  const Tuple* ytup = pointers(y);
  for(int i = 0 ; i < n ; i++) {
    int r = types[i]->compare(xtup[i], ytup[i]);
    if( r != 0 ) return r;
  }
  return 0;
}


bool DteTupleAdt::canHash() const
{
  return allCanHash;
}


// returns a hash value for this tuple
int DteTupleAdt::hashNonNull(const Type* x) const
{
  int n = types.size();
  int h = 0;
  const Tuple* xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    h = 5 * h + types[i]->hash(xtup[i]);
  }
  return h;
}


void DteTupleAdt::swizzle(Type* x, char* page) const
{
  int n = types.size();
  Type** xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    if( xtup[i] ) {
      //kb: I could avoid this call if swizzle not needed...
      types[i]->swizzle(xtup[i], page);
      xtup[i] = (Type*)((char*)(xtup[i]) - page);
    }
  }
}


void DteTupleAdt::unswizzle(Type* x, char* page) const
{
  int n = types.size();
  Type** xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    if( xtup[i] ) {
      xtup[i] = page + (int)(xtup[i]);
      types[i]->unswizzle(xtup[i], page);
    }
  }
}


// prints the tuple without braces
void DteTupleAdt::print(ostream& s, const Tuple* tup) const
{
  int n = types.size();
  if( n > 0 ) {
    types[0]->print(s, tup[0]);
    for(int i = 1 ; i < n ; i++) {
      s << ", ";
      types[i]->print(s, tup[i]);
    }
  }
}


void DteTupleAdt::printNonNull(ostream& s, const Type* x) const
{
  s << '{';
  print(s, pointers(x));
  s << '}';
}


void DteTupleAdt::toAscii(ostream& s, const Type* x) const
{
  int n = types.size();
  const Tuple* xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    if( types[i]->isNullable() ) { // 0 = non-null, 1 = null
      s << (xtup[i] ? '0' : '1') << ' ';
    }
    if( xtup[i] ) {
      types[i]->toAscii(s, xtup[i]);
      s << ' ';
    }
  }
}


int DteTupleAdt::fromAscii(istream& s, char* to, int size) const
{
  int n = types.size();
  char* endTo = to + size;
  Tuple* ptrs = (Tuple*)to;
  to += pointerSize;

  if( to >= endTo ) {
    return -1;
  }
    
  for(int i = 0 ; i < n ; i++ ) {
    if( types[i]->isNullable() ) {
      char ni;
      s >> ni;
      if( !s ) return -1;
      if( ni == '1' ) {
        ptrs[i] = NULL;
        continue;
      }
      assert( ni == '0' );
    }
    // non-null
    assert(to <= endTo);
    to = types[i]->align(to);
    ptrs[i] = to;
    int u = types[i]->fromAscii(s, to, endTo - to);
    if( u == -1 ) return -1;
    to += u;
  }
  return to - (char*)ptrs;
}


void DteTupleAdt::toBinary(ostream& s, const Type* x) const
{
  int n = types.size();
  const Tuple* xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    if( types[i]->isNullable() ) { // 0 = non-null, 1 = null
      s.put(xtup[i] ? '\x00' : '\x01');
    }
    if( xtup[i] ) {
      types[i]->toBinary(s, xtup[i]);
    }
  }
}


int DteTupleAdt::fromBinary(istream& s, char* to, int size) const
{
  int n = types.size();
  char* endTo = to + size;
  Tuple* ptrs = (Tuple*)to;
  to += pointerSize;

  if( to >= endTo ) {
    return -1;
  }
    
  for(int i = 0 ; i < n ; i++ ) {
    if( types[i]->isNullable() ) { // 0 = non-null, 1 = null
      char ni;
      s.get(ni);
      if( !s ) return -1;
      if( ni == '\x01' ) {
        ptrs[i] = NULL;
        continue;
      }
      assert( ni == '\x00' );
    }
    assert(to <= endTo);
    to = types[i]->align(to);
    ptrs[i] = to;
    int u = types[i]->fromBinary(s, to, endTo - to);
    if( u == -1 ) return -1;
    to += u;
  }
  return to - (char*)ptrs;
}


void DteTupleAdt::toNet(ostream& s, const Type* x) const
{
  int n = types.size();
  const Tuple* xtup = pointers(x);
  for(int i = 0 ; i < n ; i++) {
    if( types[i]->isNullable() ) { // 0 = non-null, 1 = null
      s.put(xtup[i] ? '\x00' : '\x01');
    }
    if( xtup[i] ) {
      types[i]->toNet(s, xtup[i]);
    }
  }
}


int DteTupleAdt::fromNet(istream& s, char* to, int size) const
{
  int n = types.size();
  char* endTo = to + size;
  Tuple* ptrs = (Tuple*)to;
  to += pointerSize;

  if( to >= endTo ) {
    return -1;
  }
    
  for(int i = 0 ; i < n ; i++ ) {
    if( types[i]->isNullable() ) { // 0 = non-null, 1 = null
      char ni;
      s.get(ni);
      if( !s ) return -1;
      if( ni == '\x01' ) {
        ptrs[i] = NULL;
        continue;
      }
      //if( ni != '\x00' ) {
      //  cerr << "bad ni: " << (int)ni << " at offset " << s.tellg() << endl;
      //}
      assert( ni == '\x00' );
    }
    assert(to <= endTo);
    to = types[i]->align(to);
    ptrs[i] = to;
    int u = types[i]->fromNet(s, to, endTo - to);
    if( u == -1 ) return -1;
    to += u;
  }
  return to - (char*)ptrs;
}


int DteTupleAdt::getMaxSize() const
{
  return maxSize;
}


bool DteTupleAdt::isVariableSize() const
{
  return !allFixedSize;
}

