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
  Revision 1.12  1998/03/17 17:19:06  donjerko
  Added new namespace management through relation ids.

  Revision 1.11  1998/02/17 23:09:13  donjerko
  *** empty log message ***

  Revision 1.10  1997/12/04 04:05:17  donjerko
  *** empty log message ***

  Revision 1.9  1997/11/12 23:17:38  donjerko
  Improved error checking.

  Revision 1.8  1997/09/05 22:20:14  donjerko
  Made changes for port to NT.

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

void stripQuotes(const char* from, char* to, size_t len); // can throw excetion

void stripQuotes(istream& in, string& val);	// obsolete, can throw excetion

void stripQuotes(istream& in, char* buf, size_t bufsz);// can throw excetion

string addQuotes(const string& in);

string stripSQLQuotes(const char* input);

string addSQLQuotes(const char* inp, char quote);

string* dupStrArr(const string* inp, int numFlds);

bool fileExists(const string& fileNm);
#endif
