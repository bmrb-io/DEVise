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
  Revision 1.44  1999/01/20 22:46:33  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.43  1998/12/28 21:32:26  donjerko
  Optimizer is now used by default.

  Revision 1.42  1998/11/23 19:18:56  donjerko
  Added support for gestalts

  Revision 1.41  1998/10/08 22:27:13  donjerko
  *** empty log message ***

  Revision 1.40  1998/07/24 04:38:00  donjerko
  *** empty log message ***

  Revision 1.39  1998/06/28 21:47:46  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.38  1998/06/24 22:14:08  donjerko
  *** empty log message ***

  Revision 1.37  1998/06/17 20:53:14  donjerko
  *** empty log message ***

  Revision 1.36  1998/06/04 23:26:28  donjerko
  *** empty log message ***

  Revision 1.35  1998/04/09 20:26:24  donjerko
  *** empty log message ***

  Revision 1.34  1998/03/17 17:19:09  donjerko
  Added new namespace management through relation ids.

  Revision 1.33  1998/03/12 18:23:33  donjerko
  *** empty log message ***

  Revision 1.32  1997/12/10 00:00:37  okan
  ODBC Interface Changes ...

  Revision 1.31  1997/12/04 04:05:21  donjerko
  *** empty log message ***

  Revision 1.30  1997/11/23 21:23:33  donjerko
  Added ODBC stuff.

  Revision 1.29  1997/11/13 22:19:25  okan
  Changes about compilation on NT

  Revision 1.28  1997/11/05 00:19:48  donjerko
  Separated typechecking from optimization.

  Revision 1.27  1997/10/02 02:27:33  donjerko
  Implementing moving aggregates.

  Revision 1.26  1997/09/29 02:51:58  donjerko
  Eliminated class GlobalSelect.

  Revision 1.25  1997/09/17 02:35:49  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.24  1997/09/05 22:20:19  donjerko
  Made changes for port to NT.

  Revision 1.23  1997/08/25 15:28:15  donjerko
  Added minmax table

  Revision 1.22  1997/08/21 21:04:34  donjerko
  Implemented view materialization

  Revision 1.21  1997/08/14 02:08:56  donjerko
  Index catalog is now an independent file.

  Revision 1.20  1997/08/12 19:58:45  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.19  1997/07/30 21:39:26  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.18  1997/07/22 15:00:57  donjerko
  *** empty log message ***

  Revision 1.17  1997/06/27 23:17:22  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.16  1997/06/16 16:04:53  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.15  1997/04/14 20:44:15  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.14  1997/04/10 21:50:29  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.13  1997/03/25 17:57:41  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.12  1997/03/20 20:42:24  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.11  1997/02/25 22:14:54  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.10  1997/02/18 18:06:05  donjerko
  Added skeleton files for sorting.

  Revision 1.9  1997/02/03 04:11:36  donjerko
  Catalog management moved to DTE

  Revision 1.8.4.2  1997/03/25 15:41:08  wenger
  Slight change to resolve.c to get it to compile on SPARC/SunOS.

  Revision 1.8.4.1  1997/03/18 14:41:20  wenger
  Various minor changes to get 1.3 to compile on SGI.

  Revision 1.8  1996/12/21 22:21:50  donjerko
  Added hierarchical namespace.

  Revision 1.7  1996/12/18 00:55:59  donjerko
  Introduced the Operator* parent into the ExecOperator so that
  getTypeID work.

  Revision 1.6  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

  Revision 1.5  1996/12/07 15:14:29  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/04 22:09:54  donjerko
  Removed the IntegerConstant and StringConstant and replaced them with
  ConstantSelection

  Revision 1.3  1996/12/03 23:51:02  donjerko
  The begining of the support for indexes, also a minor bug fix for select *.

  Revision 1.2  1996/11/26 02:01:48  donjerko
  Added the files that will perform aggregate functions

  Revision 1.1  1996/11/23 02:00:44  donjerko
  Restructered DTE directory

  Revision 1.3  1996/11/21 16:21:26  wenger
  Updated dependencies; fixed some compile errors and warnings.

  Revision 1.2  1996/11/19 15:31:44  wenger
  Various fixes to try to get DTE to compile on HP, Sun, etc.

 */

