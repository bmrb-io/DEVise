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

#ifdef DTE_AS_SERVER
  #include "SockStream.h"
  #include "common.h"
#else
  #define out cout
#endif

const int DETAIL = 1;

int main(int argc, char** argv){

#ifndef DTE_AS_SERVER

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

#else   // ifndef DTE_AS_SERVER

  String query, temperory, keeped="";

  Cor_sockbuf *sockBuf=new Cor_sockbuf((unsigned short) serverport);
  Cor_sockbuf *accepted;

  for (;;)
    {

  accepted=sockBuf->AcceptConnection();
  keeped="";


  istream in(accepted);
  in >> temperory;
  while (strcmp(temperory, "~~|")!=0)
    {

    keeped+=temperory;
    keeped+=" ";

    }

  query=keeped;

  ostream out(accepted);

#endif  // ifndef DTE_AS_SERVER

// so far query is set

     Engine engine(query);
     TRY(engine.optimize(), 0);
     int numFlds = engine.getNumFlds();
	assert(numFlds > 0);
	WritePtr* writePtrs = engine.getWritePtrs();
     String* types = engine.getTypeIDs();
	String* attrs = engine.getAttributeNames();
     Tuple tup[numFlds];

#ifndef DTE_AS_SERVER
     out << "0 OK\n";
#endif 

     out << numFlds << endl;

     for(int i = 0; i < numFlds; i++){
          out << types[i] << " ";
     }
     out << endl;
     for(int i = 0; i < numFlds; i++){
		out << attrs[i] << " ";
     }
     out << endl;
	out << ";" << endl;

	engine.initialize();
     while(engine.getNext(tup)){

#ifdef DTE_AS_SERVER
       out << "|~~ ";
#endif

          for(int i = 0; i < numFlds; i++){
			writePtrs[i](out, tup[i]);
               out << '\t';
          }
          out << endl;
     }

#ifdef DTE_AS_SERVER
       out << "~~| ";
#endif

	engine.finalize();

#ifdef DTE_AS_SERVER
    delete accepted;
  }
  delete  sockBuf;
#endif
	}



