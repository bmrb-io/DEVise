#ifndef MARSHAL_H
#define MARSHAL_H

#include <assert.h>
#include <iostream.h>
#include <strstream.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "types.h"

const int PAGE_SIZE = 10;

class Tuple{
     int numFields;
	Type** fields;
public:
	Tuple(int num){
		numFields = num;
		fields = new (Type*)[num];
	}
	Type*& operator[](int i){
		assert(i < numFields);
		return fields[i];
	}
     void display(ostream& out, String* types, int numFields){
		out << '(';
        int i;
		for(i = 0; i < numFields - 1; i++){
			displayAs(out, fields[i], types[i]);
			out << ", ";
		}
		if(i == numFields - 1){
			displayAs(out, fields[i], types[i]);
		}
		out << ')';
	}
};

class DummyUnmarshal /*: public Unmarshal*/ {
     Tuple** tupArray;
	int cnt;
	int current;
public:
	DummyUnmarshal(String schema, String data) /*: Unmarshal()*/{
		// cout << "DummyUnmarshal called with: " << schema << ", " 
		//	<< data << endl;
		cnt = 2;
		tupArray = new (Tuple*)[cnt];
		tupArray[0] = new Tuple(2);
		tupArray[1] = new Tuple(2);
		(*tupArray[0])[0] = new IInt(10);
		(*tupArray[0])[1] = new IString("hello");
		(*tupArray[1])[0] = new IInt(15);
		(*tupArray[1])[1] = new IString("world");
		current = 0;
	}
	int getNumFlds(){
		return 2;
	}
	String* getAttNames(){
		String* rv = new String[2];
		rv[0] = "i";
		rv[1] = "s";
		return rv;
	}
	String* getTypeIDs(){
		String* rv = new String[2];
		rv[0] = "IInt";
		rv[1] = "IString";
		return rv;
	}
	Tuple* getNext(){
		if(current < cnt){
			return tupArray[current++];
		}
		else{
			return NULL;
		}
	}
};

#endif
