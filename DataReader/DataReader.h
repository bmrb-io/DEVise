#ifndef DATAREADER_DATAREADER_H
#define DATAREADER_DATAREADER_H
#include "DataReaderParser.h"
#include "Buffer.h"

// This class defines the DataReader interface for external programs
class DataReader {
private:
	char* _dataFileName; // Name of DataFile
	char* _schemaFileName; // Name of Schema File
	Buffer* myBuffer; // Buffer Object used for extracting fields
	DataReaderParser* myParser; // Schema Parser Object
	ifstream schemaStream; // File Stream for Schema File
	long* offset; // Offsets of each field in the destination buffer
	Status _uStat; // Status of DataReader
public:
	Schema* mySchema; // Schema Object associated with this Reader
	DataReader(char* dataFile, char* schemaFile);
	Status getRecord(char* dest, int* offset); // Function to read next record
	Status getRndRec(char* dest, int* offset, int fileOffset); // Function to read a random record
	bool isOk() {return _uStat == OK;} // Check the Status of Reader
};

#endif
