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
  Development Group.sel
*/

#include "InitShut.h"
#include "Common.h"
#include "sysdep.h"
#include "exception.h"

int main(int argc, char** argv){

     initialize_system();

	string query = extractQuery(cin);
	EXIT(processQuery(query, cout));

     shutdown_system();
	return 0;
}
