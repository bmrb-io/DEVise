#include "ParseTree.h"
#include "site.h"
#include "Utility.h"
#include "catalog.h"
#include "Engine.h"
#include "Interface.h"
#include "Inserter.h"
#include <string>

Site* MaterializeParse::createSite(){
	string nameStr = tableName->toString();
	TableName dirName = tableName->dirName();

#if defined(DEBUG)
	cerr << "Dir name: "; dirName.display(cerr); cerr << endl;
#endif

	string fileName = tableName->fileName();
	TRY(Interface* interf = ROOT_CATALOG.createInterface(&dirName), NULL);

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
	int numFlds = engine.getNumFlds();
	const TypeID* typeIDs = engine.getTypeIDs();

	string materFile = DTE_ENV_VARS.materViewDir; 
	materFile += "/" + nameStr;
	cerr << "Materializing view: " << nameStr << " into " << materFile << endl;
	ofstream* outf = new ofstream(materFile.c_str());
	if(!outf || !outf->good()){
		string err = string("Could not open file: ") + materFile;
		THROW(new Exception(err), NULL);
		// throw Exception(err);
	}

	Inserter inserter;
	inserter.open(outf, numFlds, typeIDs);
	engine.initialize();
	const Tuple* tuple;
	while((tuple = engine.getNext())){
		inserter.insert(tuple);
	}

	MaterViewInterface mvi
		(numFlds, typeIDs, attributeNames, materFile, query);
	TRY(dir.replace(fileName, (const ViewInterface*) &mvi), NULL);

	return new Site();
}
