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
 */

#include "ParseTree.h"
#include "site.h"

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

class SchemaSite : public Site {
	Schema* schema;	
	bool done;
public:
	SchemaSite(Schema* schema) : Site(), schema(schema), done(false) {}
	virtual ~SchemaSite(){
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
	virtual void initialize(){}
	virtual Tuple* getNext(){
		if(done){
			return NULL;
		}
		done = true;
		Tuple* retVal = new Tuple[1];
		retVal[0] = schema;
		return retVal;
	}
};

Site* SchemaParse::createSite(){
	Catalog* catalog = getRootCatalog();
	Interface* interf;
	assert(catalog);
	TRY(interf = catalog->findInterface(tableName), NULL);
	delete catalog;
	TRY(Schema* schema = interf->getSchema(tableName), NULL);
	delete interf;
	return new SchemaSite(schema);
}
