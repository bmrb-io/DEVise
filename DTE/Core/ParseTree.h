#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "queue.h"
#include "myopt.h"
#include "site.h"

class ParseTree {
public:
	virtual Site* createSite() = 0;	// throws exception
};

class QueryTree : public ParseTree {
	List<BaseSelection*>* selectList;
	List<TableAlias*>* tableList;
	BaseSelection* predicates;
	String* sequencebyTable;
	BaseSelection* withPredicate;
	List<String*>* namesToResolve;
	void resolveNames();	// throws exception
public:	
	QueryTree(
		List<BaseSelection*>* selectList,
		List<TableAlias*>* tableList,
		BaseSelection* predicates,
		String *sequencebyTable,
		BaseSelection* withPredicate,
		List<String*>* namesToResolve) :
		selectList(selectList), tableList(tableList), 
		predicates(predicates), sequencebyTable(sequencebyTable),
		withPredicate(withPredicate),namesToResolve(namesToResolve) {}
	
	virtual Site* createSite();	// throws exception
	virtual ~QueryTree(){
		delete selectList;      // destroy list too
		delete tableList;

		// predicates should be deleted in createSite
	}
	
};

class IndexParse : public ParseTree {
	String* indexName;
	TableName* tableName;
	List<BaseSelection*>* attributeList;
	List<String*>* namesToResolve;
	void resolveNames();	// throws exception
public:	
	IndexParse(
		String* indexName,
		List<String*>* tableName,
		List<BaseSelection*>* attributeList,
		List<String*>* namesToResolve) :
		indexName(indexName), tableName(new TableName(tableName)),
		attributeList(attributeList), namesToResolve(namesToResolve) {}
	virtual Site* createSite();	// throws exception
	virtual ~IndexParse(){
		delete indexName;
		delete tableName;	// destroy too
		delete attributeList;	// destroy too
		delete namesToResolve;	// destroy too
	}
};

#endif
