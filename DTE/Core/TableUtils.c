#include "TableUtils.h"
#include "ExecOp.h"
#include "StandardRead.h"
#include "myopt.h"

Iterator* createIteratorFor(
	const ISchema& schema, istream* in, const string& tableStr)
{
	assert(in && in->good());
	int numFlds = schema.getNumFlds();
	StandReadExec* fs = new StandReadExec(numFlds, schema.getTypeIDs(), in);
	Array<ExecExpr*>* select = new Array<ExecExpr*>(numFlds);
	for(int i = 0; i < numFlds; i++){
		(*select)[i] = new ExecSelect(0, i);
	}

	BaseSelection* name = new EnumSelection(0, "string");
	BaseSelection* value = new ConstantSelection(
		"string", strdup(tableStr.c_str()));
	BaseSelection* predicate = new Operator("=", name, value);
	assert(predicate);
	predicate->typeCheck();

	Array<ExecExpr*>* where = new Array<ExecExpr*>(1);
	TRY((*where)[0] = predicate->createExec(NULL, NULL), NULL);
	assert((*where)[0]);
	return new SelProjExec(fs, select, where);
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
		bitmap << 1;
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
