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
  Revision 1.6  1998/06/29 17:18:23  wenger
  Fixed bug 372 (crashed in DataReader caused by a pointer alignment problem).

  Revision 1.5  1998/06/16 16:30:52  wenger
  Added standard headers to DataReader sources.

 */

#include "sysdep.h"
#include "DataReader.h"
#include "DataReaderParser.h"

//#define DEBUG

DataReader::DataReader(const char* dataFile, const char* schemaFile)
{
#if defined(DEBUG)
    printf("DataReader::DataReader(%s, %s)\n", dataFile, schemaFile);
#endif

	int rc;
	_uStat = OK;
	myDRSchema = new DRSchema();

	ifstream schemaStream;
	schemaStream.open(schemaFile);
	if (schemaStream.fail()) {
		cout << "DRSchema File " << schemaFile << " can't be opened !..."
		  << endl;
		_uStat = FAIL;
	} else {
		myParser = new DataReaderParser(&schemaStream,0);
		myParser->setDRSchema(myDRSchema);
		rc = myParser->parse();
		schemaStream.close();
		if (rc != 0) {
			cerr << "Parse Error, DRSchema file " << schemaFile <<
			  " can't be parsed !" << endl;
			_uStat = FAIL;
		} else {
			if (myDRSchema->finalizeDRSchema() == OK) {
				offset = new long[myDRSchema->qAttr];
				myBuffer = new Buffer(dataFile, myDRSchema);
			} else {
				_uStat = FAIL;
			}
		}
	}

#if defined(DEBUG)
    printf("  End of DataReader::DataReader(); _uStat = %d\n", _uStat);
#endif
}

DataReader::~DataReader() {

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
#if defined(DEBUG)
    printf("DataReader::getRecord()\n");
#endif

	Status status;
	char* tmpPoint;

	for (int i = 0 ; i < (int)(myDRSchema->qAttr); i++) {
		tmpPoint = dest + myDRSchema->tableAttr[i]->offset; 
		status = myBuffer->extractField(myDRSchema->tableAttr[i],tmpPoint);
		if (status != FOUNDSEPARATOR) {
			return status;
		}
	}

	// Consume the rest of this record (does nothing if record delimiter
	// not defined.
	return myBuffer->consumeRecord();
}
Status DataReader::getRndRec(char* dest, int fileOffset) {
	Status status;
	status = myBuffer->setBufferPos(fileOffset-1);
	if (status != OK)
		return status;
	status = getRecord(dest);
	return status;
}
