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
	BASE_ID, SELECT_ID, GLOBAL_ID, OP_ID, INT_ID, PATH_ID, METHOD_ID, 
	EXEC_ID, STRING_ID
};

class Path {
friend class GlobalSelect;
friend class BaseSelection;
friend class IntegerConstant;
friend class Operator;
friend class PrimeSelection;
protected:
	String* path;
	Path* nextPath;
public:
	Path(String* p, Path* n = NULL) : 
          path(p), nextPath(n) {}
	virtual void display(ostream& out, int detail = 0){
		assert(path);
		out << *path;
		if(nextPath){
			nextPath->display(out, detail);
		}
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
public:
	ExecSelect(int lr, int field, Path* next = NULL) :
		leftRight(lr), fieldNo(field), 
		BaseSelection(next) {}
	virtual void display(ostream& out, int detail = 0){
		out << "(" << leftRight << ", " << fieldNo << ")";
		BaseSelection::display(out);
	}
	virtual BaseSelection* selectionF(){
		assert(0);
	}
	virtual BaseSelection* filter(Site* siteGroup){assert(0);}
	virtual bool exclusive(Site* s){assert(0);}
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
		assert(0);
	}
	virtual Type* evaluate(Tuple* left, Tuple* right){
		return (leftRight ? right[fieldNo] : left[fieldNo]);
	}
};
		
class IntegerConstant : public BaseSelection {
     int value;
public:
	IntegerConstant(int i) : BaseSelection(NULL), value(i) {}
	virtual void display(ostream& out, int detail = 0){
		out << value;
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
	virtual BaseSelection* duplicate(){
		return new IntegerConstant(value);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
     virtual SelectID selectID(){
          return INT_ID;
     }
	virtual bool match(BaseSelection* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
		IntegerConstant* y = (IntegerConstant*) x;
		if(value != y->value){
			return false;
		}
		return BaseSelection::match(y, upTo);
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
          TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
		return new IntegerConstant(value);
     }
     virtual TypeID typify(List<Site*>* sites){
		return "int";
	}
     virtual TypeID getTypeID(){
          return "int";
     }
	virtual Type* evaluate(Tuple* left, Tuple* right){
		return new IInt(value);
	}
	virtual int getSize(){
		return 4;
	}
	virtual bool checkOrphan(){
		return true;
	}
};

class StringConstant : public BaseSelection {
     String* value;
public:
	StringConstant(String* i) : BaseSelection(NULL), value(i) {}
	virtual void display(ostream& out, int detail = 0){
		out << *value;
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
	virtual BaseSelection* duplicate(){
		String* newvalue = new String(*value);
		return new StringConstant(newvalue);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(nextPath){
			nextPath->collect(s, to);
		}
	}
     virtual SelectID selectID(){
          return STRING_ID;
     }
	virtual bool match(BaseSelection* x, Path*& upTo){
          if(!(selectID() == x->selectID())){
               return false;
          }
		StringConstant* y = (StringConstant*) x;
		if(*value != *(y->value)){
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
		return "string";
	}
     virtual TypeID getTypeID(){
          return "string";
     }
	virtual Type* evaluate(Tuple* left, Tuple* right){
		return new IString((char*) value->chars());
	}
	virtual int getSize(){
		return value->length();
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
		if(typeID == "IBool"){
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
public:
	ExecOperator(String n, BaseSelection* l, BaseSelection* r, 
		OperatorPtr opPtr) :
		Operator(n, l, r), opPtr(opPtr) {}
	virtual void display(ostream& out, int detail = 0){
          Operator::display(out, 0);
	}
     virtual TypeID getTypeID(){
          return typeID;
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
		int avgSize = 0) : 
          BaseSelection(n), alias(a), typeID(typeID), avgSize(avgSize) {}
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
				assert(i == position);
                    retVal = new ExecSelect(leftRight, i, upTo);
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
};

class TableAlias {
	bool _isQuote;
public:
	String* table;
	String* alias;
	TableAlias(String* t, String* a = NULL, bool isQuote = false) : 
		_isQuote(isQuote), table(t), alias(a) {}
	void display(ostream& out, int detail = 0){
		assert(table);
		out << *table;
		if(alias){
			out << " " << *alias;
		}
	}
	bool isQuote(){
		return  _isQuote;
	}
};

#endif
