/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.13  1998/11/23 19:18:47  donjerko
  Added support for gestalts

  Revision 1.12  1998/04/14 17:03:25  donjerko
  *** empty log message ***

  Revision 1.11  1998/02/17 23:09:11  donjerko
  *** empty log message ***

  Revision 1.10  1997/12/04 04:05:10  donjerko
  *** empty log message ***

  Revision 1.9  1997/11/12 23:17:24  donjerko
  Improved error checking.

  Revision 1.8  1997/09/05 22:56:08  donjerko
  *** empty log message ***

  Revision 1.7  1997/09/05 22:20:04  donjerko
  Made changes for port to NT.

  Revision 1.6  1997/08/22 23:13:02  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.5  1997/08/21 21:04:20  donjerko
  Implemented view materialization

  Revision 1.4  1997/08/04 14:50:47  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.3  1997/07/30 21:39:17  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.2  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
//#include<stdio.h>	// for perror   erased for sysdep.h
//#include<errno.h>   erased for sysdep.h

//#include "queue.h"
#include "myopt.h"
//#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
//#include "listop.h"
#include "ParseTree.h"
#include "sysdep.h"
#include "DTE/util/Del.h"
#include "STuple.h"
#include "Inserter.h"
#include "DteSymbolTable.h"
#include "ExecExpr.h"
#include "DTE/types/DteStringAdt.h"


static const int DETAIL = 1;
LOG(extern ofstream logFile);

// #define DEBUG

//kb: insert should be able to take a query
  
Iterator* InsertParse::createExec(){

#ifdef DEBUG
	cerr << "Inserting into ";
	tableName->display(cerr);
	cerr << " (";
	displayList(cerr, (List<BaseSelection*>*) fieldList, ", ");
	cerr << ")" << endl;
#endif

        // validate expressions
        DteSymbolTable symtab;
        if( ! typeCheckVec(fieldList, symtab) ) {
          string msg = "typecheck failure during insert";
          cerr << msg << endl;
          THROW(new Exception(msg), NULL);
        }

        // find the table
        Del<Interface> interface( ROOT_CATALOG.createInterface(tableName) );
        assert(interface);

        int numFields = fieldList.size();

        Del<Inserter> inserter = interface->createInserter();

        if( numFields == 1 &&
            interface->getTypeNm() == CatalogInterface::typeName &&
            fieldList[0]->getExprType() == OptExpr::CONST_ID &&
            (fieldList[0]->getAdt()).getTypeID() == DteStringAdt::typeID ) {
          // skip further type checks.
          // This is a hack to allow catalog inserts from a single string.
          // It is used by devise V1. KB 1/19/1999
          //kb: should probably add a getPtr() function to class Del.
          CatalogInterface* iface = 
            static_cast<CatalogInterface*>(interface.operator->());
          OptConstant* optc = static_cast<OptConstant*>(fieldList[0]);
          string filename = iface->getCatalogName();
          ofstream out(filename.c_str(), ios::app);
          out << DteStringAdt::cast(optc->getValue()) << '\n';
          return 0;

        } else {

          // make sure types match
          if( numFields != interface->getNumFields() ) {
            string msg = "number of fields do no match during insert";
            cerr << msg << endl;
            THROW(new Exception(msg), NULL);
          }
          DteTupleAdt adt = interface->getISchema()->getAdt();
          for(int i = 0 ; i < numFields ; i++) {
            const DteAdt& adt1 = fieldList[i]->getAdt();
            const DteAdt& adt2 = adt.getAdt(i);
            if( adt1 != adt2 ) {
              string msg = "types do not match during insert";
              cerr << msg << endl;
              THROW(new Exception(msg), NULL);
            }
          }
        }

        // evaluate expressions
        vector< Del<ExecExpr> > exprs(numFields);
        STuple tuple(numFields);
        for(int i = 0 ; i < numFields ; i++) {
          exprs[i] = fieldList[i]->createExec(OptExprListList());
          assert(exprs[i]);
          tuple[i] = exprs[i]->eval(NULL,NULL);
        }
        
        inserter->insert(tuple);
	return 0;

#if 0
// kb: old stuff
     TRY(Site* site = ROOT_CATALOG.find(tableName), NULL);

	assert(site);

//	TRY(site->typify(""), NULL);
//	int numFlds = site->getNumFlds();
/*
	// this is not satisfied because we are inserting strings
	if(numFlds != fieldList->cardinality()){
		THROW(new Exception("Number of fields do not match"), NULL);
	}
*/
//	const string* types = site->getTypeIDs();

// What follows is just a temporary kludge. 
// One should construct a tuple and inset it

	fieldList->rewind();
	assert(fieldList->cardinality() == 1);
	ConstantSelection* sel = fieldList->get();
	string tmp = sel->toString();
#if defined(DEBUG)
	cerr << "got string: " << tmp << endl;
#endif
	size_t bufLen = tmp.size() + 1;
	string inStr = stripSQLQuotes(tmp.c_str());
#if defined(DEBUG)
	cerr << "Appending to the file: " << inStr << endl;
#endif
//	TRY(site->writeOpen(), NULL);
	site->append(inStr.c_str());
	site->writeClose();
	delete site;
	return 0;
#endif

}
