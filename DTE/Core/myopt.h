/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.10  1996/12/18 00:55:58  donjerko
  Introduced the Operator* parent into the ExecOperator so that
  getTypeID work.

  Revision 1.9  1996/12/16 11:13:09  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.8  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

  Revision 1.7  1996/12/09 10:01:53  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.6  1996/12/07 15:14:28  donjerko
  Introduced new files to support indexes.

  Revision 1.5  1996/12/05 16:06:04  wenger
  Added standard Devise file headers.

 */

#ifndef MYOPT_H
#define MYOPT_H

#include <assert.h>
#include <String.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include "queue.h"
#include "types.h"
#include "exception.h"
#include "url.h"
#include "listop.h"

typedef enum SelectID {
	BASE_ID, SELECT_ID, GLOBAL_ID, OP_ID, CONST_ID, PATH_ID, METHOD_ID, 
	EXEC_ID,
};

class Path {
friend class GlobalSelect;
friend class BaseSelection;
friend class Operator;
friend class PrimeSelection;
protected:
	String* path;
	Path* nextPath;
public:
	Path(String* p, Path* n = NULL) : 
          path(p), nextPath(n) {}
	Path(Path &newPath):
		 path(newPath.path),nextPath(newPath.nextPath){}
	virtual void display(ostream& out, int detail = 0){
		assert(path);
		out << *path;
		if(nextPath){
			nextPath->display(out, detail);
		}
	}
	String toString(){
		ostrstream os;
		display(os);
		char* tmp = os.str();
		String retVal(tmp);
		delete tmp;
		return retVal;
	}
	virtual Path* filter(Site* site){
		if(nextPath){
			Path* nextRet = nextPath->filter(site);
			if(nextRet){
				return nextRet;
			}
			else{
				return this;
			}
		}
		else{
			return this;
		}
	}
	virtual BaseSelection * getSelectList(){

		assert(!"SelecList cannot be given by Path");
		return NULL;
	}

	virtual void propagate(Site* site){
		if(nextPath){
			nextPath->propagate(site);
		}
	}
	virtual bool exclusive(Site* site){
		if(nextPath){
			return nextPath->exclusive(site);
		}
		else{
			return true;
		}
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		assert(!"not implemented");
		return false;
	}
	virtual Path* duplicate(){
		if(nextPath){
			return new Path(new String(*path), nextPath->duplicate());
		}
		else{
			return new Path(new String(*path));
		}
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
     virtual void enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2){

          if(nextPath){
               TRY(nextPath->enumerate(site1, list1, site2, list2), );
          }
     }
     virtual SelectID selectID(){
          return PATH_ID;
     }
     virtual bool match(Path* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
          if(*path != *x->path){
               return false;
          }
          if(x->nextPath){
               if(nextPath){
				return nextPath->match(x->nextPath, upTo);
               }
               else{
                    return false;
               }
          }
          else{
               upTo = nextPath;
               return true;
          }
     }
	void append(Path* np){
		assert(nextPath == NULL);
		nextPath = np;
	}
	String* getPathName(){
		return path;
	}
	virtual bool isFunction(){
		return false;
	}
	virtual List<BaseSelection*>* getArgs(){
		return NULL;	// implemented for functions only;
	}
};

