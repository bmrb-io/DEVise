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

int main(int ARGV, char **ARGC) {

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
	Status status = OK;

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

	int recordNum = 0;
	while (true) {
#if defined(DEBUG)
		cout << "\nBefore getRecord()\n";
#endif
		status = myDataReader->getRecord(results);
#if defined(DEBUG)
		cout << "  After getRecord(); status = " << status << endl;
#endif
		if (status == OK || status == FOUNDEOL || status == FOUNDEOF) {
			// these are okay
		} else if (status == FAIL) {
			cerr << "Error occured in DataReader at record " << recordNum <<
			  endl ;
			testResult = TEST_ERROR;
		} else {
			cerr << "Possible error in DataReader at record " << recordNum <<
			  endl;
			cerr << "  status = " << status << endl;
			if (testResult != TEST_ERROR) testResult = TEST_PROB_ERROR;
		}

#ifndef TESTDATAREADER
		cout << "Record " << recordNum << ": ";

		for (int i = 0 ; i < aa ; i++) {
			char* tmp;
			EncodedDTF tmpDate;
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
						  tmpDate.getNanoSec() * 1.0e-6;
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
#endif

		if (status == FOUNDEOF) {
			cerr << "End of file" << endl;
			break;
		}

		recordNum++;
	}

end:
	delete myDataReader;

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
