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
  Revision 1.11  1999/02/01 19:18:51  wenger
  Added DataReader functions to set and get file offset.

  Revision 1.10  1999/01/18 22:34:17  wenger
  Considerable changes to the DataReader:  reading is now per-field rather
  than per-character (except for dates); the "extractor" functions now do
  all the work, and the "value" functions have been eliminated; return values
  are more clear, and behaviour in "boundary conditions" is better-defined;
  fixed a number of bugs in the course of making these changes.  (The
  DataReader could still use some more cleanup.)

  Revision 1.9  1998/11/03 17:53:36  okan
  Fixed Several bugs and changed DataReader to use UtilAtof

  Revision 1.8  1998/10/13 15:41:39  wenger
  Purify'd DataReader.

  Revision 1.7  1998/10/12 21:24:22  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

  Revision 1.6  1998/10/06 20:06:35  wenger
  Partially fixed bug 406 (dates sometimes work); cleaned up DataReader
  code without really changing functionality: better error handling,
  removed unused class members, added debug output, close data file (never
  closed before), various other cleanups.

  Revision 1.5  1998/10/02 17:20:01  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.4  1998/06/16 16:30:55  wenger
  Added standard headers to DataReader sources.

 */

#include <iostream.h>
#include <fstream.h>
#include "DataReader.h"
#include "DateTime.h"

//#define TESTDATAREADER  // ifdef, no output

enum TestResult { TEST_OK, TEST_ERROR, TEST_PROB_ERROR };

void
PrintRecord(DataReader* myDataReader, char results[])
{
	for (int i = 0 ; i < (int)myDataReader->myDRSchema->qAttr ; i++) {
		char* tmp;
		EncodedDTF tmpDate;
		if (myDataReader->myDRSchema->tableAttr[i]->getFieldName() == NULL) {
			// This is a skip attribute.
			continue;
		}
		switch (myDataReader->myDRSchema->tableAttr[i]->getType()) {
			case TYPE_INVALID:
				cerr << "Invalid attribute type\n";
				break;
			case TYPE_INT:
				cout << "{" << *(int*)(results+
				  (myDataReader->myDRSchema->tableAttr[i]->offset)) <<
				  "} " ;
				break;

			case TYPE_DOUBLE:
				cout << "{" << *(double*)(results+
				  (myDataReader->myDRSchema->tableAttr[i]->offset)) <<
				  "} " ;
				break;

			case TYPE_STRING:
				tmp = results +
				  (myDataReader->myDRSchema->tableAttr[i]->offset);
				cout << "{" << tmp << "} " ;
				break;

			case TYPE_DATE:
				memcpy(&tmpDate,
				  results+(myDataReader->myDRSchema->tableAttr[i]->offset),
				  sizeof(tmpDate));
				{
					double secs = tmpDate.getSec() +
					  tmpDate.getNanoSec() * 1.0e-9;
					cout << "{" << tmpDate.getYear() << "-" <<
					  tmpDate.getMonth() << "-" << tmpDate.getDay() << " " <<
					  tmpDate.getHour() << ":" << tmpDate.getMin() << ":" <<
					  secs << "} ";
				}
				break;

			default:
				cerr << "Illegal attribute type.\n";
				break;
		}
	}
	cout << endl << endl;
}

int
main(int ARGV, char **ARGC)
{
	if (ARGV != 3) {
		cerr << "Usage : testParser <datafile> <schemafile>\n" << endl ;
		exit(1);
	}
	char* df = ARGC[1];
	char* sf = ARGC[2];

	TestResult testResult = TEST_OK;
	int exitVal;

	int aa;
	char* results = new char[2048];
	bool status = true;
	int recordNum = 0;
	int offset = 0;

	DataReader* myDataReader = new DataReader(df,sf);
	if (!(myDataReader->isOk())) {
		testResult = TEST_ERROR;
		goto end;
	}

#if defined(DEBUG)
	cout << *myDataReader;
#endif

	aa = myDataReader->myDRSchema->qAttr;

/**********************************************************************
	if (myDataReader->myDRSchema->getKeys() != NULL) {
		 cout << "KEYS : " << endl;
		 int cc; 
		 for (i = 0; i < myDataReader->myDRSchema->getKeys()->size() ; i++) {
			 for (aa = 0 ; aa < (*(myDataReader->myDRSchema->getKeys()))[i].size(); aa++) {
				 cout << (*(myDataReader->myDRSchema->getKeys()))[i][aa] << " " ;
			 }
			 cout << endl;
		 }
	}
	if (myDataReader->myDRSchema->getSorted() != NULL) {
		 cout << "SORTED : " << endl;
		 for (i = 0; i < myDataReader->myDRSchema->getSorted()->size(); i++) {
			 cout << (*(myDataReader->myDRSchema->getSorted()))[i] << " " ;
		 }
		 cout << endl;
	} 
 **********************************************************************/

	bool writeData; 

	//
	// Test setting and getting the file pointer location.
	//
	if (!myDataReader->setBufferPos(0)) {
		cerr << "Error setting buffer position\n";
		testResult = TEST_ERROR;
	}
	offset = myDataReader->getBufferPos();
	if (offset != 0) {
		cerr << "Error after setting buffer position: offset should be 0; "
		  "it is " << offset << endl;
		testResult = TEST_ERROR;
	}

	//
	// Read all of the records.
	//
	offset = -1;
	while (true) {
		writeData = true;
#if defined(DEBUG)
		cout << "\nBefore getRecord()\n";
#endif

		results[0] = '\0';
		status = myDataReader->getRecord(results);

#if defined(DEBUG)
		cout << "  After getRecord(); status = " << status << endl;
#endif

		if (!status) {
			cerr << "Error occured in DataReader at record " << recordNum <<
			  endl << endl;
			testResult = TEST_ERROR;
			writeData = false;
		}

#ifndef TESTDATAREADER
		if (writeData) {
			cout << "Record " << recordNum << ": ";
			PrintRecord(myDataReader, results);
		}
#endif

		if (myDataReader->isEof()) {
			cerr << "End of file" << endl;
			break;
		}

		if (recordNum == 0) {
			offset = myDataReader->getBufferPos();
		}

		//
		// Test reading a random record (read record 1 again).
		//
		if (recordNum == 1) {
			if (offset >= 0) {
				status = myDataReader->getRndRec(results, offset);

				if (!status) {
					cerr << "Error occured in DataReader while reading "
					  "random record"
			  		<< endl << endl;
					testResult = TEST_ERROR;
					writeData = false;
				}

#ifndef TESTDATAREADER
				if (writeData) {
					cout << "Record 1: ";
					PrintRecord(myDataReader, results);
				}
#endif
			}
		}

		recordNum++;
	}


end:
	delete myDataReader;
	delete [] results;

	cout << endl << "Test result: ";
	switch (testResult) {
	case TEST_OK:
		cout << "OK";
		exitVal = 0;
		break;
	case TEST_ERROR:
		cout << "Error";
		exitVal = 1;
		break;
	case TEST_PROB_ERROR:
		cout << "Possible error";
		exitVal = 1;
		break;
	}
	cout << endl;
	
	return exitVal;
}
