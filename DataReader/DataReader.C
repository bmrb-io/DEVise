#include "sysdep.h"
#include "DataReader.h"
#include "DataReaderParser.h"

DataReader::DataReader(const char* dataFile, const char* schemaFile) {
	int rc;
	_uStat = OK;
	_dataFileName = new char[strlen(dataFile)+1];
	strcpy(_dataFileName,dataFile);
	_schemaFileName = new char[strlen(schemaFile)+1];
	strcpy(_schemaFileName,schemaFile);
	myDRSchema = new DRSchema(_schemaFileName);

	schemaStream.open(_schemaFileName);
	if (schemaStream.fail()) {
		cout << "DRSchema File can't be opened !..." << endl;
		_uStat = FAIL;
	} else {
		myParser = new DataReaderParser(&schemaStream,0);
		myParser->setDRSchema(myDRSchema);
		rc = myParser->parse();
		schemaStream.close();
		if (rc != 0) {
			cerr << "Parse Error, DRSchema file can't be parsed !" << endl;
			_uStat = FAIL;
		} else {
			if (myDRSchema->finalizeDRSchema() == OK) {
				offset = new long[myDRSchema->qAttr];
				myBuffer = new Buffer(_dataFileName, myDRSchema);
			} else {
				_uStat = FAIL;
			}
		   
		}
	}
}

DataReader::~DataReader() {

	if (_dataFileName != NULL)
		delete [] _dataFileName;
		
	if (_schemaFileName != NULL)
		delete [] _schemaFileName;
		
	if (myBuffer != NULL)
		delete myBuffer;
		
	if (myParser != NULL)
		delete myParser;
		
	if (offset != NULL)
		delete [] offset;
		
	if (myDRSchema != NULL)
		delete myDRSchema;
}

Status DataReader::getRecord(char* dest) {
	Status status;
	char* tmpPoint;

	for (int i = 0 ; i < (int)(myDRSchema->qAttr); i++) {
		tmpPoint = dest + myDRSchema->tableAttr[i]->offset; 
		status = myBuffer->extractField(myDRSchema->tableAttr[i],tmpPoint);
		if (status != FOUNDSEPARATOR)
			return status;
	}
	return FOUNDSEPARATOR;
}
Status DataReader::getRndRec(char* dest, int fileOffset) {
	Status status;
	status = myBuffer->setBufferPos(fileOffset-1);
	if (status != OK)
		return status;
	status = getRecord(dest);
	return status;
}
