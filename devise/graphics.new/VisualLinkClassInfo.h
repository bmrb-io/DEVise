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
  Revision 1.3  1998/03/08 00:01:17  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.2  1997/07/22 15:36:38  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.1  1996/05/31 15:37:43  jussi
  Moved to the graphics.new directory.

  Revision 1.3  1996/01/30 21:10:07  jussi
  Removed extraneous Changeable() which is inherited from ClassInfo.

  Revision 1.2  1995/09/05 21:13:29  jussi
  Added/updated CVS header.
*/

#ifndef VisualLinkClassInfo_h
#define VisualLinkClassInfo_h

#include "ClassDir.h"
#include "VisualArg.h"
#include "DList.h"

class VisualLinkClassInfo;
DefinePtrDList(DevLinkList, VisualLinkClassInfo *);

class DevLink {
public:
  static int GetCount() { return _linkList.Size(); }

  /* Read-only access to link list. */
  static int InitIterator() { return _linkList.InitIterator(); }
  static Boolean More(int index) { return _linkList.More(index); }
  static VisualLinkClassInfo *Next(int index) { return _linkList.Next(index); }
  static void DoneIterator(int index) { _linkList.DoneIterator(index); }

  static void Dump(FILE *fp);

protected:
  friend class VisualLinkClassInfo;

  static DevLinkList _linkList;
};

class DeviseLink;

class VisualLinkClassInfo: public ClassInfo {
public:
  VisualLinkClassInfo();
  VisualLinkClassInfo(const char *name, VisualFlag flag, DeviseLink *link);
  virtual ~VisualLinkClassInfo();

  /* Info for category */
  virtual const char *CategoryName() { return "link"; } 

  /* Info for class */
  virtual const char *ClassName(); 	/* name of class */

  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, const char **&argv);

  /* Create instance using the supplied parameters. Return 
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv) ;

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
  VisualFlag _flag;
  const char *_name;
  DeviseLink *_link;
};

#endif
