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
  Revision 1.10  1997/08/12 19:58:42  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.9  1997/07/30 21:39:18  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.8  1997/07/22 15:00:54  donjerko
  *** empty log message ***

  Revision 1.7  1997/06/30 23:04:56  donjerko
  CVS:

  Revision 1.6  1997/06/21 22:48:01  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.5  1997/06/16 16:04:44  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "ParseTree.h"
#include "site.h"
#include "catalog.h"

class ISchemaExec : public Iterator {
	bool done;
	ISchema* schema;
	Tuple retVal[1];
public:
	ISchemaExec(ISchema* schema) : done(false), schema(schema) {
		retVal[0] = (Type*) schema;
	}
	virtual ~ISchemaExec(){
		delete schema;
	}
	virtual void initialize(){}
	virtual const Tuple* getNext(){
		if(done){
			return NULL;
		}
		done = true;
		return retVal;
	}
};

const String SCHEMA_STR("schema");

class ISchemaSite : public Site {
	ISchema* schema;	
public:
	ISchemaSite(const ISchema* schema) : Site(){
		assert(schema);
		this->schema = new ISchema(*schema);
	}
	virtual ~ISchemaSite(){
		// do not delete schema, ISchemaExec is the owner
	}
	virtual int getNumFlds(){
		return 1;
	}
     virtual const String* getTypeIDs(){
		return &SCHEMA_STR;
     }
     virtual const String *getAttributeNames(){
		return &SCHEMA_STR;
     }
	ISchemaExec* createExec(){
		return new ISchemaExec(schema);
	}
};

Site* ISchemaParse::createSite(){
	Catalog* catalog = getRootCatalog();
	Interface* interf;
	assert(catalog);
	TRY(interf = catalog->findInterface(tableName), NULL);
	delete catalog;
	TRY(const ISchema* schema = interf->getISchema(tableName), NULL);
	ISchemaSite* retVal = new ISchemaSite(schema);
	delete interf;
	return retVal;
}
