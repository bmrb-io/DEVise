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

Catalog* getRootCatalog(){
	String catalogName;
	char* dev_schema = getenv("DEVISE_SCHEMA");
	if(dev_schema){
		catalogName = String(dev_schema) + "/catalog.dte";
	}
	else {
		catalogName = "./catalog.dte";
	}
	return new Catalog(catalogName);
}

/*
String getIndexCatName(){
	String catalogName;
	char* dev_schema = getenv("DEVISE_SCHEMA");
	if(dev_schema){
		catalogName = String(dev_schema) + "/index.dte";
	}
	else {
		catalogName = "./index.dte";
	}
	return catalogName;	
}
*/

class ISchemaExec : public Iterator {
	bool done;
	Tuple retVal[1];
public:
	ISchemaExec(const ISchema* schema) : done(false) {
		retVal[0] = (Type*) schema;
		// need to copy schema over
	}
	virtual ~ISchemaExec(){
		// do not delete schema  ?
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

class ISchemaSite : public Site {
	const ISchema* schema;	
public:
	ISchemaSite(const ISchema* schema) : Site(), schema(schema){}
	virtual ~ISchemaSite(){
		// do not delete schema
	}
	virtual int getNumFlds(){
		return 1;
	}
     virtual String* getTypeIDs(){
          return new String("schema");
     }
     virtual String *getAttributeNames(){
          return new String("schema");
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
	delete interf;
	return new ISchemaSite(schema);
}