#include "types.h"
#include "myopt.h"
//#include "site.h"
#include "ExecExpr.h"
#include "catalog.h" 	// for root catalog
#include "Interface.h"
//#include "listop.h"
#include "RelationManager.h"
#include "AccessMethod.h"        // for getAccessMethods
#include "DteSymbolTable.h"
#include "Aggregates.h"
#include "DTE/mql/MqlSession.h"


//---------------------------------------------------------------------------


ExecExpr* OptExpr::createExec(const OptExprListList& inputs) const
{
  //cerr << "searching for " << toString() << " in:\n";
  //cerr << "tableMap: " << tableMap << endl;
  assert(inputs.size() <= 2);
  for(int i = 0; i < (int)inputs.size(); i++) {
    for(int j = 0; j < (int)inputs[i].size(); j++) {
      OptExpr* curr = inputs[i][j];
      assert(curr);
      //cerr << curr->toString();
      if(match(curr)) {
        //cerr << "found\n";
        return ExecExpr::createField(getAdt(), i, j);
      }
    }
  }
  //cerr << "not found\n";
  return 0;
}

//---------------------------------------------------------------------------


OptConstant::~OptConstant()
{
  adt->deallocate(value);
}


bool OptConstant::typeCheck(const DteSymbolTable& symbols)
{
  return true;
}


void OptConstant::display(ostream& out, int detail = 0) const
{
  adt->print(out, value);
  OptExpr::display(out, detail);
}


OptExpr* OptConstant::duplicate()
{
  return new OptConstant(adt, adt->allocateCopy(value));
}


void OptConstant::collect(TableMap group, OptExprList& to)
{
  // constants are only placed on top-level queries,
  // and so they are never collected
}


OptExpr::ExprType OptConstant::getExprType() const
{
  return CONST_ID;
}


bool OptConstant::match(const OptExpr* x) const
{
  if( getExprType() != x->getExprType() ) return false;
  //if( !adt->isExactSameType(x->getAdt()) ) return false;
  if( adt->getTypeID() != x->getAdt().getTypeID() ) return false;
  if( !adt->canCompare() ) return false;
  return( adt->compare(value, ((OptConstant*)x)->value) == 0 );
}


ExecExpr* OptConstant::createExec(const OptExprListList& inputs) const
{
  return ExecExpr::createConstant(*adt, adt->allocateCopy(value));
}


//---------------------------------------------------------------------------

OptVariable::OptVariable(const string& varName)
  : varName(varName)
{
}


OptVariable::~OptVariable()
{
}


bool OptVariable::typeCheck(const DteSymbolTable& symbols)
{
  assert(adt == NULL);
  OptConstant* value = mqlSession.findVariable(varName);
  if( value == NULL ) {
    cerr << "variable not found: $" << varName << endl;
    return false;
  }
  adt = value->getAdt().clone();
  return true;
}


void OptVariable::display(ostream& out, int detail = 0) const
{
  out << '$' << varName;
  OptExpr::display(out, detail);
}


OptExpr* OptVariable::duplicate()
{
  return new OptVariable(varName);
}


void OptVariable::collect(TableMap group, OptExprList& to)
{
}


OptExpr::ExprType OptVariable::getExprType() const
{
  return VARIABLE_ID;
}


bool OptVariable::match(const OptExpr* x) const
{
  if( getExprType() != x->getExprType() ) return false;
  return varName == ((OptVariable*)x)->varName;
}


ExecExpr* OptVariable::createExec(const OptExprListList& inputs) const
{
  return ExecExpr::createVariable(varName);
}


//---------------------------------------------------------------------------


OptField::~OptField()
{
}


void OptField::display(ostream& out, int detail = 0) const
{
  if(alias != "") {
    out << alias << '.';
  }
  out << '"' << fieldNm << '"';
  OptExpr::display(out, detail);
}


OptExpr* OptField::duplicate()
{
  return new OptField(alias, fieldNm);
}


bool OptField::typeCheck(const DteSymbolTable& symbols)
{
  assert(adt == NULL);
  if( symbols.findAttr(alias, fieldNm, tableId, slot, adt) ) {
    tableMap.set(tableId);
    return true;
  }
  return false;
}

OptExpr::ExprType OptField::getExprType() const
{
  return FIELD_ID;
}

