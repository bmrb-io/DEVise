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
  Revision 1.2  1995/09/05 22:14:37  jussi
  Added CVS header.
*/

/*
   Composite parser for user to register a function to be called
   to decode composites
*/

#ifndef CompositeParser_h
#define CompositeParser_h

class RecInterp;
class AttrInfo;

class UserComposite {
public:
  /* This is called by the Composite parser to parse composite attributes */
  virtual void Decode(RecInterp *recInterp) = 0;
};

struct CompositeEntry {
  char *fileType;
  UserComposite *userComposite;
};

const int MAX_COMPOSITE_ENTRIES = 20;

class CompositeParser {
public:
  static void Register(char *fileType, UserComposite *userComposite);

  /* called by parser when it needs a composite attribute parsed */
  static void Decode(char *fileType, RecInterp *recInterp);

private:
  static CompositeEntry _entries[MAX_COMPOSITE_ENTRIES];
  static int _numEntries;
  static int _hintIndex;
};
#endif