class BaseSelection{
protected:
	Path* nextPath;
public:
     BaseSelection(Path* np) : nextPath(np) {}
	String toString(){
		ostrstream os;
		display(os);
		os << ends;
		char* tmp = os.str();
		String retVal(tmp);
		delete tmp;
		return retVal;
	}
     virtual void display(ostream& out, int detail = 0){
          if(nextPath){
			out << ".";
               nextPath->display(out, detail);
          }
	};
	virtual void displayFlat(ostream& out, int detail = 0){
		display(out, detail);
	}
	virtual BaseSelection* filter(Site* site) = 0;
	virtual bool exclusive(Site* site) = 0;
	virtual bool exclusive(String* attributeNames, int numFlds) = 0;
	void append(Path* np){
		Path* next = nextPath;
		Path* prevPath = NULL;
		while(next){
			prevPath = next;
			next = next->nextPath;
		}
		if(prevPath){
			prevPath->nextPath = np;
		}
		else{
			nextPath = np;
		}
	}
	virtual String* siteName(){
		assert(0);
	}
	virtual BaseSelection* selectionF(){
		assert(0);
	}
	virtual BaseSelection* cnf(){
		return NULL;
	}
	virtual bool distribute(BaseSelection* bs){
		return false;
	}
	virtual ~BaseSelection(){}
	virtual BaseSelection* duplicate() = 0;
	virtual bool insertConj(List<BaseSelection*>* predList){
		predList->append(this);
		return false;
	}
	virtual void collect(Site* s, List<BaseSelection*>* to) = 0;
     virtual BaseSelection* enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2){

          if(nextPath){
               TRY(nextPath->enumerate(site1, list1, site2, list2), NULL);
          }
		return NULL;
     }
	virtual TypeID typify(List<Site*>* sites){
          if(nextPath){
			assert(!"not implemented");
               // TRY(nextPath->typify(sites), "Unknown");
          }
		return "Unknown";
     }
     virtual bool match(BaseSelection* x, Path*& upTo){
          if(x->nextPath){
               if(nextPath){
				return nextPath->match(x->nextPath, upTo);
               }
               else{
                    return false;
               }
          }
          else{
               upTo = nextPath;
               return true;
          }
     }
     virtual SelectID selectID() = 0;
	virtual TypeID getTypeID(){
		assert(0);
	}
	virtual Type* evaluate(Tuple* left, Tuple* right){
		assert(0);
	}
	virtual void setSize(int size){
		assert(0);
	}
	virtual int getSize(){
		assert(0);
	}
	virtual void setTypeID(TypeID type){
		assert(0);
	}
	virtual double getSelectivity(){
		assert(0);
	}
	virtual bool checkOrphan(){
		assert(0);
	}
	virtual Path* getNext(){
		return nextPath;
	}
	virtual BaseSelection* makeNonComposite(){
		
		// Used in the select * queries to make sure that all the
		// referncies to base tables are simple (eg., not a.i + 1).

		if(nextPath){
			// nextPath->makeNonComposite();
		}
		return NULL;
	}
	virtual BaseSelection* distributeWrapper(Site* site){

		// Used in the select * queries to make sure that all the
		// referncies to base tables are simple (eg., not a.i + 1).

		if(nextPath){
			// nextPath->distributeWrapper(site);
		}
		return NULL;
	}
	virtual bool isGlobal(){
		return false;
	}
	Path* getPath(){
		return nextPath;
	}
	virtual bool isIndexable(
		String& attrName, String& opName, BaseSelection*& value){

		// Introduced to recognize the indexable predicates.
		// Only OperatorSelection can be indexable

		return false;
	}
	virtual String toStringAttOnly(){
		cout << "toStringAttOnly not definded for: " << selectID() << endl;
		assert(0);
	}
};

class GlobalSelect : public BaseSelection {
	Site* site;
	BaseSelection* selection;
	TypeID typeID;
	int avgSize;	// to estimate result sizes
public:
	GlobalSelect( Site* s, BaseSelection* sel, Path* np) :
		site(s), selection(sel), BaseSelection(np) {
		typeID = sel->getTypeID();
		avgSize = sel->getSize();
	}
	virtual void display(ostream& out, int detail = 0);
	virtual void displayFlat(ostream& out, int detail = 0){
		selection->displayFlat(out, detail);
	}
	virtual BaseSelection* selectionF(){
		return selection;
	}
	virtual BaseSelection* filter(Site* siteGroup);
	virtual bool exclusive(Site* s);
	virtual bool exclusive(String* attributeNames, int numFlds){
		assert(!"not implemented");
		return false;
	}
	virtual BaseSelection* duplicate(){
		cerr << "Error: GlobalSelect::duplicate called\n";
		exit(1);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(site == s){
			to->append(selection);
		}
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
	virtual BaseSelection* enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return GLOBAL_ID;
     }
	virtual TypeID typify(List<Site*>* sites);
	virtual TypeID getTypeID(){
		return typeID;
	}
     virtual void setTypeID(TypeID type){
		typeID = type;
     }
     virtual bool match(BaseSelection* x, Path*& upTo);
	virtual void setSize(int size){
		avgSize = size;
	}
	virtual int getSize(){
		return avgSize;
	}
	virtual bool checkOrphan(){
		return true;
	}
	virtual BaseSelection* makeNonComposite(){
		return selection->distributeWrapper(site);
	}
	virtual BaseSelection* distributeWrapper(Site* site){
		assert(0);
	}
};
		
