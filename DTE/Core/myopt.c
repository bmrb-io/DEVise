#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include "types.h"
#include "exception.h"
#include "Engine.h"

const int DETAIL = 1;

int main(int argc, char** argv){

	String query;
	char c = '\0';
	char l = '\0';
	while(1){
		cin.get(c);
		if(l == ';' && c == '\n'){
			break;
		}
		query += c;
		l = c;
	}
	iTimer.reset();
	cout << "Query in main is: " << query << endl; 
	Engine engine(query);
	TRY(engine.optimize(), 0);
	int numFlds = engine.getNumFlds();
	String* types = engine.getTypeIDs();
	String* attrs = engine.getAttributeNames();
	for(int i = 0; i < numFlds; i++){
		cout << attrs[i] << " ";
	}
	cout << endl << "---------------------------------" << endl;
	Tuple* tup;

	while((tup = engine.getNext())){
		for(int i = 0; i < numFlds; i++){
			displayAs(cout, tup[i], types[i]);
			cout << '\t';
		}
		cout << endl;
	}
	LOG(logFile << "Query completed at ");
	LOG(iTimer.display(logFile));
}
