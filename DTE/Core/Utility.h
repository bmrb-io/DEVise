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

/*
  $Id$

  $Log$
  Revision 1.7  1997/08/21 21:04:28  donjerko
  Implemented view materialization

  Revision 1.6  1997/08/14 02:08:54  donjerko
  Index catalog is now an independent file.

  Revision 1.5  1997/07/30 21:39:22  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.4  1997/06/16 16:04:47  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.2  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
//#include <iostream.h>   erased for sysdep.h
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

// class Catalog;

string selectFileName(const string& env, const string& def);

// Catalog* getRootCatalog();

istream* getIndexTableStream();
ostream* getIndexTableOutStream(int mode = ios::out);

string stripQuotes(char* str);	// can throw excetion

void stripQuotes(istream& in, string& val);	// obsolete, can throw excetion

void stripQuotes(istream& in, char* buf, int bufsz);// can throw excetion

string addQuotes(const string& in);

string* dupStrArr(const string* inp, int numFlds);

#endif
