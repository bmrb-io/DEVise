#ifndef _MAPATTR_H_
#define _MAPATTR_H_

// $Id$
// use 'cvs log' for log entries

#include "ExecExpr.h"
#include <map>


//ksb: need to add table to load/store mappings
template< class T >
class MapAttrExec : public ExecExpr
{
public:

  MapAttrExec(ExecExpr* in /*, string mapTable*/ )
    : ExecExpr(INT_TP), input(in), nextID(0) {}
  
  virtual ~MapAttrExec() {}

  inline T cast(const Type* a);
  
  virtual const Type* evaluate(const Tuple* left, const Tuple* right) {
    T x = cast( input->evaluate(left, right) );
    int id;
    map<T,int>::const_iterator mi = attrMap.find(x);
    if( mi == attrMap.end() ) {
      id = nextID++;
      attrMap[x] = id;
    } else {
      id = mi->second;
    }
    return (Type*)id;
  }

protected:

  ExecExpr* input;

  int nextID;

  map<T, int> attrMap;

private:

  MapAttrExec(const MapAttrExec& x);
  MapAttrExec& operator=(const MapAttrExec& x);
};


inline int MapAttrExec<int>::cast(const Type* a) {
  return (int)a;
}

inline double MapAttrExec<double>::cast(const Type* a) {
  return *(double*)a;
}

inline string MapAttrExec<string>::cast(const Type* a) {
  return string((const char*)a);
}

#endif // _MAPATTR_H_
