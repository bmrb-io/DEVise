/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.10  1998/11/03 17:53:33  okan
  Fixed Several bugs and changed DataReader to use UtilAtof

  Revision 1.9  1998/10/12 21:24:20  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

  Revision 1.8  1998/10/06 20:06:33  wenger
  Partially fixed bug 406 (dates sometimes work); cleaned up DataReader
  code without really changing functionality: better error handling,
  removed unused class members, added debug output, close data file (never
  closed before), various other cleanups.

  Revision 1.7  1998/10/02 17:20:00  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.6  1998/06/29 17:18:23  wenger
  Fixed bug 372 (crashed in DataReader caused by a pointer alignment problem).

  Revision 1.5  1998/06/16 16:30:52  wenger
  Added standard headers to DataReader sources.

 */

#include "sysdep.h"
#include "DataReader.h"
#include "Buffer.h"
#include "DataReaderParser.h"

//#define DEBUG

//---------------------------------------------------------------------------
DataReader::DataReader(const char* dataFile, const char* schemaFile)
{
#if defined(DEBUG)
    cout << "DataReader::DataReader(" << dataFile << ", " << schemaFile <<
	  ")\n";
#endif

	int rc;
	_uStat = OK;
	myDRSchema = NULL;
	myBuffer = NULL;

	ifstream schemaStream;
	schemaStream.open(schemaFile);
	if (schemaStream.fail()) {
		cout << "DRSchema File " << schemaFile << " can't be opened !..."
		  << endl;
		perror("open");
		_uStat = FAIL;
	} else {
		DataReaderParser *myParser = new DataReaderParser(&schemaStream,0);

		myDRSchema = new DRSchema();
		myParser->setDRSchema(myDRSchema);

		rc = myParser->parse();
		schemaStream.close();
		delete myParser;

#if defined(DEBUG)
		cout << "\nSchema before finalizing\n";
		cout << *myDRSchema << endl;
#endif

		if (rc != 0) {
			cerr << "Parse Error, DRSchema file " << schemaFile <<
			  " can't be parsed !" << endl;
			_uStat = FAIL;
		} else {
			if (myDRSchema->finalizeDRSchema() == OK) {
#if defined(DEBUG)
			cout << "\nSchema after finalizing\n";
			cout << *myDRSchema << endl;
#endif
				Status bufStatus;
				myBuffer = new Buffer(dataFile, myDRSchema, bufStatus);
				if (bufStatus != OK) {
					_uStat = FAIL;
				}
			} else {
				_uStat = FAIL;
			}
		}
	}

	if (_uStat != OK) {
		delete myBuffer;
		myBuffer = NULL;
		delete myDRSchema;
		myDRSchema = NULL;
	}

#if defined(DEBUG)
    cout << "  End of DataReader::DataReader(); status = " << _uStat << endl;
#endif
}

//---------------------------------------------------------------------------
DataReader::~DataReader() {

	_uStat = FAIL;

	delete myBuffer;
	myBuffer = NULL;
		
	delete myDRSchema;
	myDRSchema = NULL;
}

//---------------------------------------------------------------------------
bool
DataReader::getRecord(char* dest)
{
#if defined(DEBUG)
	cout << "DataReader::getRecord()\n";
#endif

	bool result;

	result = myBuffer->ReadRecord(dest, myDRSchema);

	return result;
}

//---------------------------------------------------------------------------
bool
DataReader::getRndRec(char* dest, int fileOffset)
{
	bool result;
	result = myBuffer->setBufferPos(fileOffset-1); //TEMP -- why -1?
	if (result) {
		result = myBuffer->ReadRecord(dest, myDRSchema);
	}
	return result;
}

//---------------------------------------------------------------------------
bool
DataReader::isEof()
{
	return myBuffer->GetState() == BufEof;
}

//---------------------------------------------------------------------------
ostream &
operator<<(ostream &out, const DataReader &dr)
{
	out << "DataReader:\n";
	out << "  Status: " << dr._uStat << endl;
	if (dr.myDRSchema != NULL) out << *dr.myDRSchema;

	return out;
}