class ExecSelect : public BaseSelection {
     int leftRight;
	int fieldNo;
	BaseSelection* parent;
public:
	ExecSelect(BaseSelection* parent, int lr, int field, Path* next = NULL) :
		leftRight(lr), fieldNo(field), parent(parent),
		BaseSelection(next) {}
	virtual void display(ostream& out, int detail = 0){
		out << "(" << leftRight << ", " << fieldNo << ")";
		BaseSelection::display(out);
	}
	virtual void displayFlat(ostream& out, int detail = 0){
		parent->displayFlat(out, detail);
	}
	virtual BaseSelection* selectionF(){
		assert(0);
	}
	virtual BaseSelection* filter(Site* siteGroup){assert(0);}
	virtual bool exclusive(Site* s){assert(0);}
	virtual bool exclusive(String* attributeNames, int numFlds){assert(0);}
	virtual BaseSelection* duplicate(){
		assert(0);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		assert(0);
	}
     virtual SelectID selectID(){
		assert(0);
     }
	virtual TypeID getTypeID(){
		return parent->getTypeID();
	}
	virtual Type* evaluate(Tuple* left, Tuple* right){
		return (leftRight ? right[fieldNo] : left[fieldNo]);
	}
};
		
class ConstantSelection : public BaseSelection {
	TypeID typeID;
     Type* value;
public:
	ConstantSelection(TypeID typeID, Type* value) : 
		BaseSelection(NULL), typeID(typeID), value(value) {}
	int toBinary(char* to){
		marshal(value, to, typeID);
		return packSize(value, typeID);
	}
	int binarySize(){
		return packSize(value, typeID);
	}
	virtual bool operator <(ConstantSelection arg){
		assert(0);
	}
	virtual bool operator >(ConstantSelection arg){
		assert(0);
	}
	virtual bool operator ==(ConstantSelection arg){
		TypeID tmp;
		GeneralPtr* genPtr = getOperatorPtr("=", typeID, arg.typeID, tmp);
		assert(tmp == "bool");
		assert(genPtr && genPtr->opPtr);
		return ((IBool*) genPtr->opPtr(value, arg.value))->getValue();
	}
	virtual void display(ostream& out, int detail = 0){
		displayAs(out, value, typeID);
		BaseSelection::display(out, detail);
	}
	virtual BaseSelection* filter(Site* site){
		if(nextPath){
               nextPath->propagate(site);
          }
          return NULL;
	}
	virtual bool exclusive(Site* site){
		if(nextPath){
			return nextPath->exclusive(site);
		}
		else{
			return true;
		}
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		if(nextPath){
			return nextPath->exclusive(attributeNames, numFlds);
		}
		else{
			return true;
		}
	}
	virtual BaseSelection* duplicate(){
		return new ConstantSelection(typeID, value);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
     virtual SelectID selectID(){
          return CONST_ID;
     }
	virtual bool match(BaseSelection* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
		ConstantSelection* y = (ConstantSelection*) x;
		TypeID tmp;
		GeneralPtr* genPtr = getOperatorPtr("=", typeID, y->typeID, tmp);
		assert(tmp == "bool");
		assert(genPtr && genPtr->opPtr);
		if(!((IBool*) genPtr->opPtr(value, y->value))->getValue()){
			return false;
		}
		return BaseSelection::match(y, upTo);
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
          TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
		return duplicate();
     }
     virtual TypeID typify(List<Site*>* sites){
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
     }
	virtual Type* evaluate(Tuple* left, Tuple* right){
		return value;		// may need to duplicate
	}
	virtual int getSize(){
		return packSize(value, typeID);
	}
	virtual bool checkOrphan(){
		return true;
	}
};

class Operator : public BaseSelection {
friend BaseSelection* distrib(BaseSelection* l, BaseSelection* r);
protected:
     String name;
     BaseSelection* left;
	BaseSelection* right;
	TypeID typeID;
	OperatorPtr opPtr;
	int avgSize;	// to estimate result sizes
	double selectivity;
public:
	Operator(String n, BaseSelection* l, BaseSelection* r)
		: BaseSelection(NULL), name(n), left(l), right(r) {
		typeID = "Unknown";
		opPtr = NULL;
          assert(left);
          assert(right);
		avgSize = 0;
		selectivity = 0;
     }
	virtual void display(ostream& out, int detail = 0){
		out << "(";
		left->display(out); 
		out << " " << name << " ";
		right->display(out);
		out << ")";
		if(detail){
			out << "% size = " << avgSize;
			out << ", type = " << typeID << " %";
		}
		BaseSelection::display(out, detail);
	}
	virtual void displayFlat(ostream& out, int detail = 0){
		out << "(";
		left->displayFlat(out); 
		out << " " << name << " ";
		right->displayFlat(out);
		out << ")";
		// BaseSelection::displayFlat(out, detail);
	}
	virtual bool isIndexable(
		String& attrName, String& opName, BaseSelection*& value);
	String getAttribute(){
		ostrstream os;
		if(left->selectID() == SELECT_ID){
			left->getNext()->display(os);
		}
		else if(right->selectID() == SELECT_ID){
			right->getNext()->display(os);
		}
		else{
			assert(!"selection expected");
		}
		os << ends;
		return String(os.str());
	}
	String getValue(){
		ostrstream os;
		if(left->selectID() == SELECT_ID){
			right->display(os);
		}
		else if(right->selectID() == SELECT_ID){
			left->display(os);
		}
		else{
			assert(!"selection expected");
		}
		os << ends;
		return String(os.str());
	}
	virtual BaseSelection* filter(Site* site){
		Path* endPath = NULL;
		Path* nextGlobal = NULL;
		if(exclusive(site)){
			if(nextPath){
				endPath = nextPath->filter(site);
			}
			if(endPath){
				nextGlobal = endPath->nextPath;
				endPath->nextPath = NULL;
			}
			return new GlobalSelect(site, this, nextGlobal);
		}
		else{
			BaseSelection* leftG = left->filter(site);
			BaseSelection* rightG = right->filter(site);
			if(leftG){
				left = leftG;
			}
			if(rightG){
				right = rightG;
			}
			if(nextPath){
				nextPath->propagate(site);
			}
			return NULL;
		}
	}
	virtual bool exclusive(Site* site){
		if(!left->exclusive(site)){
			return false;
		}
		else if(!right->exclusive(site)){
			return false;
		}
		else if(nextPath){
			return nextPath->exclusive(site);
		}
		else{
			return true;
		}
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		if(!left->exclusive(attributeNames, numFlds)){
			return false;
		}
		else if(!right->exclusive(attributeNames, numFlds)){
			return false;
		}
		else if(nextPath){
			return nextPath->exclusive(attributeNames, numFlds);
		}
		else{
			return true;
		}
	}
	virtual BaseSelection* duplicate(){
		return new Operator(name, left->duplicate(), right->duplicate());
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		left->collect(s, to);
		right->collect(s, to);
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
	virtual BaseSelection* enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return OP_ID;
     }
	virtual bool match(BaseSelection* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
		Operator* y = (Operator*) x;
		if(name != y->name){
			return false;
		}
		if(!left->match(y->left, upTo)){
			return false;
		}
		if(!right->match(y->right, upTo)){
			return false;
		}
		return BaseSelection::match(y, upTo);
	}
	virtual TypeID typify(List<Site*>* sites){
          TRY(left->typify(sites), "");
          TRY(right->typify(sites), "");
          TypeID root = left->getTypeID();
          TypeID arg = right->getTypeID();
		GeneralPtr* genPtr;
          genPtr = getOperatorPtr(name, root, arg, typeID);
          if(!genPtr){
			String msg = "No operator " + name + "(" + root + ", " +
				arg + ") defined";
			THROW(new Exception(msg), "Unknown");
          }
		opPtr = genPtr->opPtr;
		avgSize = genPtr->sizePtr(left->getSize(), right->getSize());
		if(typeID == "bool"){
			SelectyPtr selectyPtr = genPtr->selectyPtr;
			if(!selectyPtr){
				String msg = "Undefined selectiviy for operator " + name;
				THROW(new Exception(msg), "Unknown");
			}
			else{
				selectivity = selectyPtr(left, right);
			}
		}
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
     }
	virtual int getSize(){
		return avgSize;
	}
	virtual double getSelectivity(){
		return selectivity;
	}
	virtual bool checkOrphan(){
		TRY(left->checkOrphan(), false);
		TRY(right->checkOrphan(), false);
		return true;
	}
	virtual BaseSelection* makeNonComposite(){
		BaseSelection* leftG = left->makeNonComposite();
		BaseSelection* rightG = right->makeNonComposite();
		if(leftG){
			assert(left != leftG);
			delete left;	// shallow 
			left = leftG;
		}
		if(rightG){
			assert(right != rightG);
			delete right;	// shallow
			right = rightG;
		}
		return BaseSelection::makeNonComposite();
	}
	virtual BaseSelection* distributeWrapper(Site* site);
};

class ExecOperator : public Operator{
     OperatorPtr opPtr;
	Operator* parent;
public:
	ExecOperator(String n, BaseSelection* l, BaseSelection* r, 
		OperatorPtr opPtr, Operator* parent) :
		Operator(n, l, r), opPtr(opPtr), parent(parent) {}
	virtual void display(ostream& out, int detail = 0){
		assert(parent);
		parent->display(out);
		if(detail){
			Operator::display(out, 0);
		}
	}
	virtual void displayFlat(ostream& out, int detail = 0){
		parent->displayFlat(out, detail);
	}
     virtual TypeID getTypeID(){
          return parent->getTypeID();
     }
	virtual Type* evaluate(Tuple* leftT, Tuple* rightT){
		Type* arg1 = left->evaluate(leftT, rightT);
		Type* arg2 = right->evaluate(leftT, rightT);
		return opPtr(arg1, arg2);
	}
};

class Method : public Path {
	List<BaseSelection*>* args; 
public:
	Method(String* p, List<BaseSelection*>* a, Path* n = NULL)
		: Path(p, n), args(a) {}
	virtual void display(ostream& out, int detail = 0){
		assert(path);
		out << *path << '(';
		if(args){
			displayList(out, args, ", ", detail);
		}
		out << ')';
		if(nextPath){
			nextPath->display(out, detail);
		}
	}
	virtual Path* filter(Site* site){
		if(args){
			if(exclusiveF(args, site)){
				return Path::filter(site);
			}
			else{
				filterList(args, site);
				Path::propagate(site);
				return NULL;
			}
		}
		else{
			return Path::filter(site);
		}
	}
	virtual void propagate(Site* site){
		if(args){
			filterList(args, site);
		}
		Path::propagate(site);
	}
	virtual bool exclusive(Site* site){
		if(args){
			if(exclusiveF(args, site)){
				return Path::exclusive(site);
			}
			else{
				return false;
			}
		}
		else{
			return Path::exclusive(site);
		}
	}
	virtual Path* duplicate(){
		Path* newNext;
		newNext = nextPath ? nextPath->duplicate() : (Path*)NULL;
		List<BaseSelection*>* newArgs;
		newArgs = args ? duplicateF(args) : (List<BaseSelection*>*)NULL;
		return new Method(new String(*path), newArgs, newNext);
	}
     virtual void enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){

          if(args){
               TRY(enumerateList(args, site1, list1, site2, list2), );
          }
          TRY(Path::enumerate(site1, list1, site2, list2), );
     }
     virtual SelectID selectID(){
          return METHOD_ID;
     }
     virtual bool match(Path* x, Path*& upTo){
		// check for args equality
		assert(!"Method execution not supported yet");
		return false;
     }
	virtual bool isFunction(){
		return true;
	}
	virtual List<BaseSelection*>* getArgs(){
		return args;
	}
	virtual BaseSelection * getSelectList(){

		// Need to check if this is fine..??
		// Get the nextpath from arg 0 of the sel list
		String *dummy = new String;

		args->rewind();
		return args->get();

	}

};

