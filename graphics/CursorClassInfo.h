/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.5  2000/02/16 18:51:20  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.4  1997/07/22 15:36:23  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.3  1995/12/29 18:29:48  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:35  jussi
  Added/updated CVS header.
*/

#ifndef CursorClassInfo_h
#define CursorClassInfo_h

#include "ClassDir.h"
#include "VisualArg.h"
#include "DList.h"

class CursorClassInfo;
DefinePtrDList(DevCursorList, CursorClassInfo *);

class DevCursor{
public:
  static int GetCount() { return _cursorList.Size(); }

  /* Read-only access to link list. */
  static int InitIterator() { return _cursorList.InitIterator(); }
  static Boolean More(int index) { return _cursorList.More(index); }
  static CursorClassInfo *Next(int index) { return _cursorList.Next(index); }
  static void DoneIterator(int index) { _cursorList.DoneIterator(index); }

  static void Dump(FILE *fp);

protected:
  friend class CursorClassInfo;

  static DevCursorList _cursorList;
};

class DeviseCursor;
class CursorClassInfo: public ClassInfo {
public:
  /* specify VISUAL_X, VISUAL_Y, VISUAL_LOC */
  CursorClassInfo();
  CursorClassInfo(const char *name, VisualFlag flag, DeviseCursor *cursor);
  virtual ~CursorClassInfo();

  /* Info for category */
  virtual const char *CategoryName() { return "cursor"; } 

  /* Info for class */
  virtual const char *ClassName(); 	/* name of class */
  
  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, const char **&argv);
  
  /* Create instance using the supplied parameters. Return 
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv);
  
  /* Return true is parameters can be changed dynamically at run time */
  virtual Boolean Changeable() { return false; }
  
  /* Set default parameters */
  void SetDefaultParams(int argc, const char * const *argv);
  
  /* Get default parameters */
  void GetDefaultParams(int &argc, const char **&argv);
  
  
  /**************************************************
    Instance Info. 
  ***************************************************/
  virtual const char *InstanceName();
  virtual void *GetInstance();

  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, const char **&argv);

  void Dump(FILE *fp);

private:
  // Note: I'm not sure if we really need _flag and _name here, since
  // those are also stored in the Cursor object (and _flag in that object
  // can get changed).  wenger 2013-02-21
  VisualFlag _flag;
  const char *_name;
  DeviseCursor *_cursor;
};

#endif
