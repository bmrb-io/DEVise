/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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

#ifndef DATAREADER_DATAREADER_H
#define DATAREADER_DATAREADER_H
#include "Buffer.h"

class DataReaderParser;

// This class defines the DataReader interface for external programs
class DataReader {
private:
	char* _dataFileName; // Name of DataFile
	char* _schemaFileName; // Name of DRSchema File
	Buffer* myBuffer; // Buffer Object used for extracting fields
	DataReaderParser* myParser; // DRSchema Parser Object
	ifstream schemaStream; // File Stream for DRSchema File
	long* offset; // Offsets of each field in the destination buffer
	Status _uStat; // Status of DataReader
public:
	DRSchema* myDRSchema; // DRSchema Object associated with this Reader
	DataReader(const char* dataFile, const char* schemaFile);
	Status getRecord(char* dest); // Function to read next record
	Status getRndRec(char* dest,int fileOffset); // Function to read a random record
	bool isOk() {return _uStat == OK;} // Check the Status of Reader
};

#endif
