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
  Revision 1.26  1997/06/27 23:17:21  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.25  1997/06/16 16:04:52  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.24  1997/04/18 20:46:19  donjerko
  Added function pointers to marshall types.

  Revision 1.23  1997/04/14 20:44:14  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.22  1997/04/10 21:50:29  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.21  1997/03/25 17:57:41  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.20  1997/03/23 23:45:23  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.19  1997/03/20 20:42:23  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.18  1997/03/14 18:36:13  donjerko
  Making space for the SQL UNION operator.

  Revision 1.17  1997/02/25 22:14:53  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.16  1997/02/21 01:38:07  donjerko
  Fixed some problems with "group by" clause.

  Revision 1.15  1997/02/18 18:06:05  donjerko
  Added skeleton files for sorting.

  Revision 1.14  1997/02/03 04:11:35  donjerko
  Catalog management moved to DTE

  Revision 1.13.4.1  1997/03/18 14:41:20  wenger
  Various minor changes to get 1.3 to compile on SGI.

  Revision 1.13  1996/12/26 03:42:01  kmurli
  MOdified to make joinprev work right

  Revision 1.12  1996/12/24 21:00:53  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.11  1996/12/21 22:21:50  donjerko
  Added hierarchical namespace.

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
	EXEC_ID, CAST_ID, MEMBER_ID
};

