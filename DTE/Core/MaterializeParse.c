#include "ParseTree.h"
//#include "site.h"
#include "Utility.h"
#include "catalog.h"
#include "Engine.h"
#include "Interface.h"
#include "Inserter.h"
#include <string>

Iterator* MaterializeParse::createExec(){
	string nameStr = tableName.toString();
	TableName dirName = tableName.dirName();

#if defined(DEBUG)
	cerr << "Dir name: "; dirName.display(cerr); cerr << endl;
#endif

	string fileName = tableName.fileName();
	TRY(Interface* interf = ROOT_CATALOG.createInterface(dirName), NULL);

	if(!interf){
		string err("Collection ");
		err += dirName.toString() + " does not exists";
		THROW(new Exception(err), NULL);
		// throw Exception(err);
	}
	if(interf->getType() != Interface::CATALOG){
		string err("View ");
		err += nameStr + " is not defined within a directroy file";
		THROW(new Exception(err), NULL);
		// throw Exception(err);
	}
	CatalogInterface* catInterf = (CatalogInterface*) interf;
	string dirPath = catInterf->getFileName();
	delete interf;
	interf = NULL;

	Directory dir(dirPath);
	TRY(interf = dir.createInterface(fileName), NULL);
	assert(interf);
	if(interf->getType() != Interface::VIEW){
		string err = nameStr + " is not a view";
		THROW(new Exception(err), NULL);
		// throw Exception(err);
	}

	ViewInterface* viewInterf = (ViewInterface*) interf;
	string query = viewInterf->getQuery();
	const string* attributeNames = viewInterf->getAttributeNames();
	assert(attributeNames);

	Engine engine(query);
	TRY(engine.optimize(), NULL);

	string materFile = DTE_ENV_VARS.valueOf(DTE_ENV_VARS.materViewDir); 
	if(materFile.empty()){
		string err = "Please set the env var '" 
			+ DTE_ENV_VARS.materViewDir + "' to point to a directory";
		THROW(new Exception(err), NULL);
	}
	materFile += "/" + nameStr;
	cerr << "Materializing view: " << nameStr << " into " << materFile << endl;
	Inserter inserter(materFile, engine.getAdt());
	const Tuple* tuple = engine.getFirst();
	while(tuple){
		inserter.insert(tuple);
		tuple = engine.getNext();
	}

	MaterViewInterface mvi(engine.getAdt(),
                               vector<string>(attributeNames,
                                              attributeNames + 
                                                engine.getNumFields()),
                               materFile, query);
	TRY(dir.replace(fileName, &mvi), NULL);

	return NULL;
}