string OptField::toStringAttOnly() const
{
  return fieldNm;
}

// bool OptField::exclusive(string* attributeNames, int numFlds)
// {
//   string me = toStringAttOnly();
//   for(int i = 0; i < numFlds; i++) {
//     // cout << "attr = " << attributeNames[i] << endl;
//     if(me == attributeNames[i]) {
//       return true;
//     }
//   }
//   // cout << "me = " << me << endl;
//   return false;
// }


bool OptField::match(const OptExpr* x) const {
	assert(x);
	if( getExprType() != x->getExprType() ) {
		return false;
	}
	OptField* y = (OptField*) x;
	if( alias != y->alias) {
		return false;
	}
	if(fieldNm != y->fieldNm) {
		return false;
	}
	return true;
}


ExecExpr* OptField::createExec(const OptExprListList& inputs) const
{
  if( inputs.size() > 0 ) {
    // this is the normal path that is used by the optimizer.
    ExecExpr* retVal = OptExpr::createExec(inputs);
    if(retVal){
      return retVal;
    }
    string msg = "Table " + alias + " does not have attribute " + fieldNm;
    THROW(new Exception(msg), NULL);
  }
  // no inputs, so assume input will be a scan of this field's table.
  // This is used by DeleteParse to avoid creating OptNodes and collect(), etc.
  assert(slot >= 0);
  return ExecExpr::createField(getAdt(), tableId, slot);
}


void OptField::collect(TableMap group, OptExprList& to)
{
  if(containedIn(group)) {
    to.push_back(this);
  }
}


// TableMap OptField::setTableMap(const vector<TableAlias*>& x) {
// 	int pos = 1;
// 	bool found = false;
// 	vector<TableAlias*>::const_iterator it;
// 	for(it = x.begin(); it != x.end(); ++it){
// 		if(alias == (*it)->getAlias()){
// 			found = true;
// 			break;
// 		}
// 		pos <<= 1;
// 	}
//         cerr << "found table as pos: " << pos << endl;
// 	assert(found);
// 	tableMap = TableMap(pos);
// 	return tableMap;
// }


//---------------------------------------------------------------------------


OptFunction::~OptFunction()
{
  delete_all(args);
}


void OptFunction::display(ostream& out, int detail = 0) const
{
  out << name << "(";
  print_ptr_list(out, args);
  out << ')';
  OptExpr::display(out, detail);
}

OptExpr* OptFunction::duplicate()
{
  int n = args.size();
  OptExprList copyArgs(n);
  for(int i = 0 ; i < n ; i++) {
    copyArgs[i] = args[i]->duplicate();
  }
  return new OptFunction(name, copyArgs);
}


OptExpr::ExprType OptFunction::getExprType() const
{
  return FUNCTION_ID;
}


bool OptFunction::match(const OptExpr* x) const
{
  if( getExprType() != x->getExprType() ) {
    return false;
  }
  OptFunction* y = (OptFunction*)x;
  if( name != y->name ) {
    return false;
  }
  int n = args.size();
  if( n != (int)((y->args).size()) ) return false;
  for(int i = 0 ; i < n ; i++) {
    if( ! args[i]->match(y->args[i]) ) {
      return false;
    }
  }
  return true;
}

bool OptFunction::typeCheck(const DteSymbolTable& symbols)
{
  // typecheck all the function arguments
  int n = args.size();
  DteAdtList argTypes(n);
  for(int i = 0 ; i < n ; i++) {
    if( !args[i]->typeCheck(symbols) ) {
      return false;
    }
    argTypes[i] = args[i]->getAdt();
    tableMap |= args[i]->getTableMap();
  }
  // all the arguments are ok

  // see if the function is an aggregate
  if( n == 1 ) { // aggs can only have one arg (for now at least)
    adt = ExecAgg::typeCheck(name, *(argTypes[0]));
    if( adt ) {
      aggFn = true;
      return true;
    }
  }

  // not an aggregate, find a regular function
  adt = ExecExpr::typeCheck(name, argTypes);
  if( adt ) {
    aggFn = false;
    return true;
  }
  return false;
}


