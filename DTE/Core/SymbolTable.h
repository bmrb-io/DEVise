#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "myopt.h"
#include <string>
#include <map.h>

struct StringLess {
	bool operator()(const string& s1, const string& s2) const {
		return s1 < s2;
	}
};

// typedef int BaseSelection;

class SymbolTable {
	map<string, BaseSelection*, StringLess> symtab;
public:
	void put(string key, BaseSelection* element){
		symtab[key] = element;
	}
	BaseSelection* get(string key){
		return symtab[key];
	}
	~SymbolTable(){
		map<string, BaseSelection*, StringLess>::iterator it;
		for(it = symtab.begin(); it != symtab.end(); it++){
			delete (*it).second;
		}
	}
};

/**************** 	test

main(){
	SymbolTable st;
	BaseSelection* bs;
	bs = st.get("junk");
	cerr << "Lookup for junk should be null: " << bs << endl;
	bs = new int(1);
	st.put("junk", bs);
	bs = st.get("junk");
	cerr << "Lookup for junk should be 1: " << *bs << endl;
}

*/

#endif
