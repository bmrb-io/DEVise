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
#include "Optimizer.h"
#include "ParseTree.h"

#include <stl.h>	// includes all of stl

class BaseSelection;
class Site;
class JoinTable;
class String;
class TableAlias;
class ConstantSelection;

template class MemoryLoaderTemplate<EncodedDTF>;
template class MemoryLoaderTemplate<IDouble>;
template class MemoryLoaderTemplate<ISeqSimVec>;

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
template class vector<float>;
template class vector<OptNode*>;
template class vector<TableAlias*>;
template class Array<ExecExpr*>;
template class Array<void const *>;
template class map<basic_string<char, string_char_traits<char> >, BaseSelection*, StringLess>;

// this template does not work with g++ 2.8

template class rb_tree<basic_string<char, string_char_traits<char> >, pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, select1st<pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, basic_string<char, string_char_traits<char> > >, StringLess>;

template class deque<void*>;
template class vector<LogicalProp>;
template class vector<TableMap>;
template class vector<AltEntry>;

template vector<pair<basic_string<char, string_char_traits<char> > *, basic_string<char, string_char_traits<char> > *> *>::push_back(pair<basic_string<char, string_char_traits<char> > *, basic_string<char, string_char_traits<char> > *> *const &);

template class vector<void const *const *>;
template class vector<int>;
template class vector<string>;
template vector<void const *const *>::push_back(void const *const *const &);
template vector<void const *const *>::vector(void);
template vector<void const *const *>::~vector(void);

template vector<pair<basic_string<char, string_char_traits<char> > *, basic_string<char, string_char_traits<char> > *> *>::vector(void);

template vector<pair<basic_string<char, string_char_traits<char> > *, basic_string<char, string_char_traits<char> > *> *>::~vector(void);

template pair<basic_string<char, string_char_traits<char> > *, basic_string<char, string_char_traits<char> > *>::pair(basic_string<char, string_char_traits<char> > *const &, basic_string<char, string_char_traits<char> > *const &);

template max(int const &, int const &);
template min(int const &, int const &);

static void template_junk1(void)
{
	AltEntry junk_alt_entry(0, (JoinMethod*) 0);
	vector<AltEntry> vae;
	make_heap(vae.begin(), vae.end(), AlternativeLess());
	sort_heap(vae.begin(), vae.end(), AlternativeLess());
	push_heap(vae.begin(), vae.end(), AlternativeLess());
	for_each(vae.begin(), vae.end(), printAlts);

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

typedef pair<BaseSelection*, Ordering> td1;

static void junk(){
	vector<BaseSelection*> vec;
	vector<TableAlias*> vta;
	List<BaseSelection*>* list;
	List<TableAlias *>* lta;

	displayVec(cout, vec, string(", "));
	displayVec(cout, vta, string(", "));
	translate(list, vec);
	translate(vec, list);
	translate(vta, lta);

	map<string, OptNode*, StringLess>::iterator it1;
	map<string, LogicalProp*, StringLess>::iterator it2;
	vector<TableAlias*>::const_reverse_iterator it3;
	*it3;
	it3 != it3;
	++it3;
	vector<td1> pair1;
	vector<SortCriterion> orderBy;
	const vector<SortCriterion>& orderBy2 = orderBy;
	vector<SortCriterion> orderBy3(orderBy2);
	vector<AccessMethod *> amj;
	vector<AccessMethod *> amj2(amj);
}

class OptNode;
class LogicalProp;

template class map<string, OptNode*, StringLess>;
template class map<string, LogicalProp*, StringLess>;
