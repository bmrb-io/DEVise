/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/


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

        int numFlds;
	String* types;
	String* attrs;

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
	ReadPtr fieldRead;
	WritePtr fieldWrite;
	Type *field;

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
			// int queryLength = strlen(queryString);
			// queryString[queryLength - 1] = ';';
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

	sockBuf=new Cor_sockbuf(DTEserver, (unsigned short) DTEserverport);
	ostream out(sockBuf);
	out << queryString << " ~~| ";

	istream in(sockBuf);
	  
	out << "0 OK\n";

	in >> numFlds;
	
	for(int i = 0; i < numFlds; i++){
	  in >> types[i];
          cout << types[i] << " ";
	}

	cout << endl;

	for(int i = 0; i < numFlds; i++)
	  {
	    in >> attrs[i];
	    cout << attrs[i] << " ";
	  }
	cout << endl;
	cout << ";" << endl;

	String terminator;

	in >> terminator;

	while (!strcmp(terminator, "|~~"))
	  {
	    for(int i = 0; i < numFlds; i++)
	      {
		fieldRead=getReadPtr(types[i]);
		fieldRead(in, field);
		fieldWrite=getWritePtr(types[i]);
		fieldWritePtr(cout, field);
		cout << "\t";
	      }
	    cout << endl;
	  }

	delete sockBuf;
      }
