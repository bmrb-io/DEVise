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
  Revision 1.21  1997/08/14 02:08:55  donjerko
  Index catalog is now an independent file.

  Revision 1.20  1997/08/12 19:58:44  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.19  1997/08/10 20:30:56  donjerko
  Fixed the NO_RTREE option.

  Revision 1.18  1997/08/09 00:54:45  donjerko
  Added indexing of select-project unmaterialized views.

  Revision 1.17  1997/07/22 15:00:55  donjerko
  *** empty log message ***

  Revision 1.16  1997/06/30 23:05:05  donjerko
  CVS:

  Revision 1.15  1997/06/21 22:48:06  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.14  1997/06/16 16:04:48  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.11  1997/02/25 22:14:53  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.10  1997/02/18 18:06:03  donjerko
  Added skeleton files for sorting.

  Revision 1.9  1997/02/03 04:11:34  donjerko
  Catalog management moved to DTE

  Revision 1.8  1996/12/24 21:00:52  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.7  1996/12/21 22:21:47  donjerko
  Added hierarchical namespace.

  Revision 1.6  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.5  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#ifndef CATALOG_H
#define CATALOG_H

#include <assert.h>
#include <limits.h>
#include "queue.h"
#include "exception.h"
#include "Utility.h"

const int INFINITY = INT_MAX;

class Inserter;
class Site;
class TableName;
class Interface;

class Directory {
	string fileName;
public:
	Directory(const string& fileName) : fileName(fileName) {}
	~Directory(){}
	Interface* createInterface(const string& entry);
	// throws

	void remove(const string& entry);
	void insert(const string& entry, const Interface* interf);
	void replace(const string& entry, const Interface* interf);
	// throws
};

class Catalog {
	string fileName;
public:
	Catalog(const string& fileName) : fileName(fileName) {}
	Site* find(TableName* path) const;     // Throws Exception
	Interface* createInterface(TableName* path) const;  // Throws exception
};

extern const Catalog ROOT_CATALOG;

#endif