class ArithmeticOp : public Operator {
public:
	ArithmeticOp(String o, BaseSelection* l, BaseSelection* r)
		: Operator(o, l, r){}
	virtual BaseSelection* duplicate(){
		return new ArithmeticOp(name, left->duplicate(), right->duplicate());
	}
};

class BoolOperator : public Operator {
protected:
     BoolOperator(String s, BaseSelection* l, BaseSelection* r)
          : Operator(s, l, r) {}
	void propagateCnf(){
		BaseSelection* newLeft = left->cnf();
		BaseSelection* newRight = right->cnf();
		if(newRight){
			delete right;		// shallow
			right = newRight;
		}
		if(newLeft){
			delete left;		// shallow
			left = newLeft;
		}
     }
};

class OrOperator : public BoolOperator {
public:
	OrOperator(BaseSelection* l, BaseSelection* r)
          : BoolOperator("or", l, r){}
	virtual BaseSelection* cnf(){
		propagateCnf();
		if(right->distribute(left)){
			return right;
		}
		else if(left->distribute(right)){
			return left;
		}
		else{
			return NULL;
		}
     }
	virtual BaseSelection* duplicate(){
		return new OrOperator(left->duplicate(), right->duplicate());
	}
};

class AndOperator : public BoolOperator {
public:
	AndOperator(BaseSelection* l, BaseSelection* r)
          : BoolOperator("and", l, r){}
	virtual BaseSelection* cnf(){
		propagateCnf();
          return NULL;
     }
	virtual bool distribute(BaseSelection* bs){
		left = new OrOperator(bs, left);
		right = new OrOperator(bs->duplicate(), right);
		propagateCnf();
		return true;
	}
	virtual BaseSelection* duplicate(){
		return new AndOperator(left->duplicate(), right->duplicate());
	}
	virtual bool insertConj(List<BaseSelection*>* predList){
		if(left->insertConj(predList)){
			delete left;	// shallow
		}
		if(right->insertConj(predList)){
			delete right;	// shallow
		}
		return true;
	}
};

