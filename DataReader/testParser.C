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

#include <iostream.h>
#include <fstream.h>
#include "DataReader.h"

//#define TESTDATAREADER  // ifdef, no output

int main(int ARGV, char **ARGC) {

	if (ARGV != 3) {
		cerr << "Usage : testParser <datafile> <schemafile>\n" << endl ;
		exit(1);
	}
	char* df = ARGC[1];
	char* sf = ARGC[2];

	int aa;
	DataReader* myDataReader = new DataReader(df,sf);
	if (!(myDataReader->isOk())) {
		exit(1);
	}
	aa = myDataReader->myDRSchema->qAttr;
	char* results = new char[2048];
	Status status = OK;

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

#ifdef TESTDATAREADER

	while (true) {
		status = myDataReader->getRecord(results);
		if ((status == FAIL) || (status == FOUNDEOF)) {
			if (status == FAIL) {
				cerr << "Encountered Error in DataReader " << endl;
			} else {
				cerr << "End Of File" << endl;
			}
			return 1;
		}
	}

#else
	int i;
	char* tmp;
	while (true) {
		status = myDataReader->getRecord(results);
		if (status == FAIL) {
			cerr << "Error Occured in DataReader" << endl ;
			return 1;
		}
		for ( i = 0 ; i < aa ; i++) {
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
		if (status == FOUNDEOF) {
			cerr << "FOUND EOF" << endl;
			return 1;
		}
	}

#endif
	
	return 1;
}

