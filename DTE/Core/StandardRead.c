/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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

#include <strstream.h>
#include <string.h>
#include "StandardRead.h"
#include "exception.h"
#include "assert.h"
#include "url.h"

void StandardRead::open(){	// Throws exception
	(*in) >> numFlds;
	assert(in->good());
	typeIDs = new String[numFlds];
	attributeNames = new String[numFlds];
	readPtrs = new ReadPtr[numFlds];
	int i;
	for(i = 0; i < numFlds; i++){
		(*in) >> typeIDs[i];
		assert(in->good());
		readPtrs[i] = getReadPtr(typeIDs[i]);
	}
	for(i = 0; i < numFlds; i++){
		(*in) >> attributeNames[i];
		assert(in->good());
	}
	String statsStr;
	(*in) >> statsStr;
	assert(in->good());
	stats = new Stats(numFlds);
	if(statsStr == "stats"){
		stats->read(in);
	}
	else if(statsStr != "noStats"){
		String msg = "stats/noStats missing";
		THROW(new Exception(msg), );
	}
}


void NCDCRead::open(){	// Throws exception
     char buff[100];
     ostrstream err;
     while(in->good()){
          in->read(buff, 100);
          err.write(buff, in->gcount());
     }
     err << ends;
     char* response = err.str();
     char* tmp = response;

     // find the third occurence of "http:"

     assert(tmp);
     for(int i = 0; i < 3; i++){
          tmp = tmp + 1;
          tmp = strstr(tmp, "http:");
          if(!tmp){
               String msg = "Unexpected response from the NCDC server:\n" +
                    String(response);
               THROW(new Exception(msg), );
          }
     }
     int j = 0;
     while(tmp[j] != '"' && tmp[j]){
          j++;
     }
     assert(tmp[j]);
     tmp[j] = '\0';
     String tableUrlStr = String(tmp);
     delete response;
     URL* tableUrl = new URL(tableUrlStr);
	istream* tablein = NULL;
     TRY(tablein = tableUrl->getInputStream(), );
	delete tableUrl;

     // ignore first 8 lines

     for(int k = 0; k < 8; k++){
          assert(*tablein);
          tablein->getline(buff, 100);
     }

     numFlds = 2;
	typeIDs = new String[numFlds];
     typeIDs[0] = "int";
     typeIDs[1] = "int";
	attributeNames = new String[numFlds];
	readPtrs = new ReadPtr[numFlds];
	int i;
	for(i = 0; i < numFlds; i++){
		readPtrs[i] = getReadPtr(typeIDs[i]);
	}
	attributeNames[0] = "day";
	attributeNames[1] = "temp";
	stats = new Stats(numFlds);
	in = tablein;
}
