/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2001
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
  Revision 1.17  2000/01/13 23:06:50  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.16  1999/06/30 17:38:39  wenger
  Data color of parent view's mapping (if specified) now controls the
  background color of view symbols; defined constant strings for GData
  attribute values to avoid potential problems.

  Revision 1.15  1998/11/25 22:31:11  wenger
  Reduced the amount of memory allocated in the MappingInterp constructor,
  mainly by improving the AttrList class.

  Revision 1.14  1998/10/21 17:16:29  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.13  1998/10/13 19:40:35  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.12  1998/07/29 14:20:18  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.11  1997/05/28 20:01:42  andyt
  Shape attributes for 'Tcl/Tk Window' shape can now be variables, numeric
  constants, or string constants. Used to assume that all attributes except
  argc were strings.

  Revision 1.10  1997/04/25 22:46:54  ssl
  Added a method to get attr num given its name.

  Revision 1.9  1997/03/07 17:41:40  donjerko
  Added method Clear() that resets attrList to empty.

  Revision 1.8  1996/11/23 21:22:46  jussi
  Removed Config.h. Includes Init.h or ApInit.h instead.

  Revision 1.7  1996/07/23 20:12:41  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.6  1996/05/07 16:14:19  jussi
  Added copy constructor and GetVal() method.

  Revision 1.5  1996/04/19 19:06:35  wenger
  Added DEVise copyright notice, etc.

  Revision 1.4  1995/12/20 07:04:28  ravim
  High and low values of attrs can be specified.

  Revision 1.3  1995/09/05 21:12:23  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:21  jussi
  Added CVS header.
*/

#ifndef AttrList_h
#define AttrList_h

#if defined(OSF)
//#   include <pthread.h>
#endif
#include <sys/time.h>

#include "DeviseTypes.h"
#include "DList.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#endif

#define REC_ID_NAME "recId"

enum AttrType { InvalidAttr, IntAttr, FloatAttr, DoubleAttr, StringAttr,
  DateAttr };

union AttrVal {
	double doubleVal;          /* double value */
	float floatVal;            /* float value */
	int intVal;                /* integer value */
	char *strVal;              /* string value */
	time_t dateVal;            /* date value */
};

struct AttrInfo {
	char *name;                /* name of attribute */
	int attrNum;               /* attribute number, starting from 0 */
	int offset;                /* offset from beginning of record */
	int length;                /* max length of attribute */
	Boolean isComposite;       /* true if this attribute is a composite
				      requiring user defined parser. */
	Boolean isSorted;          /* true if this attirubte is the
				      sort attribute */
	AttrType type;             /* attribute type */
	Boolean hasMatchVal;       /* true if matching value specified */
	AttrVal matchVal;          /* matching value */
	Boolean hasHiVal;          /* true if high value specified */
	AttrVal hiVal;             /* high value */
	Boolean hasLoVal;          /* true if low value specified */
	AttrVal loVal;             /* low value */
};

const int MIN_ATTRLIST_SIZE = 32;

class AttrList {
public:
  AttrList(const char *name, int startSize = MIN_ATTRLIST_SIZE);
  ~AttrList();

  /* Copy constructor */
  AttrList(AttrList &attrs);

  /* Copy attributes from another AttrList. */
  Boolean SetAttrs(const AttrList &newAttrs);

  /* Insert attribute into list of attributes */
  void InsertAttr(int attrNum,
		  const char *name, int offset, int length, AttrType type,
		  Boolean hasMatchVal = false,
		  AttrVal *matchVal = (AttrVal *)NULL,
		  Boolean isComposite = false,
		  Boolean isSorted = false,
		  Boolean hasHiVal = false, 
		  AttrVal *hiVal = (AttrVal *)NULL, 
		  Boolean hasLoVal = false, 
		  AttrVal *loVal = (AttrVal *)NULL);

  const char *GetName() { return _name; }

  /* Find an attribute, or NULL if not found */
  AttrInfo *Find(const char *name);

  /* Find a shape attribute, or NULL if not found */
  AttrInfo *FindShapeAttr(int i);

  /* Get ith attribute info */
  AttrInfo *Get(int index);

  /* Get attr number, given its name */
  int GetAttrNum(char *name);

  /* Get # of attributes */
  int NumAttrs() { return _size;}

  /* iterator for list of attributes */
  void InitIterator();
  Boolean More();
  AttrInfo *Next();
  void DoneIterator();

  void Print();
  static void PrintAttr(const AttrInfo *info);
  void Write(int fd);

  static double GetVal(const AttrVal *aval, AttrType atype);

  void AttrList::Clear();

private:
  AttrInfo **_attrs;
  int   _arraySize; // size of _attrs array
  int   _size; // actual number of entries
  int   _index;
  const char *_name;

  static void PrintVal(const AttrVal *aval, AttrType atype);
  void EnlargeArray();
};

#endif
