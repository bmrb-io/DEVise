#include "types.h"
#include "StandardRead.h"
#include "AccessMethod.h"
#include "myopt.h"
#include "catalog.h" 	// for root catalog
//#include "ExecOp.h"
#include "Interface.h"
#include "DataRead.h"
#include "DevRead.h"
#include "DataReader.h"
#include "UniData.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"

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
	return new StandReadExec(schemaM.getAdt(), url);
}

vector<OptExpr*> StandardAM::getProjectList(const string& alias) const
{
	vector<OptExpr*> retVal;
	const vector<string>& atts = schemaM.getAttributeNames();
	for(int i = 0; i < schemaM.getNumFields(); i++){
		retVal.push_back(new OptField(alias, atts[i]));
	}
	return retVal;
}

DataReaderAM::DataReaderAM(const string& schemaFile, const string& dataFile)
  : dr(NULL), ud(NULL)
{
	//
	// Try opening the data source as a DataReader; if that doesn't work,
	// try opening it as a UniData.
	//
	dr = new DataReader(dataFile.c_str(), schemaFile.c_str());
	if (dr && dr->isOk()) {
		DataReadExec::translateSchema(dr, schema);
	} else {
		if (dr) {
			delete dr;
			dr = NULL;
		}

		cout << "Cannot create DataReader table(" << dataFile << ", " <<
		  schemaFile << ")\nTrying to create UniData table\n";

		ud = new UniData(dataFile.c_str(), schemaFile.c_str());
		if (ud && ud->isOk()) {
			DevReadExec::translateSchema(ud, schema);
		} else {
			if (ud) {
				delete ud;
				ud = NULL;
			}

			cout << "Cannot create UniData table(" << dataFile << ", " <<
			  schemaFile << ")\nTrying to create UniData table\n";
		}
	}

	if (!dr && !ud) {
		string msg = string("Cannot create DataReader or Unidata table(") +
			dataFile + ", " + schemaFile + ")";
		CON_THROW(new Exception(msg));
		// throw Exception(msg);
	}
	CON_END:;
}

#if 0
void DataReaderAM::translateDRInfo() {
	int temNum = dr->myDRSchema->qAttr;
	int fType;

        //tupAdt.push_back(DteInt4Adt());
	//attributeNames.push_back("recId");

	for(int i = 0; i < temNum; i++) {

		if ((dr->myDRSchema->tableAttr[i])->getFieldName() == NULL) { //skipping SKIP attributes
			continue;
		}
			
		fType = dr->myDRSchema->tableAttr[i]->getType();
		switch (fType) {
                case TYPE_INT:
                  tupAdt.push_back(DteIntAdt());
                  break;
                case TYPE_STRING: {
                  int maxlen = dr->myDRSchema->tableAttr[i]->getMaxLen();
                  if( maxlen < 0 )
                    maxlen = dr->myDRSchema->tableAttr[i]->getFieldLen();
                  tupAdt.push_back(DteStringAdt(maxlen));
                  break;
                }
                case TYPE_DOUBLE:
                  tupAdt.push_back(DteDoubleAdt());
                  break;
                case TYPE_DATE:
                  tupAdt.push_back(DteDateAdt());
                  break;
                default:
                  cout <<"This type isn't handled yet: "
                       << dr->myDRSchema->tableAttr[i]->getType() << endl;
                  assert(!"unknown type");
		}
		attributeNames.push_back(
                  dr->myDRSchema->tableAttr[i]->getFieldName());
	}
}
#endif

#if 0
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
#endif

Iterator* DataReaderAM::createExec()
{
	if(dr){
		DataReadExec* retVal = new DataReadExec(dr);
		dr = 0;	// not the owner any more
		return retVal;
	}
        DevReadExec* retVal = new DevReadExec(ud);
        ud = 0;	// not the owner any more
        return retVal;
}

DataReaderAM::~DataReaderAM()
{
        delete ud;
	delete dr;
}

Cost DataReaderAM::getCost() const
{
	// fix this later

	return 0;
}

vector<OptExpr*> DataReaderAM::getProjectList(const string& alias) const
{
	vector<OptExpr*> retVal;
        int numFlds = schema.getNumFields();
	const vector<string>& atts = schema.getAttributeNames();
	for(int i = 0; i < numFlds; i++) {
		retVal.push_back(new OptField(alias, atts[i]));
	}
	return retVal;
}

#if 0
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
       ostringstream tmp;
       tmp << "string" << size;
       return tmp.str();
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
#endif
