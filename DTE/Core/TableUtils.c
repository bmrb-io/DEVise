#include "TableUtils.h"
#include "ExecOp.h"
#include "StandardRead.h"
#include "myopt.h"
#include "DTE/util/Del.h"

Iterator* createIteratorFor(const ISchema& schema, istream* in,
                            const string& tableStr)
{
  assert(in && in->good());
  int numFlds = schema.getNumFields();
  const DteTupleAdt& adt = schema.getAdt();
  Del<StandReadExec> fs = new StandReadExec(adt, in);
  
  Del<ExprList> project = new ExprList;
  for(int i = 0; i < numFlds; i++) {
    project->push_back(ExecExpr::createField(adt.getAdt(i), 0, i));
  }
  Del<ExecExpr> expr1 = ExecExpr::createField(adt.getAdt(0), 0, 0);
  Del<ExecExpr> expr2 = ExecExpr::createString(tableStr);
  Del<ExecExpr> expr3 = ExecExpr::createFunction("=", expr1.steal(),
                                                 expr2.steal());
  Del<ExprList> where = new ExprList;
  where->push_back(expr3);
  assert((*where)[0]);
  Iterator* iter = new SelProjExec(fs.steal(), where.steal(), project.steal());
  expr3.steal();
  return iter;
}

ostream& operator<<(ostream& out, const TableMap& x){
	out << "{";
	int tmpMap = x.bitmap;
	for(int i = 0; i < sizeof(int) * 8; i++){	
		if(tmpMap & 1){
			out << i + 1;
			out << " ";
		}
		tmpMap >>= 1;
	}
	out << "}";
	return out;
}

void TableMap::initialize(int numTabs){
	assert(numTabs <= sizeof(int) * 8);
	bitmap = 0;
	for(int i = 0; i < numTabs; i++){
		bitmap <<= 1;
		bitmap += 1;
	}
}

TableMap::Iterator TableMap::begin(){
	return Iterator(bitmap, 1);
}

TableMap::Iterator TableMap::end(){
	int last = (1 << (count() - 1));
	return Iterator(bitmap, last);
}

int TableMap::count() const {
	int cnt = 0;
	int tmpMap = bitmap;
	for(int i = 0; i < sizeof(int) * 8; i++){	
		cnt += ((tmpMap & 1) ? 1 : 0);
		tmpMap >>= 1;
	}
	return cnt;
}

TableMap::Iterator::Iterator(int bitmap, int counter) :
	refMap(bitmap), currMap(1), counter(counter)
{
	assert(bitmap);
	while(!(currMap & bitmap)){
		currMap <<= 1;
	}
}

TableMap::Iterator::Iterator() :
	refMap(0), currMap(0), counter(0){}

string TableMap::toString() const
{
	ostringstream tmp;
	tmp << *this;
	return tmp.str();
}

int TableMap::index() const {
	int tmpMap = bitmap;
	for(int i = 0; i < sizeof(int) * 8; i++){	
		if(tmpMap & 1){
			return i;
		}
		tmpMap >>= 1;
	}
	assert(0);
	return 0;
}

