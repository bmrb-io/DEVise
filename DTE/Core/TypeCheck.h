#ifndef TYPECHECK_H
#define TYPECHECK_H

#include <vector>
#include "queue.h"
#include <string>
#include <map.h>

#ifndef __GNUG__
using namespace std;
#endif

struct StringLess {
	bool operator()(const string& s1, const string& s2) const {
		return s1 < s2;
	}
};

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
