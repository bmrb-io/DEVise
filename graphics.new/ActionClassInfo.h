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
  Revision 1.1  1996/05/07 16:35:15  jussi
  Moved files from graphics directory.

  Revision 1.4  1996/01/30 21:10:15  jussi
  Removed extraneous Changeable() which is inherited from ClassInfo.

  Revision 1.3  1995/09/05 21:12:21  jussi
  Added/updated CVS header.

  Revision 1.2  1995/09/05 20:39:20  jussi
  Added CVS header.
*/

#ifndef ActionClassInfo_h
#define ActionClassInfo_h

#include "DeviseTypes.h"
#include "ClassDir.h"

class Action;
class GenAction;

class ActionClassInfo: public ClassInfo {
public:
  ActionClassInfo(const char *className, GenAction *gen);
  ActionClassInfo(const char *className, const char *instName, Action *action);
  virtual ~ActionClassInfo();

  /* Info for category */
  virtual const char *CategoryName() { return "action"; }

  /* Info for class */
  virtual const char *ClassName(); 	/* name of class */

  /* Get name of parameters and default/current values */
  virtual void ParamNames(int &argc, const char **&argv);

  /* Create instance using the supplied parameters. Return 
     the instance info if successful, otherwise return NULL. */
  virtual ClassInfo *CreateWithParams(int argc, const char * const *argv);
  
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

private:
  GenAction *_gen;
  const char *_className, *_instName;
  Action *_action;
};

#endif
