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
  Revision 1.3  1996/01/30 21:10:07  jussi
  Removed extraneous Changeable() which is inherited from ClassInfo.

  Revision 1.2  1995/09/05 21:13:29  jussi
  Added/updated CVS header.
*/

#ifndef VisualLinkClassInfo_h
#define VisualLinkClassInfo_h

#include "ClassDir.h"
#include "VisualArg.h"

class VisualLink;

class VisualLinkClassInfo: public ClassInfo {
public:
  VisualLinkClassInfo();
  VisualLinkClassInfo(char *name, VisualFlag flag, VisualLink *link);
  virtual ~VisualLinkClassInfo();

  /* Info for category */
  virtual char *CategoryName() { return "link"; } 

  /* Info for class */
  virtual char *ClassName(); 	/* name of class */

  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, char **&argv);

  /* Create instance using the supplied parameters. Return 
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, char **argv) ;

  /* Set default parameters */
  void SetDefaultParams(int argc, char **argv);

  /* Get default parameters */
  void GetDefaultParams(int &argc, char **&argv);

  /**************************************************
    Instance Info. 
  ***************************************************/
  virtual char *InstanceName();
  virtual void *GetInstance();
  
  /* Get parameters that can be used to re-create this instance */
  virtual void CreateParams(int &argc, char **&argv);

private:
  VisualFlag _flag;
  char *_name;
  VisualLink *_link;
};

#endif
