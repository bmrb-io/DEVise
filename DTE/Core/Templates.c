#include <iostream.h>
#include <string>
#include <vector>

#include "queue.h"
#include "Array.h"

#include "types.h"
#include "DateTime.h"
#include "ExecExpr.h"
#include "MemoryMgr.h"
#include "listop.h"
#include "myopt.h"

#include <stl.h>	// includes all of stl

class BaseSelection;
class Site;
class JoinTable;
class String;
class TableAlias;
class ConstantSelection;

template class MemoryLoaderTemplate<EncodedDTF>;
template class MemoryLoaderTemplate<IDouble>;

struct StringLess {
	bool operator()(const string& s1, const string& s2) const {
		return s1 < s2;
	}
};

template class List<BaseSelection*>;
template class List<Tuple*>;
template class List<Site*>;
template class List<JoinTable*>;
template class List<String*>;
template class List<TableAlias*>;
template class List<char*>;
template class List<ConstantSelection*>;
template class List<List<Tuple*>*>;
template class List<void**>;		// Necessary; can't find original use
template class List<List<void**>*>;	// Necessary; can't find original use
template class set<basic_string<char, string_char_traits<char> >, ltstr>;
template class List<basic_string<char, string_char_traits<char> > *>;
template class List<void*>;
template class List<void *const *>;
template class vector<BaseSelection*>;
template class vector<TableAlias*>;
template class Array<ExecExpr*>;
template class Array<void const *>;
template class map<basic_string<char, string_char_traits<char> >, BaseSelection*, StringLess>;
template class rb_tree<basic_string<char, string_char_traits<char> >, pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, select1st<pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, basic_string<char, string_char_traits<char> > >, StringLess>;

template class deque<void*>;

static void		template_junk1(void);

void template_junk1(void)
{
	set<string, ltstr>	tmp, tables, tables2;
	bool				incl = includes(tables.begin(), tables.end(),
										tmp.begin(), tmp.end());

	string		alias;

	tables.insert(alias);

	set_union(tables.begin(), tables.end(), 
		tables2.begin(), tables.end(), inserter(tables, tables.end()));

	Array<ExecExpr*>*	mySelect;

	destroyArray(*mySelect);
}

void junk(){
	vector<BaseSelection*> vec;
	vector<TableAlias*> vta;
	List<BaseSelection*>* list;
	List<TableAlias *>* lta;

	displayVec(cout, vec, string(", "));
	displayVec(cout, vta, string(", "));
	translate(list, vec);
	translate(vec, list);
	translate(vta, lta);
}