class PrimeSelection : public BaseSelection{
	String* alias;
	TypeID typeID;
	int avgSize;	// to estimate result sizes
	int position;  // position of this selection in a tuple
public:
	PrimeSelection(String* a, Path* n = NULL, TypeID typeID = "Unknown",
		int avgSize = 0, int position = 0): 
          BaseSelection(n), alias(a), typeID(typeID), avgSize(avgSize),
		position(position) {}
     virtual void display(ostream& out, int detail = 0){
		if(alias){
			out << *alias;
		}
		BaseSelection::display(out, detail);
		if(detail){
               out << "% size = " << avgSize;
               out << ", type = " << typeID << " %";
          }
	}
	virtual BaseSelection* filter(Site* site);
	virtual bool exclusive(Site* site);
	virtual bool exclusive(String* attributeNames, int numFlds);
	virtual BaseSelection* duplicate(){
		String* dupAlias = (alias ? new String(*alias) : (String*) NULL);
		if(nextPath){
			return new PrimeSelection(dupAlias, nextPath->duplicate());
		}
		else{
			return new PrimeSelection(dupAlias);
		}
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
		assert(site2 == "" && list2 == NULL);
		int leftRight = 0;
		List<BaseSelection*>* selList = list1;
		assert(selList);
		selList->rewind();
		int i = 0;
		while(!selList->atEnd()){
               Path* upTo = NULL;
			if(match(selList->get(), upTo)){
				BaseSelection* retVal;
				// assert(i == position); failed for RTreeIndex
                    retVal = new ExecSelect(this, leftRight, i, upTo);
				TRY(BaseSelection::enumerate(site1, list1, site2, list2),
					NULL);
				return retVal;
			}
			selList->step();
			i++;
		}
		String msg = "Table " + *alias + " does not have attribute " +
			*nextPath->path;
		THROW(new Exception(msg), NULL);
	}
	virtual TypeID typify(List<Site*>* sites);
     virtual SelectID selectID(){
          return SELECT_ID;
     }
	virtual bool match(BaseSelection* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
		PrimeSelection* y = (PrimeSelection*) x;
		assert(y->alias);
		assert(alias);
		if(*alias != *y->alias){
			return false;
		}
		return BaseSelection::match(y, upTo);
	}
	virtual TypeID getTypeID(){
		return typeID;
	}
     virtual void setTypeID(TypeID type){
		typeID = type;
     }
	virtual void setSize(int size){
		avgSize = size;
	}
	virtual int getSize(){
		return avgSize;
	}
	virtual bool checkOrphan(){
		assert(alias);
		String msg = "Table " + *alias + " is not listed in FROM clause";
		THROW(new Exception(msg), false);
	}
	virtual BaseSelection* distributeWrapper(Site* site){
		return new GlobalSelect(site, this, NULL); 
	}
	virtual bool isGlobal(){
		return *alias == "";
	}
	virtual String toStringAttOnly(){
		return nextPath->toString();
	}
};

