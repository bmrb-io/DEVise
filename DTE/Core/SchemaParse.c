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

  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

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

class ISchemaSite : public Site {
	ISchema* schema;	
	bool done;
	Tuple retVal[1];
public:
	ISchemaSite(ISchema* schema) : Site(), schema(schema), done(false) {}
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
	virtual void initialize(){}
	virtual const Tuple* getNext(){
		if(done){
			return NULL;
		}
		done = true;
		retVal[0] = schema;
		return retVal;
	}
};

Site* ISchemaParse::createSite(){
	Catalog* catalog = getRootCatalog();
	Interface* interf;
	assert(catalog);
	TRY(interf = catalog->findInterface(tableName), NULL);
	delete catalog;
	TRY(ISchema* schema = interf->getISchema(tableName), NULL);
	delete interf;
	return new ISchemaSite(schema);
}
