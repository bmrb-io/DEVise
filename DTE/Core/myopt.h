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
  Revision 1.31  1997/08/25 15:28:14  donjerko
  Added minmax table

  Revision 1.30  1997/08/21 21:04:33  donjerko
  Implemented view materialization

  Revision 1.29  1997/08/14 02:08:55  donjerko
  Index catalog is now an independent file.

  Revision 1.28  1997/07/30 21:39:25  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.27  1997/07/22 15:00:56  donjerko
  *** empty log message ***

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
#include <string>
//#include <iostream.h>   erased for sysdep.h
//#include <fstream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
#include "queue.h"
#include "types.h"
#include "exception.h"
#include "url.h"
#include "listop.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

class ExecExpr;

typedef enum SelectID {
	BASE_ID, SELECT_ID, GLOBAL_ID, OP_ID, CONST_ID, PATH_ID, METHOD_ID, 
	CAST_ID, MEMBER_ID, ENUM_SELECT_ID
};

class BaseSelection{
protected:
public:
     BaseSelection() {}
	virtual ~BaseSelection(){}
	virtual void destroy(){}
	string toString(){
		ostringstream os;
		display(os);
		os << ends;
		string retVal(os.str());
		return retVal;
	}
     virtual void display(ostream& out, int detail = 0){
	};
	virtual void displayFlat(ostream& out, int detail = 0){
		display(out, detail);
	}
	virtual BaseSelection* filter(Site* site) = 0;
	virtual bool exclusive(Site* site) = 0;
	virtual bool exclusive(string* attributeNames, int numFlds) = 0;
	virtual string* siteName(){
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
     virtual ExecExpr* createExec(
		string site1, List<BaseSelection*>* list1,
		string site2, List<BaseSelection*>* list2){

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
	virtual void setSize(int size){
		assert(0);
	}
	virtual int getSize(){
		return 10;	// not used really
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
		string& attrName, string& opName, BaseSelection*& value){

		// Introduced to recognize the indexable predicates.
		// Only OperatorSelection can be indexable

		return false;
	}
	virtual string toStringAttOnly(){
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
	virtual bool exclusive(string* attributeNames, int numFlds){
		assert(!"not implemented");
		return false;
	}
	virtual BaseSelection* duplicate(){
		cerr << "Error: GlobalSelect::duplicate called\n";
		exit(1);
		return NULL ;
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		if(site == s){
			to->append(selection);
		}
	}
	virtual ExecExpr* createExec(
		string site1, List<BaseSelection*>* list1,
		string site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return GLOBAL_ID;
     }
	virtual TypeID typify(List<Site*>* sites);
	virtual TypeID getTypeID(){
		return typeID;
	}
     virtual void setTypeID(TypeID type){
		assert(0);
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
		return (result ? true : false);
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
	virtual bool exclusive(string* attributeNames, int numFlds){
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
		return (result ? true : false);
	}
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2);
     virtual TypeID typify(List<Site*>* sites){
		return typeID;
	}
     virtual TypeID getTypeID(){
          return typeID;
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
public:
	TypeCast(TypeID& typeID, BaseSelection* input, 
		PromotePtr promotePtr = NULL) : 
		BaseSelection(), typeID(typeID), input(input), 
		promotePtr(promotePtr){

		assert(input);
	}
	TypeCast(const TypeCast& x) {
		typeID = x.typeID;
		input = x.input->duplicate();
		promotePtr = x.promotePtr;
	}
	virtual ~TypeCast(){}
	void destroy(){
		input->destroy();
		delete input;
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
	virtual bool exclusive(string* attributeNames, int numFlds){
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
		if(!(typeID == y->typeID)){
			return false;
		}
		if(!input->match(y->input)){
			return false;
		}
		return true;
	}
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2);
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
	string* name;
	TypeID typeID;
	BaseSelection* input;
	MemberPtr memberPtr;
	int avgSize;	// to estimate result sizes
public:
	Member(string* name, BaseSelection* input) : 
		BaseSelection(), name(name), typeID(UNKN_TYPE), input(input), 
		memberPtr(NULL) {
	}
	Member(string* name, BaseSelection* input, MemberPtr memberPtr,
			TypeID typeID) : 
		BaseSelection(), name(name), typeID(typeID), input(input), 
		memberPtr(memberPtr) {
	}
	Member(const Member& x){
		name = new string(*x.name);
		typeID = x.typeID;
		input = x.input->duplicate();
		memberPtr = x.memberPtr;
		avgSize = x.avgSize;
	}
	virtual ~Member(){
		delete name;
	}
	void destroy(){
		input->destroy();
		delete input;
	}
	virtual void display(ostream& out, int detail = 0){
		input->display(out, detail);
		out << "." << *name;
		BaseSelection::display(out, detail);
	}
	const string* getName(){
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
	virtual bool exclusive(string* attributeNames, int numFlds){
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
          if(!(*name == *y->name)){
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
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2);
     virtual TypeID typify(List<Site*>* sites){
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
	virtual void setTypeID(TypeID type){
		typeID = type;
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
	Method(string* name, List<BaseSelection*>* args, BaseSelection* input) :
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
	virtual bool exclusive(string* attributeNames, int numFlds){
		assert(0);
		return input->exclusive(attributeNames, numFlds);
	}
	virtual BaseSelection* duplicate(){
		assert(0);
		return new Member(new string(*name), input->duplicate());
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
          if(!(*name == *y->name)){
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
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2){
		assert(!"methods not implemented yet");
		return NULL;
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
	virtual int getSize(){
		assert(0);
		return packSize(typeID);
	}
	virtual bool checkOrphan(){
		assert(0);
		return input->checkOrphan();
	}
};

class EnumSelection : public BaseSelection {
	TypeID typeID;
	int position;	// position of this selection in the input stream
public:
	EnumSelection(int position, TypeID typeID) :
		typeID(typeID), position(position) {}
	virtual TypeID getTypeID(){
		return typeID;
	}
	virtual TypeID typify(List<Site*>* sites){
		return typeID;
	}
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return ENUM_SELECT_ID;
     }
	virtual BaseSelection* filter(Site* site){
		assert(0);
		return NULL;
	}
	virtual bool exclusive(Site* site){
		assert(0);
		return false;
	}
	virtual bool exclusive(string* attributeNames, int numFlds){
		assert(0);
		return false;
	}
	virtual BaseSelection* duplicate(){
		assert(0);
		return NULL;
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
		assert(0);
	}
	virtual bool match(BaseSelection* x){
		assert(0);
		return false;
	}
};

class PrimeSelection : public BaseSelection{
	string* alias;
	string* fieldNm;
	TypeID typeID;
	int avgSize;	// to estimate result sizes
public:
	PrimeSelection(string a, string attr) : BaseSelection() {
		alias = new string(a);
		fieldNm = new string(attr);
		typeID = "Unknown";
		avgSize = 0;
	}
	PrimeSelection(
		string* a, string* fieldNm = NULL, TypeID typeID = "Unknown",
		int avgSize = 0): 
          BaseSelection(), alias(a), fieldNm(fieldNm), typeID(typeID), 
		avgSize(avgSize){}
	~PrimeSelection(){
		delete alias;
		delete fieldNm;
	}
	const string* getFieldNm(){
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
	virtual bool exclusive(string* attributeNames, int numFlds);
	virtual BaseSelection* duplicate(){
		string* dupAlias = (alias ? new string(*alias) : (string*) NULL);
		string* dupFieldNm = 
			(fieldNm ? new string(*fieldNm) : (string*) NULL);
		return new PrimeSelection(dupAlias, dupFieldNm);
	}
	virtual void collect(Site* s, List<BaseSelection*>* to){
	}
     virtual ExecExpr* createExec(
          string site1, List<BaseSelection*>* list1,
          string site2, List<BaseSelection*>* list2);
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
		string msg = "Table " + *alias + " is not listed in FROM clause";
		THROW(new Exception(msg), false);
	}
	virtual BaseSelection* distributeWrapper(Site* site){
		return new GlobalSelect(site, this); 
	}
	virtual string toStringAttOnly(){
		return *fieldNm;
	}
};

class Operator : public BaseSelection {
friend BaseSelection* distrib(BaseSelection* l, BaseSelection* r);
protected:
     string name;
     BaseSelection* left;
	BaseSelection* right;
	TypeID typeID;
	OperatorPtr opPtr;
	int avgSize;	// to estimate result sizes
	double selectivity;
public:
	Operator(string n, BaseSelection* l, BaseSelection* r)
		: BaseSelection(), name(n), left(l), right(r) {
		typeID = "Unknown";
		opPtr = NULL;
          assert(left);
          assert(right);
		avgSize = 0;
		selectivity = 0;
     }
	virtual ~Operator(){
	}
	void destroy(){
		left->destroy();
		delete left;
		right->destroy();
		delete right;
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
		string& attrName, string& opName, BaseSelection*& value);
	string getAttribute(){
		string attrNm;
		ostringstream os;
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
	string getValue(){
		ostringstream os;
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
		return os.str();
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
	virtual bool exclusive(string* attributeNames, int numFlds){
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
	virtual ExecExpr* createExec(
		string site1, List<BaseSelection*>* list1,
		string site2, List<BaseSelection*>* list2);
     virtual SelectID selectID(){
          return OP_ID;
     }
	virtual bool match(BaseSelection* x);
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

class ArithmeticOp : public Operator {
public:
	ArithmeticOp(string o, BaseSelection* l, BaseSelection* r)
		: Operator(o, l, r){}
	virtual BaseSelection* duplicate(){
		return new ArithmeticOp(name, left->duplicate(), right->duplicate());
	}
};

class BoolOperator : public Operator {
protected:
     BoolOperator(string s, BaseSelection* l, BaseSelection* r)
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

class TableName {
	List<string*>* tableName;
public:
	TableName() {
		tableName = new List<string*>;
	}
	TableName(List<string*>* tableName) : tableName(tableName) {}
	TableName(string* str){	// deletes str when done
		tableName = new List<string*>;
		tableName->append(str);
	}
	TableName(const char* path){
		int len = strlen(path);
		char* tmp = new char[len + 1];
		tmp[len] = '\0';
		tableName = new List<string*>;
		for(int i = len - 1; i >= 0; i--){
			if(path[i] == '.'){
				tableName->prepend(new string(&tmp[i + 1]));
				tmp[i] = '\0';
			}
			else{
				tmp[i] = path[i];
			}
		}
		cerr << "path " << path << " resolved as: ";
		displayList(cerr, tableName, ".");
		delete [] tmp;
	}
	TableName dirName();
	string fileName();
	~TableName(){
		delete tableName;
	}
	string* getFirst(){
		assert(tableName);
		assert(!tableName->isEmpty());
		tableName->rewind();
		return tableName->get();
	}
	void deleteFirst(){
		assert(tableName);
		assert(!tableName->isEmpty());
		tableName->rewind();
		string* firstPath = tableName->remove();
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
	string toString(){
		ostringstream tmp;
		tmp << ".";
		displayList(tmp, tableName, ".");
		tmp << ends;
		string retVal(tmp.str());
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
	string* alias;
	string *function;
	int shiftVal;
public:
	TableAlias(TableName *t, string* a = NULL,string *func = NULL,
			int optShiftVal = 0) : table(t), alias(a),function(func),
		shiftVal(optShiftVal) {}
	TableAlias(string tableNm, string aliasNm){
		table = new TableName(tableNm.c_str());
		alias = new string(aliasNm);
		function = NULL;
		shiftVal = 0;
	}
	virtual ~TableAlias(){}
	virtual TableName* getTable(){
		return table;
	}
	string* getAlias(){
		return alias;
	}
	string * getFunction(){
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
	virtual Site* createSite();
};

class QuoteAlias : public TableAlias {
	string* quote;
	Interface* interf;
public:
	QuoteAlias(string* quote, string* alias = NULL) :
		TableAlias(new TableName(), alias), quote(quote), interf(NULL) {}
	QuoteAlias(Interface* interf, string* alias) :
		TableAlias(new TableName(), alias), quote(NULL), interf(interf) {}
	virtual ~QuoteAlias();
	virtual void display(ostream& out, int detail = 0){
		if(quote){
			out << *quote;
		}
		if(alias){
			out << " " << *alias;
		}
	}
	virtual TableName* getTable(){

		// There is no table name for quoted tables, but 
		// the optimizer needs to know table name to find indexes

		return new TableName();
	}
	const string* getQuote(){
		return quote;
	}
	virtual bool isQuote(){
		return  true;
	}
	virtual Site* createSite();
};

#endif
