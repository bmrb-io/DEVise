#include "types.h"
#include "StandardRead.h"
#include "AccessMethod.h"
#include "myopt.h"
#include "catalog.h" 	// for root catalog
#include "ExecOp.h"
#include "Interface.h"
#include "DataRead.h"
#include "DevRead.h"
#include "DataReader.h"
#include "UniData.h"

Iterator* FileScan::createExec() {
	assert(!"not implemented");
	return NULL;
}

FileScan::FileScan(const NewStat& stat)
{
//	cost = stat.getNumPgs();
//	cost = TABLE_CARD * NUM_FLDS * FIELD_SZ / PAGE_SZ;
}

Cost FileScan::getCost() const
{
	return cost;
}

StandardAM::StandardAM
	(const ISchema& schema, const string& url, const Stats& stats) : schemaM(schema), url(url)
{
}

Cost StandardAM::getCost() const
{
	// fix this later

	return 0;
}

Iterator* StandardAM::createExec()
{
	TypeIDList typeIDs = schemaM.getTypeIDs();
	return new StandReadExec(typeIDs, url);
}

vector<BaseSelection*> StandardAM::getProjectList(const string& alias) const
{
	vector<BaseSelection*> retVal;
	const string* atts = schemaM.getAttributeNames();
	for(int i = 0; i < schemaM.getNumFlds(); i++){
		retVal.push_back(new PrimeSelection(alias, atts[i]));
	}
	return retVal;
}

DataReaderAM::DataReaderAM(const string& schemaFile, const string& dataFile) :
          ud(NULL), numFlds(0), typeIDs(NULL),
          attributeNames(NULL),
          order(NULL)
{
	ud = 0;
	dr = 0;

	if(schemaFile.substr(schemaFile.size() - 3) == "ddr"){

		// datareader

		dr = new DataReader(dataFile.c_str(), schemaFile.c_str());
		if(!dr || !dr->isOk()){
			string msg = string("Cannot create DataReader table(") +
				dataFile + ", " + schemaFile + ")";
			CON_THROW(new Exception(msg));
			// throw Exception(msg);
		}
		translateDRInfo();
	}
	else{

		// unidata
		// this path is obsolete

		ud = new UniData(dataFile.c_str(), schemaFile.c_str());
		if(!ud || !ud->isOk()){
			string msg = string("Cannot create Unidata table(") +
				dataFile + ", " + schemaFile + ")";
			CON_THROW(new Exception(msg));
			// throw Exception(msg);
		}
		translateUDInfo();
	}
	CON_END:;
}

void DataReaderAM::translateDRInfo() {
	numFlds = dr->myDRSchema->tAttr + 1; //for RECID
	int temNum = dr->myDRSchema->qAttr + 1;
	int i = 0;
	int fType;
	ostringstream tmp;
	typeIDs = new TypeID[numFlds];
	attributeNames = new string[numFlds];

	typeIDs[0] = INT_TP;
	attributeNames[0] = string("recId");

	for (int count = 1; count < temNum; count ++) {

		if ((dr->myDRSchema->tableAttr[count-1])->getFieldName() == NULL) { //skipping SKIP attributes
			continue;
		}
		i++;
			
		fType = dr->myDRSchema->tableAttr[count-1]->getType();
		switch (fType) {
			case TYPE_INT:
				typeIDs[i] = string("int");
				break;
			case TYPE_STRING:
				tmp << "string" << (dr->myDRSchema->tableAttr[count-1]->getMaxLen() > 0 ? dr->myDRSchema->tableAttr[count-1]->getMaxLen() : dr->myDRSchema->tableAttr[count-1]->getFieldLen()) << ends;
				typeIDs[i] = string(tmp.str());
				tmp.seekp(0);
				break;
			case TYPE_DOUBLE:
				typeIDs[i] = string("double");
				break;
			case TYPE_DATE:
				typeIDs[i] = string("date");
				break;
			default:
				cout <<"This type isn't handled yet: " << dr->myDRSchema->tableAttr[count-1]->getType() << endl;
				typeIDs[i] = string("unknown");
		}
		attributeNames[i] = string(dr->myDRSchema->tableAttr[count-1]->getFieldName());
	}
}

void DataReaderAM::translateUDInfo() {

	// this function is same as translateDRInfor, except it is for unidata

	numFlds = ud->schema()->NumFlatAttrs() + 1; // for recId
    typeIDs = new TypeID[numFlds];
    attributeNames = new string[numFlds];
    AttrStk *stk = ud->schema()->GetFlatAttrs();
    typeIDs[0] = INT_TP;
	typeIDlist.push_back(INT_TP);
    attributeNames[0] = string("recId");
    for(int i = 1; i < numFlds; i++){
        Attr *at = stk->ith(i - 1);
        typeIDs[i] = translateUDType(at);
		typeIDlist.push_back(typeIDs[i]);
        attributeNames[i] = string(at->flat_name());
    }
}

Iterator* DataReaderAM::createExec()
{
	if(dr){
		DataReadExec* retVal = new DataReadExec(dr);
		dr = 0;	// not the owner any more
		return retVal;
	}
	else{
		DevReadExec* retVal = new DevReadExec(ud, typeIDlist);
		ud = 0;	// not the owner any more
		return retVal;
	}
}

DataReaderAM::~DataReaderAM()
{
	delete ud;
	delete dr;

	delete [] typeIDs;
	typeIDs = NULL;
	delete [] attributeNames;
	attributeNames = NULL;
	delete order;
	order = NULL;
}

Cost DataReaderAM::getCost() const
{
	// fix this later

	return 0;
}

vector<BaseSelection*> DataReaderAM::getProjectList(const string& alias) const
{
	vector<BaseSelection*> retVal;
	assert(attributeNames);
	const string* atts = attributeNames;
	for(int i = 0; i < numFlds; i++){
		retVal.push_back(new PrimeSelection(alias, atts[i]));
	}
	return retVal;
}

TypeID DataReaderAM::translateUDType(Attr* at){

   switch (at->type()) {

	case Int_Attr:
       return "int";	

	case Float_Attr:
       return "float";

	case Double_Attr:
       return "double";

	case String_Attr:{
       int size = at->size_of();
       ostrstream tmp;
       tmp << "string" << size << ends;
       char* tmp2 = tmp.str();
       string retVal(tmp2);
       delete [] tmp2;
       return retVal;
     } 

	case UnixTime_Attr:
       return "time_t";

	case DateTime_Attr:
       return "date";

	case Invalid_Attr:
	case UserDefined_Attr:
	  cout << "This type isn't handled yet: " << at->type() << endl;
	  break;
   }
   return "unknown";
}
