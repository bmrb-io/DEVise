#include "sysdep.h"
#include "DataReader.h"

DataReader::DataReader(char* dataFile, char* schemaFile) {
	int rc;
	_uStat = OK;
	_dataFileName = new char[strlen(dataFile)+1];
	strcpy(_dataFileName,dataFile);
	_schemaFileName = new char[strlen(schemaFile)+1];
	strcpy(_schemaFileName,schemaFile);
	mySchema = new Schema(schemaFile);

	schemaStream.open(_schemaFileName);
	if (schemaStream.fail()) {
		cout << "Schema File can't be opened !..." << endl;
		_uStat = FAIL;
	} else {
		myParser = new DataReaderParser(&schemaStream,0);
		myParser->setSchema(mySchema);
		rc = myParser->parse();
		schemaStream.close();
		if (rc != 0) {
			cerr << "Parse Error, Schema file can't be parsed !" << endl;
			_uStat = FAIL;
		} else {
			if (mySchema->finalizeSchema() == OK) {
				offset = new long[mySchema->qAttr];
				myBuffer = new Buffer(dataFile, mySchema);
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
		
	if (mySchema != NULL)
		delete mySchema;
}

Status DataReader::getRecord(char* dest) {
	Status status;
	char* tmpPoint;

	for (int i = 0 ; i < (int)(mySchema->qAttr); i++) {
		tmpPoint = dest + mySchema->tableAttr[i]->offset; 
		status = myBuffer->extractField(mySchema->tableAttr[i],tmpPoint);
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
