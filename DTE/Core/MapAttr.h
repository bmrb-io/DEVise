#ifndef _MAPATTR_H_
#define _MAPATTR_H_

#include "ExecExpr.h"
#include <map>


//ksb: need to add table to load/store mappings
template< class ADT >
class MapAttrExec : public ExecExpr
{
public:

  typedef typename ADT::ManagedType T;
  typedef map<T, int4> MapType;

  MapAttrExec(ExecExpr* in /*, string mapTable*/ )
    : ExecExpr(DteIntAdt(false)), input(in), nextID(0) {}
  
  virtual ~MapAttrExec() {}

  virtual const Type* eval(const Tuple* left, const Tuple* right) {
    T x = ADT::cast( input->eval(left, right) );
    MapType::const_iterator mi = attrMap.find(x);
    if( mi == attrMap.end() ) {
      id = nextID++;
      attrMap[x] = id;
    } else {
      id = mi->second;
    }
    return &id;
  }

protected:

  ExecExpr* input;

  MapType attrMap;

  int4 nextID;

  int4 id;

private:

  MapAttrExec(const MapAttrExec& x);
  MapAttrExec& operator=(const MapAttrExec& x);
};


#endif // _MAPATTR_H_