class TableName {
	List<String*>* tableName;
public:
	TableName() {
		tableName = new List<String*>;
	}
	TableName(List<String*>* tableName) : tableName(tableName) {}
	TableName(String* str){	// delete str when done
		tableName = new List<String*>;
		tableName->append(str);
	}
	~TableName(){
		delete tableName;
	}
	String* getFirst(){
		assert(tableName);
		assert(!tableName->isEmpty());
		tableName->rewind();
		return tableName->get();
	}
	void deleteFirst(){
		assert(tableName);
		assert(!tableName->isEmpty());
		tableName->rewind();
		String* firstPath = tableName->remove();
		delete firstPath;
	}
	bool isEmpty(){
		assert(tableName);
		return tableName->isEmpty();
	}
	void display(ostream& out){
		assert(tableName);
		out << "/";
		displayList(out, tableName, "/");
	}
	int cardinality(){
		assert(tableName);
		return tableName->cardinality();
	}
};

class TableAlias {
protected:
	TableName* table;
	String* alias;
public:
	TableAlias(TableName* t, String* a = NULL) : 
		table(t), alias(a) {
	}
	virtual TableName* getTable(){
		return table;
	}
	String* getAlias(){
		return alias;
	}
	virtual void display(ostream& out, int detail = 0){
		assert(table);
		table->display(out);
		if(alias){
			out << " as " << *alias;
		}
	}
	virtual bool isQuote(){
		return  false;
	}
};

class QuoteAlias : public TableAlias {
	String* quote;
public:
	QuoteAlias(String* quote, String* alias = NULL) :
		TableAlias(new TableName(), alias), quote(quote) {}
	virtual void display(ostream& out, int detail = 0){
		assert(quote);
		out << *quote;
		if(alias){
			out << " " << *alias;
		}
	}
	virtual TableName* getTable(){
		assert(0);
	}
	String* getQuote(){
		return quote;
	}
	virtual bool isQuote(){
		return  true;
	}
};

#endif
