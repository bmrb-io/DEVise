#include <iostream.h>
#include <fstream.h>
#include "DataReader.h"
//#include <string>

int main() {

	char* df = "../../data/strings.dat";
	char* sf = "str.uni";
	
	int aa,i;
	char* tmp;
	DataReader* myDataReader = new DataReader(df,sf);
	if (!(myDataReader->isOk())) {
		exit(1);
	}
	aa = myDataReader->mySchema->qAttr;
	char* results = new char[1024];
	int* offs = new int[3];
	offs[0] = 0;
	offs[1] = 10;
	offs[2] = 20;
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

	while (true) {
		status = myDataReader->getRecord(results,offs);
		switch (status) {
			case FAIL:
				cerr << "Error Occured in DataReader" << endl ;
				return 1;
		}
		for ( i = 0 ; i < aa ; i++) {
			switch (myDataReader->mySchema->tableAttr[i]->getType()) {
				case TYPE_INT:
					cout << *(int*)(results+offs[i]) << " " ;
					break;
				case TYPE_DOUBLE:
					cout << *(double*)(results+offs[i]) << " " ;
					break;
				case TYPE_STRING:
					tmp = results + offs[i];
					cout << tmp << " " ;
			}
		}
		cout << "->  Got One" << endl ;
		if (status == FOUNDEOF) {
			cerr << "FOUND EOF" << endl;
			return 1;
		}
	}
	return 1;
}

