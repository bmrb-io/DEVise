#ifndef _MAPATTR_H_
#define _MAPATTR_H_

#include "ExecExpr.h"
#include <map>


//ksb: need to add table to load/store mappings
template< class ADT >
class MapAttrExec : public ExecExpr
{
public:

  typedef map<ADT, int4> MapType;

  MapAttrExec(ExecExpr* in /*, string mapTable*/ )
    : ExecExpr(INT_TP), input(in), nextID(0) {}
  
  virtual ~MapAttrExec() {}

  virtual const Type* evaluate(const Tuple* left, const Tuple* right) {
    ADT::ManagedType x = ADT::cast( input->evaluate(left, right) );
    int4 id;
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

  int4 nextID;

  MapType attrMap;

private:

  MapAttrExec(const MapAttrExec& x);
  MapAttrExec& operator=(const MapAttrExec& x);
};


#endif // _MAPATTR_H_