class BaseSelection{
protected:
public:
     BaseSelection() {}
	virtual ~BaseSelection(){}
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
	};
	virtual void displayFlat(ostream& out, int detail = 0){
		display(out, detail);
	}
	virtual BaseSelection* filter(Site* site) = 0;
	virtual bool exclusive(Site* site) = 0;
	virtual bool exclusive(String* attributeNames, int numFlds) = 0;
	virtual String* siteName(){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual BaseSelection* selectionF(){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual BaseSelection* cnf(){
		return NULL;
	}
	virtual bool distribute(BaseSelection* bs){
		return false;
	}
	virtual BaseSelection* duplicate() = 0;
	virtual bool insertConj(List<BaseSelection*>* predList){
		predList->append(this);
		return false;
	}
	virtual void collect(Site* s, List<BaseSelection*>* to) = 0;
     virtual BaseSelection* enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2){

		return NULL;
     }
	virtual TypeID typify(List<Site*>* sites){
		return "Unknown";
     }
     virtual bool match(BaseSelection* x) = 0;
	virtual bool matchNoMember(BaseSelection* x){ // throws exception
		return match(x);
	}
     virtual SelectID selectID() = 0;
	virtual TypeID getTypeID(){
		assert(0);
		return ""; // avoid compiler warning
	}
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual void setSize(int size){
		assert(0);
	}
	virtual int getSize(){
		assert(0);
		return 0; // avoid compiler warning
	}
	virtual void setTypeID(TypeID type){
		assert(0);
	}
	virtual double getSelectivity(){
		assert(0);
		return 0.0; // avoid compiler warning
	}
	virtual bool checkOrphan(){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual BaseSelection* makeNonComposite(){
		
		// Used in the select * queries to make sure that all the
		// referncies to base tables are simple (eg., not a.i + 1).

		return NULL;
	}
	virtual BaseSelection* distributeWrapper(Site* site){

		// Used in the select * queries to make sure that all the
		// referncies to base tables are simple (eg., not a.i + 1).

		return NULL;
	}
	virtual bool isGlobal(){
		return false;
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
		return ""; // avoid compiler warning
	}
     virtual bool matchFlat(BaseSelection* x){
		return match(x);
	}
};

class GlobalSelect : public BaseSelection {
	Site* site;
	BaseSelection* selection;
	TypeID typeID;
	int avgSize;	// to estimate result sizes
public:
	GlobalSelect(Site* s, BaseSelection* sel) :
		site(s), selection(sel), BaseSelection() {
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
     virtual bool match(BaseSelection* x);
     virtual bool matchFlat(BaseSelection* x){
		return selection->matchFlat(x);
	}
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
		return NULL; // avoid compiler warning
	}
};
		
class ExecSelect : public BaseSelection {
     int leftRight;
	int fieldNo;
	BaseSelection* parent;
public:
	ExecSelect(BaseSelection* parent, int lr, int field) :
		leftRight(lr), fieldNo(field), parent(parent),
		BaseSelection() {}
	virtual void display(ostream& out, int detail = 0){
		out << "(" << leftRight << ", " << fieldNo << ")";
		BaseSelection::display(out);
	}
	virtual void displayFlat(ostream& out, int detail = 0){
		assert(0);
		parent->displayFlat(out, detail);
	}
	virtual BaseSelection* selectionF(){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual BaseSelection* filter(Site* siteGroup){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual bool exclusive(Site* s){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual BaseSelection* duplicate(){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		assert(0);
	}
     virtual SelectID selectID(){
		assert(0);
		return BASE_ID; // avoid compiler warning
     }
	virtual TypeID getTypeID(){
		assert(0);
		return parent->getTypeID();
	}
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		const Type* base = (leftRight ? right[fieldNo] : left[fieldNo]);
		return base;
	}
     virtual bool match(BaseSelection* x){
		assert(0);
		return false;
	}
};
		
class ConstantSelection : public BaseSelection {
	TypeID typeID;
     Type* value;
public:
	ConstantSelection(TypeID typeID, Type* val) : 
		BaseSelection(), typeID(typeID), value(val) {}
	virtual ~ConstantSelection(){
		delete value;	// needs destroy ptr
	}
	ConstantSelection* promote(TypeID typeToPromote) const; // throws
	int toBinary(char* to){
		MarshalPtr marshalPtr = getMarshalPtr(typeID);
		marshalPtr(value, to);
		return packSize(value, typeID);
	}
	int binarySize(){
		return packSize(value, typeID);
	}
	virtual bool operator <(ConstantSelection arg){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual bool operator >(ConstantSelection arg){
		assert(0);
		return NULL; // avoid compiler warning
	}
	virtual bool operator ==(ConstantSelection arg){ // throws exception
		TypeID tmp;
		GeneralPtr* genPtr;
		TRY(genPtr = getOperatorPtr("=", typeID, arg.typeID, tmp), false);
		assert(tmp == "bool");
		assert(genPtr && genPtr->opPtr);
		Type* result;
		genPtr->opPtr(value, arg.value, result);
		return bool(result);
	}
	virtual void display(ostream& out, int detail = 0){
		WritePtr wp = getWritePtr(typeID);
		assert(wp);
		wp(out, value);
		BaseSelection::display(out, detail);
	}
	virtual BaseSelection* filter(Site* site){
          return NULL;
	}
	virtual bool exclusive(Site* site){
		return true;
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		return true;
	}
	virtual BaseSelection* duplicate();
	virtual void collect(Site* s, List<BaseSelection*>* to){
	}
     virtual SelectID selectID(){
          return CONST_ID;
     }
	virtual bool match(BaseSelection* x){ // throws exception
          if(!(selectID() == x->selectID())){
               return false;
          }
		ConstantSelection* y = (ConstantSelection*) x;
		TypeID tmp;
		GeneralPtr* genPtr;
		TRY(genPtr = getOperatorPtr("=", typeID, y->typeID, tmp), false);
		assert(tmp == "bool");
		Type* result;
		assert(genPtr && genPtr->opPtr);
		genPtr->opPtr(value, y->value, result);
		return bool(result);
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
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		return value;
	}
	virtual int getSize(){
		return packSize(value, typeID);
	}
	virtual bool checkOrphan(){
		return true;
	}
};

class TypeCast : public BaseSelection {
	TypeID typeID;
	BaseSelection* input;
	PromotePtr promotePtr;
	Type* value;
	size_t valueSize;
public:
	TypeCast(TypeID& typeID, BaseSelection* input, 
		PromotePtr promotePtr = NULL) : 
		BaseSelection(), typeID(typeID), input(input), 
		promotePtr(promotePtr){

		value = allocateSpace(typeID, valueSize);
	}
	TypeCast(const TypeCast& x) {
		typeID = x.typeID;
		input = x.input->duplicate();
		promotePtr = x.promotePtr;
		value = allocateSpace(typeID, valueSize);
	}
	virtual ~TypeCast(){
		delete value;
	}
	virtual void display(ostream& out, int detail = 0){
		out << typeID << "(";
		input->display(out, detail);
		out << ")";
		BaseSelection::display(out, detail);
	}
	virtual BaseSelection* filter(Site* site){
		if(input->exclusive(site)){
			return new GlobalSelect(site, this);
		}
		else{
			BaseSelection* newInp = input->filter(site);
			assert(!newInp);
			return NULL;
		}
	}
	virtual bool exclusive(Site* site){
		return input->exclusive(site);
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		return input->exclusive(attributeNames, numFlds);
	}
	virtual BaseSelection* duplicate(){
		return new TypeCast(*this);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
	}
     virtual SelectID selectID(){
          return CAST_ID;
     }
	virtual bool match(BaseSelection* x){ // throws exception
          if(!(selectID() == x->selectID())){
               return false;
          }
		TypeCast* y = (TypeCast*) x;
		if(typeID != y->typeID){
			return false;
		}
		if(!input->match(y->input)){
			return false;
		}
		return true;
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
		BaseSelection* execInp;
		TRY(execInp = input->enumerate(site1, list1, site2, list2), NULL);
          TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
		return new TypeCast(typeID, execInp, promotePtr);
     }
     virtual TypeID typify(List<Site*>* sites){
		if(promotePtr){

			// already typified when consturcted 

			return typeID;
		}
		TRY(TypeID inpType = input->typify(sites), "");
		TRY(PromotePtr promotePtr = getPromotePtr(inpType, typeID), "");
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
     }
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		promotePtr(input->evaluate(left, right), value);
		return value;
	}
	virtual int getSize(){
		return packSize(typeID);
	}
	virtual bool checkOrphan(){
		return input->checkOrphan();
	}
};

class Member : public BaseSelection {
friend class Method;
protected:
	String* name;
	TypeID typeID;
	BaseSelection* input;
	MemberPtr memberPtr;
	int avgSize;	// to estimate result sizes
	Type* value;
	size_t valueSize;
public:
	Member(String* name, BaseSelection* input) : 
		BaseSelection(), name(name), typeID(UNKN_TYPE), input(input), 
		memberPtr(NULL) {

		value = NULL;
	}
	Member(String* name, BaseSelection* input, MemberPtr memberPtr,
			TypeID typeID) : 
		BaseSelection(), name(name), typeID(typeID), input(input), 
		memberPtr(memberPtr) {

		value = allocateSpace(typeID, valueSize);
	}
	Member(const Member& x){
		name = new String(*x.name);
		typeID = x.typeID;
		input = x.input->duplicate();
		memberPtr = x.memberPtr;
		avgSize = x.avgSize;
		value = NULL;	// assuming that copy constructor is not called
		// after typify
	}
	virtual ~Member(){
		delete value;
	}
	virtual void display(ostream& out, int detail = 0){
		input->display(out, detail);
		out << "." << *name;
		BaseSelection::display(out, detail);
	}
	const String* getName(){
		return name;
	}
	virtual BaseSelection* filter(Site* site){
		if(input->exclusive(site)){
			return new GlobalSelect(site, this);
		}
		else{
			BaseSelection* newInp = input->filter(site);
			assert(!newInp);
			return NULL;
		}
	}
	virtual bool exclusive(Site* site){
		return input->exclusive(site);
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		return input->exclusive(attributeNames, numFlds);
	}
	virtual BaseSelection* duplicate(){
		return new Member(*this);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		input->collect(s, to);
	}
     virtual SelectID selectID(){
          return MEMBER_ID;
     }
	virtual bool match(BaseSelection* x){ // throws exception
          if(!(selectID() == x->selectID())){
               return false;
          }
		Member* y = (Member*) x;
          if(*name != *y->name){
               return false;
          }
		if(!input->match(y->input)){
			return false;
		}
		return true;
	}
	virtual bool matchNoMember(BaseSelection* x){ // throws exception
		if(match(x)){
			return true;
		}
		return input->matchNoMember(x);
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
		BaseSelection* execInp;
		TRY(execInp = input->enumerate(site1, list1, site2, list2), NULL);
          TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
		return new Member(new String(*name), execInp, memberPtr, typeID);
     }
     virtual TypeID typify(List<Site*>* sites){
		TRY(TypeID parentType = input->typify(sites), "unknown");
		GeneralMemberPtr* genPtr;
		TRY(genPtr = getMemberPtr(*name, parentType, typeID), "unknown");
		assert(genPtr);
		memberPtr = genPtr->memberPtr;
		assert(memberPtr);
		value = allocateSpace(typeID, valueSize);
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
     }
	virtual void setTypeID(TypeID type){
		typeID = type;
	}
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		memberPtr(input->evaluate(left, right), value);
		return value;
	}
	virtual int getSize(){
		return avgSize;
	}
     virtual void setSize(int size){
          avgSize = size;
     }
	virtual bool checkOrphan(){
		return input->checkOrphan();
	}
};

class Method : public Member {
	List<BaseSelection*>* args;
public:
	Method(String* name, List<BaseSelection*>* args, BaseSelection* input) :
		Member(name, input), args(args) {}
	virtual ~Method(){
	}
	virtual void display(ostream& out, int detail = 0){
		if(input){
			input->display(out, detail);
			out << ".";
		}
		assert(name);
		out << *name << "(";
          if(args){
               displayList(out, args, ", ", detail);
          }
          out << ')';
	}
     virtual bool isGlobal(){
          return input == NULL;
     }
     List<BaseSelection*>* getArgs(){
          return args;
     }
	virtual BaseSelection* filter(Site* site){
		assert(0);
		if(input->exclusive(site)){
			return new GlobalSelect(site, this);
		}
		else{
			BaseSelection* newInp = input->filter(site);
			assert(!newInp);
			return NULL;
		}
	}
	virtual bool exclusive(Site* site){
		assert(0);
		return input->exclusive(site);
	}
	virtual bool exclusive(String* attributeNames, int numFlds){
		assert(0);
		return input->exclusive(attributeNames, numFlds);
	}
	virtual BaseSelection* duplicate(){
		assert(0);
		return new Member(new String(*name), input->duplicate());
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		assert(0);
		input->collect(s, to);
	}
     virtual SelectID selectID(){
          return METHOD_ID;
     }
	virtual bool match(BaseSelection* x){ // throws exception
		assert(0);
          if(!(selectID() == x->selectID())){
               return false;
          }
		Member* y = (Member*) x;
          if(*name != *y->name){
               return false;
          }
		if(!input->match(y->input)){
			return false;
		}
		return true;
	}
	virtual bool matchNoMember(BaseSelection* x){ // throws exception
		assert(0);
		if(match(x)){
			return true;
		}
		return input->matchNoMember(x);
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2){
		assert(0);
		BaseSelection* execInp;
		TRY(execInp = input->enumerate(site1, list1, site2, list2), NULL);
          TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
		return new Member(new String(*name), execInp, memberPtr, typeID);
     }
     virtual TypeID typify(List<Site*>* sites){
		assert(0);
		TRY(TypeID parentType = input->typify(sites), "unknown");
		GeneralMemberPtr* genPtr;
		TRY(genPtr = getMemberPtr(*name, parentType, typeID), "unknown");
		assert(genPtr);
		memberPtr = genPtr->memberPtr;
		assert(memberPtr);
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
     }
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		assert(0);
		return NULL;
	}
	virtual int getSize(){
		assert(0);
		return packSize(typeID);
	}
	virtual bool checkOrphan(){
		assert(0);
		return input->checkOrphan();
	}
};

class PrimeSelection : public BaseSelection{
	String* alias;
	String* fieldNm;
	TypeID typeID;
	int avgSize;	// to estimate result sizes
	int position;  // position of this selection in a tuple
public:
	PrimeSelection(String a, String attr) : BaseSelection() {
		alias = new String(a);
		fieldNm = new String(attr);
		typeID = "Unknown";
		avgSize = 0;
		position = 0;
	}
	PrimeSelection(
		String* a, String* fieldNm = NULL, TypeID typeID = "Unknown",
		int avgSize = 0, int position = 0): 
          BaseSelection(), alias(a), fieldNm(fieldNm), typeID(typeID), 
		avgSize(avgSize),
		position(position) {}
	const String* getFieldNm(){
		return fieldNm;
	}
     virtual void display(ostream& out, int detail = 0){
		if(alias){
			out << *alias;
		}
		if(fieldNm){
			out << "." << *fieldNm;
		}
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
		String* dupFieldNm = 
			(fieldNm ? new String(*fieldNm) : (String*) NULL);
		return new PrimeSelection(dupAlias, dupFieldNm);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
	}
     virtual BaseSelection* enumerate(
          String site1, List<BaseSelection*>* list1,
          String site2, List<BaseSelection*>* list2);
	virtual TypeID typify(List<Site*>* sites);
     virtual SelectID selectID(){
          return SELECT_ID;
     }
	virtual bool match(BaseSelection* x);
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
		return new GlobalSelect(site, this); 
	}
	virtual String toStringAttOnly(){
		return *fieldNm;
	}
	virtual const Type* evaluate(const Tuple* left, const Tuple* right){
		assert(!"Evaluate called on PrimeSelection");
		return NULL;
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
		: BaseSelection(), name(n), left(l), right(r) {
		typeID = "Unknown";
		opPtr = NULL;
          assert(left);
          assert(right);
		avgSize = 0;
		selectivity = 0;
     }
	virtual ~Operator(){
//		delete left;
//		delete right;
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
		String attrNm;
		ostrstream os;
		if(left->selectID() == SELECT_ID){
			attrNm = *(((PrimeSelection*) left)->getFieldNm());
		}
		else if(right->selectID() == SELECT_ID){
			attrNm = *(((PrimeSelection*) right)->getFieldNm());
		}
		else{
			assert(!"selection expected");
		}
		return attrNm;
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
		if(exclusive(site)){
			return new GlobalSelect(site, this);
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
	}
	virtual BaseSelection* enumerate(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return OP_ID;
     }
	virtual bool match(BaseSelection* x){
          if(!(selectID() == x->selectID())){
               return false;
          }
		Operator* y = (Operator*) x;
		if(name != y->name){
			return false;
		}
		if(!left->match(y->left)){
			return false;
		}
		if(!right->match(y->right)){
			return false;
		}
		return true;
	}
	virtual TypeID typify(List<Site*>* sites);	// throws
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
	Type* value;
	size_t valueSize;
public:
	ExecOperator(String n, BaseSelection* l, BaseSelection* r, 
		OperatorPtr opPtr, Operator* parent) :
		Operator(n, l, r), opPtr(opPtr), parent(parent) {
		
		value = allocateSpace(getTypeID(), valueSize);
	}
	~ExecOperator(){
		delete value;
	}
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
	virtual const Type* evaluate(const Tuple* leftT, const Tuple* rightT){
		const Type* arg1 = left->evaluate(leftT, rightT);
		const Type* arg2 = right->evaluate(leftT, rightT);
		opPtr(arg1, arg2, value, valueSize);
		return value;
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

const int MAX_PATH_LEN = 10;

class TableName {
	List<String*>* tableName;
public:
	TableName() {
		tableName = new List<String*>;
	}
	TableName(List<String*>* tableName) : tableName(tableName) {}
	TableName(String* str){	// deletes str when done
		tableName = new List<String*>;
		tableName->append(str);
	}
	TableName(const char* path){
		String tmp(path);
		String* res = new String[MAX_PATH_LEN];
		int len = split(tmp, res, MAX_PATH_LEN, String(".")); 
		assert(len < MAX_PATH_LEN);
		tableName = new List<String*>;
		for(int i = 1; i < len; i++){
			tableName->append(new String(res[i]));
		}
		delete [] res;
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
	bool isEmpty() const {
		assert(tableName);
		return tableName->isEmpty();
	}
	void display(ostream& out){
		assert(tableName);
		out << ".";
		displayList(out, tableName, ".");
	}
	String toString(){
		ostrstream tmp;
		tmp << ".";
		displayList(tmp, tableName, ".");
		tmp << ends;
		char* tmps = tmp.str();
		String retVal(tmps);
		// delete tmps; // should use free instead?
		return retVal;
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
	String *function;
	int shiftVal;
public:
	TableAlias(TableName *t, String* a = NULL,String *func = NULL,
			int optShiftVal = 0) : table(t), alias(a),function(func),
		shiftVal(optShiftVal) {}
	TableAlias(String tableNm, String aliasNm){
		table = new TableName(tableNm.chars());
		alias = new String(aliasNm);
		function = NULL;
		shiftVal = 0;
	}
	virtual ~TableAlias(){}
	virtual TableName* getTable(){
		return table;
	}
	String* getAlias(){
		return alias;
	}
	String * getFunction(){
		return function;
	}
	int getShiftVal(){
		return shiftVal;
	}
	virtual void display(ostream& out, int detail = 0){
		assert(table);
		if (function ){
			out << *function << " (" ;
			table->display(out) ;
			cout << ") " ;
		}
		else{
			table->display(out);
		}
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
	virtual ~QuoteAlias(){
		delete quote;
	}
	virtual void display(ostream& out, int detail = 0){
		assert(quote);
		out << *quote;
		if(alias){
			out << " " << *alias;
		}
	}
	virtual TableName* getTable(){

		// There is no table name for quoted tables, but 
		// the optimizer needs to know table name to find indexes

		return new TableName();
	}
	const String* getQuote(){
		return quote;
	}
	virtual bool isQuote(){
		return  true;
	}
};

#endif
