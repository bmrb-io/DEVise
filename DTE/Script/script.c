#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "ApInit.h" /* for DoInit */
#include "RTreeCommon.h"
#include "ncsa.h"

const int DETAIL = 1;

int main(int argc, char** argv){

	Init::DoInit();     // to initialize Devise reading stuff
	char* queryString = NULL;
     int entryCnt = post_query();
	if(entryCnt == 0){
		return 0;
	}
	bool execute = false;
	bool profile = false;
	char* shipping = NULL;
	char* table = NULL;
	for(int i = 0; i < entryCnt; i++){
		String option = entries[i].name;
		if(option == "execute"){
			execute = true;
		}
		else if(option == "profile"){
			profile = true;
		}
		else if(option == "query"){
			queryString = entries[i].val;
			int queryLength = strlen(queryString);
			queryString[queryLength - 1] = ';';
		}
		else if(option == "shipping"){
			shipping = entries[i].val;
		}
		else if(option == "table"){
			table = entries[i].val;
		}
		else{
			cout << "1 unknown option: " << option << endl;
			return 0;
		}
	}
	if(execute){
	}
	else if(profile){
		// Change the tables in from clause to the samples.
		// do the post processing.
		cout << "1 profile not implemented\n";
		return 0;
	}
	else{
		if(!shipping || !table){
			cout << "1 shipping table not specified\n";
		}
		// create and load local table;
		cout << "1 shipping not implemented\n";
		return 0;
	}

	String query(queryString);

     Engine engine(query);
     TRY(engine.optimize(), 0);
     int numFlds = engine.getNumFlds();
     String* types = engine.getTypeIDs();
	String* attrs = engine.getAttributeNames();
     Tuple* tup;

     cout << "0 OK\n";
     cout << numFlds << endl;
     for(int i = 0; i < numFlds; i++){
          cout << types[i] << " ";
     }
     cout << endl;
     for(int i = 0; i < numFlds; i++){
		cout << attrs[i] << " ";
     }
     cout << endl;
	cout << ";" << endl;

	engine.initialize();
     while((tup = engine.getNext())){
          for(int i = 0; i < numFlds; i++){
               displayAs(cout, tup[i], types[i]);
               cout << '\t';
          }
          cout << endl;
     }
	engine.finalize();
}

