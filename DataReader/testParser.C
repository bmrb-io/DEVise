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
	aa = myDataReader->mySchema->qAttr;
	char* results = new char[2048];
	Status status = OK;

/**********************************************************************
	if (myDataReader->mySchema->getKeys() != NULL) {
		 cout << "KEYS : " << endl;
		 int cc; 
		 for (i = 0; i < myDataReader->mySchema->getKeys()->size() ; i++) {
			 for (aa = 0 ; aa < (*(myDataReader->mySchema->getKeys()))[i].size(); aa++) {
				 cout << (*(myDataReader->mySchema->getKeys()))[i][aa] << " " ;
			 }
			 cout << endl;
		 }
	}
	if (myDataReader->mySchema->getSorted() != NULL) {
		 cout << "SORTED : " << endl;
		 for (i = 0; i < myDataReader->mySchema->getSorted()->size(); i++) {
			 cout << (*(myDataReader->mySchema->getSorted()))[i] << " " ;
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
			switch (myDataReader->mySchema->tableAttr[i]->getType()) {
				case TYPE_INT:
					cout << *(int*)(results+(myDataReader->mySchema->tableAttr[i]->offset)) << " " ;
					break;
				case TYPE_DOUBLE:
					cout << *(double*)(results+(myDataReader->mySchema->tableAttr[i]->offset)) << " " ;
					break;
				case TYPE_STRING:
					tmp = results + (myDataReader->mySchema->tableAttr[i]->offset);
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

