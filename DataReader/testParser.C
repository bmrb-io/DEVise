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
  Revision 1.4  1998/06/16 16:30:55  wenger
  Added standard headers to DataReader sources.

 */

#include <iostream.h>
#include <fstream.h>
#include "DataReader.h"

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

	int aa;
	char* results = new char[2048];
	Status status = OK;

	DataReader* myDataReader = new DataReader(df,sf);
	if (!(myDataReader->isOk())) {
		testResult = TEST_ERROR;
		goto end;
	}
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

	while (true) {
		status = myDataReader->getRecord(results);
		if (status == OK || status == FOUNDEOL || status == FOUNDEOF) {
			// these are okay
		} else if (status == FAIL) {
			cerr << "Error Occured in DataReader" << endl ;
			testResult = TEST_ERROR;
			goto end;
		} else {
			cerr << "Possible error in DataReader" << endl;
			cerr << "  status = " << status << endl;
			testResult = TEST_PROB_ERROR;
		}

#ifndef TESTDATAREADER
		for (int i = 0 ; i < aa ; i++) {
			char* tmp;
			switch (myDataReader->myDRSchema->tableAttr[i]->getType()) {
				case TYPE_INT:
					cout << *(int*)(results+(myDataReader->myDRSchema->tableAttr[i]->offset)) << " " ;
					break;
				case TYPE_DOUBLE:
					cout << *(double*)(results+(myDataReader->myDRSchema->tableAttr[i]->offset)) << " " ;
					break;
				case TYPE_STRING:
					tmp = results + (myDataReader->myDRSchema->tableAttr[i]->offset);
					cout << tmp << " " ;
			}
		}
		cout << "->  Got One" << endl ;
#endif

		if (status == FOUNDEOF) {
			cerr << "End Of File" << endl;
			break;
		}
	}

end:
	cout << endl << "Test result: ";
	switch (testResult) {
	case TEST_OK:
		cout << "OK";
		break;
	case TEST_ERROR:
		cout << "Error";
		break;
	case TEST_PROB_ERROR:
		cout << "Possible error";
		break;
	}
	cout << endl;
	
	return 1;
}

