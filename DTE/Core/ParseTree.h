#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "catalog.h"

Catalog* getRootCatalog();

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
	List<BaseSelection*>* groupBy;
	List<String*>* namesToResolve;
	void resolveNames();	// throws exception
public:	
	QueryTree(
		List<BaseSelection*>* selectList,
		List<TableAlias*>* tableList,
		BaseSelection* predicates,
		String *sequencebyTable,
		BaseSelection* withPredicate,
		List<BaseSelection *>*groupBy,
		List<String*>* namesToResolve) :
		selectList(selectList), tableList(tableList), 
		predicates(predicates), sequencebyTable(sequencebyTable),
		withPredicate(withPredicate),groupBy(groupBy),
		namesToResolve(namesToResolve) {}
	
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

class InsertParse : public ParseTree {
	TableName* tableName;
	List<ConstantSelection*>* fieldList;
public:	
	InsertParse(
		List<String*>* tableName,
		List<ConstantSelection*>* fieldList) :
		tableName(new TableName(tableName)),
		fieldList(fieldList) {}
	virtual Site* createSite();	// throws exception
	virtual ~InsertParse(){
		delete tableName;	// destroy too
		delete fieldList;	// destroy too
	}
};

class DeleteParse : public ParseTree {
	TableName* tableName;
	String* alias;
	BaseSelection* predicate;
public:	
	DeleteParse(
		List<String*>* tableName, String* alias,
		BaseSelection* predicate) :
		tableName(new TableName(tableName)),
		alias(alias), predicate(predicate) {}
	virtual Site* createSite();	// throws exception
	virtual ~DeleteParse(){
		delete tableName;	// destroy too
		delete predicate;	// destroy too
	}
};

class SchemaParse : public ParseTree {
	TableName* tableName;
public:
	SchemaParse(List<String*>* tableName) :
		tableName(new TableName(tableName)) {}
	virtual Site* createSite();	// throws exception
	virtual ~SchemaParse(){
		delete tableName;	// destroy too
	}
};

#endif
