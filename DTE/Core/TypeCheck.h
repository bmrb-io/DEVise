#ifndef TYPECHECK_H
#define TYPECHECK_H

#include <vector>
#include "queue.h"
#include <string>
#include <map.h>
#include "StringLess.h"

#ifndef __GNUG__
using namespace std;
#endif

// This class is the owner of all BaseSelection objects

class BaseSelection;
class TableAlias;

class TypeCheck {
	map<string, BaseSelection*, StringLess> symtab;
public:
	void initialize(List<TableAlias*>* tableList);
	void insert(BaseSelection* element);
     void setupSelList(vector<BaseSelection*>& list);
	BaseSelection* TypeCheck::resolve(BaseSelection* curr);
     void resolve(vector<BaseSelection*>& list);	// throws
	~TypeCheck();
};

#endif