bool OptFunction::isIndexable(string& attrName, string& opName,
                                  OptExpr*& value)
{
  ExprType ls = args[0]->getExprType();
  ExprType rs = args[1]->getExprType();

  if( ls == FIELD_ID && rs == CONST_ID ) {

    attrName = ((OptField*)args[0])->getFieldNm();
    value = args[1];

  } else if( rs == FIELD_ID && ls == CONST_ID ) {

    attrName = ((OptField*)args[1])->getFieldNm();
    value = args[0];

  } else {

    return false;

  }
  opName = name;
  return true;
}


double OptFunction::getSelectivity() {
  return 0.1;
#if 0
  //kb: this code didn't really handle Operators right before I made
  //kb: a bunch of changes, so I didn't bother making converting it
  //kb: for Functions...
	if(typeID == "bool"){
		/*
		SelectyPtr selectyPtr = genPtr->selectyPtr;
		if(!selectyPtr){
			string msg = "Undefined selectiviy for operator " + name;
			THROW(new Exception(msg), "Unknown");
		}
		else{
			selectivity = selectyPtr(left, right);
		}
		*/

		if(right->getExprType() == FIELD_ID){
			
			// foreign key relationship, left matches to exactly one right
			// selectivity = 1 / cardinalityOfRightTable

			OptField* rightc = (OptField*) right;
			const TableAlias* rightTable = rightc->getTable();
			assert(rightTable);
			Cardinality c = rightTable->getStats()->getCardinality();
			if(!c){
				return 1;
			}
			return 1.0 / c; 
		}
		else{
			return 0.1;	// fix later
		}
	}
	else{
		assert(0);
		return 0;
	}
#endif
}


ExecExpr* OptFunction::createExec(const OptExprListList& inputs) const
{
  // cerr << "creating exec for OptFunction: " << toString() << endl;
  ExecExpr* retVal = OptExpr::createExec(inputs);
  if(retVal) {
    return retVal;
  }
  int numFlds = args.size();
  ExecExprList execArgs;
  for(int i = 0; i < numFlds; i++) {
    TRY(execArgs.push_back(args[i]->createExec(inputs)), NULL);
    //kb: should delete execArgs on error
  }
  return ExecExpr::createFunction(name, execArgs);
}


void OptFunction::collect(TableMap group, OptExprList& to)
{
  if(containedIn(group)) {
    to.push_back(this);
  } else {
    for(size_t i = 0 ; i < args.size() ; i++) {
      args[i]->collect(group, to);
    }
  }
}

//---------------------------------------------------------------------------

void OptBool::propagateCnf()
{
  OptExpr* newExpr = args[0]->cnf();
  if( newExpr ) {
    delete args[0];
    args[0] = newExpr;
  }
  newExpr = args[1]->cnf();
  if( newExpr ) {
    delete args[1];
    args[1] = newExpr;
  }
}

OptExpr* OptOr::cnf()
{
  propagateCnf();
  OptExpr* result = NULL;
  if(args[0]->distribute(args[1])) {
    result = args[0];
    args.clear();      // children are no longer mine
  }
  else if(args[1]->distribute(args[0])) {
      result = args[1];
      args.clear();      // children are no longer mine
  }
  return result;
}

OptExpr* OptOr::duplicate()
{
  return new OptOr(args[0]->duplicate(), args[1]->duplicate());
}


OptExpr* OptAnd::cnf()
{
  propagateCnf();
  return NULL;
}

bool OptAnd::distribute(OptExpr* bs)
{
  args[0] = new OptOr(bs, args[0]);
  args[1] = new OptOr(bs->duplicate(), args[1]);
  propagateCnf();
  return true;
}

OptExpr* OptAnd::duplicate()
{
  return new OptAnd(args[0]->duplicate(), args[1]->duplicate());
}

bool OptAnd::insertConj(OptExprList& predList)
{
  if(args[0]->insertConj(predList)) { // left was an AND op, so delete it
    delete args[0];
  }
  if(args[1]->insertConj(predList)) { // right was an AND op; delete it
    delete args[1];
  }
  args.clear();               // children are no longer mine
  return true;                // tell caller to delete me
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


// TableMap OptFunction::setTableMap(const vector<TableAlias*>& x)
// {
//   TableMap tmap;
//   int n = args.size();
//   for(int i = 0 ; i < n ; i++) {
//     tmap |= args[i]->setTableMap(x);
//   }
//   cerr << "new function tablemap for " << toString() << " = " << tmap << endl;
//   tableMap = tmap;
//   return tableMap;
// }


//===========================================================================

TableAlias::TableAlias(Interface* interface, const string& a)
: table(), alias(), stats(0), schema(), interf(0)
{
  //cerr << "TableAlias(interface(";
  //interface->write(cerr);
  //cerr << "), " << a << ")\n";
  alias = a;
  init(interface);
}

TableAlias::TableAlias(const TableName& t, const string& a)
: table(), alias(), stats(0), schema(), interf(0)
{
  //cerr << "TableAlias(" << t.toString() << ',' << a << ")\n";
  table = t;
  alias = a;
  interf = ROOT_CATALOG.createInterface(table);
  if( !interf ) {
    cerr << "Table " << t.toString() << " not found";
    //string msg = "Table " + t.toString() + " not found";
    //THROW(new Exception(msg), );
  } else {
    init(interf);
  }
}

TableAlias::~TableAlias()
{
        delete_all(accessMethods);
	delete stats;
	delete interf;
}

void TableAlias::display(ostream& out, int detail){
  table.display(out);
  out << " as " << alias;
}

void TableAlias::init(Interface* interface)
{
  assert(interface);
  interf = interface;
  const ISchema* s = interf->getISchema();
  assert(s);
  schema = *s;
  const Stats* tempStats = interf->getStats();
  if(tempStats) {
    stats = new Stats(*tempStats);
  } else {
    stats = new Stats(schema.getNumFields());
  }
  accessMethods = interf->createAccessMethods();
  isGestaltM = interf->isGestalt();
  isRemoteM = interf->isRemote();
}

void NumberAlias::display(ostream& out, int detail){
  out << relId << " as " << alias;
}

NumberAlias::~NumberAlias()
{
}




//kb: delete old Site stuff
#if 0
ExecExpr* OptExpr::createExec(Site* site1, Site* site2)
{
  OptExprList selList;
  int leftRight = 0;
  if(site1 && exclusive(site1)) {
    selList = site1->getSelectList();
    leftRight = 0;
  }
  else if(site2 && exclusive(site2)) {
    selList = site2->getSelectList();
    leftRight = 1;
  }
  else {
    return NULL;
  }
  selList->rewind();
  int i = 0;
  while(!selList->atEnd()){
    OptExpr* curr = selList->get();
    assert(curr);
    if(match(curr)) {
      return new ExecSelect(curr->getTypeID(), leftRight, i);
    }
    selList->step();
    i++;
  }
  return NULL;
}


bool OptField::exclusive(Site* site)
{
  assert(alias);
  return site->have(*alias);
}


ExecExpr* OptField::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = OptExpr::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	string msg = "Table " + *alias + " does not have attribute " +
		*fieldNm;
	THROW(new Exception(msg), NULL);
}


void OptField::collect(Site* site, List<OptExpr*>* to)
{
  if(exclusive(site)) {
    to->append(this);
  }
}


ExecExpr* OptFunction::createExec(Site* site1, Site* site2)
{
  ExecExpr* retVal = OptExpr::createExec(site1, site2);
  if(retVal) {
    return retVal;
  }
  int numFlds = args->size();
  ExprList* execArgs = new ExprList;
  for(int i = 0; i < numFlds; i++) {
    TRY(execArgs->push_back(args[i]->createExec(site1, site2)), NULL);
  }
  return new ExecFunction(name, execArgs);
}

void OptFunction::collect(Site* site, List<OptExpr*>* to)
{
  if(exclusive(site)) {
    to->append(this);
  }
  else{
    assert(args);
    int n = args.size();
    for(int i = 0 ; i < n ; i++) {
      OptExpr* curr = args[i];
      curr->collect(site, to);
    }
  }
}

Site* NumberAlias::createSite()
{
	Interface* interf = 0;
	TRY(interf = RELATION_MNGR.createInterface(relId), 0);

	// because the empty table name is passed in, this will not work
	// for remote server

	TRY(Site* retVal = interf->getSite(), 0);
	delete interf;
	return retVal;
}

Site* TableAlias::createSite(){
	TRY(return ROOT_CATALOG.find(table), NULL);
}

Site* QuoteAlias::createSite(){
	assert(interf || !"not implemented");
	return interf->getSite();
}

#endif
