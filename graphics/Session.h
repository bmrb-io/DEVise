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
  Declaration of Session class (for opening and saving sessions).
 */

/*
  $Id$

  $Log$
  Revision 1.9  1998/05/14 18:21:09  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.8  1998/04/28 18:02:40  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.7  1997/10/03 14:37:04  wenger
  Various fixes to get session opening/saving to work with client/server
  version; reading old-style (Tcl) session files now works in back end;
  got back-end session file stuff working for multi.

  Revision 1.6  1997/10/02 18:46:31  wenger
  Opening and saving batch-style sessions in back end now fully working;
  added tk2ds.tcl script for conversion.

  Revision 1.5  1997/09/23 19:57:26  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.4  1997/09/19 21:29:54  wenger
  Did various cleanups that didn't affect functionality.

  Revision 1.3  1997/09/19 20:04:11  wenger
  Now saving complete session info; works for tables as well as unixfiles;
  derived data not yet tested.

  Revision 1.2  1997/09/18 15:15:18  wenger
  Now writes a useable session file (at least in some cases).

  Revision 1.1  1997/09/17 18:44:00  wenger
  Opening sessions works except for Tcl interpreter handling result; saving
  is most of the way there.

 */

#ifndef _Session_h_
#define _Session_h_


#include <sys/types.h>

#include <tcl.h> // for ClientData

#include "DevStatus.h"

class ControlPanel;
class ControlPanelSimple;

class Session {
public:
  static DevStatus Open(char *filename);
  static DevStatus Save(char *filename, Boolean asTemplate, Boolean asExport,
      Boolean withData, Boolean selectedView = false);
  static DevStatus CreateTData(char *name);

private:
  static int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
      int argc, char *argv[]);
  static int OpenDataSourceCmd(ClientData clientData, Tcl_Interp *interp,
      int argc, char *argv[]);
  static int scanDerivedSourcesCmd(ClientData clientData, Tcl_Interp *interp,
      int argc, char *argv[]);
  static int SetDescriptionCmd(ClientData clientData, Tcl_Interp *interp,
      int argc, char *argv[]);

  struct SaveData {
    ControlPanelSimple *control;
    FILE *fp;
  };

  static DevStatus SaveCategory(SaveData *saveData, char *category);
  static DevStatus SaveClass(SaveData *saveData, char *category,
      char *devClass);
  static DevStatus SaveInstance(SaveData *saveData, char *category,
      char *devClass, char *instance);

  typedef DevStatus (*InstanceFuncP)(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus ForEachInstance(char *category, InstanceFuncP function,
      SaveData *saveData);

  static DevStatus SaveView(char *category, char *devClass, char *instance,
      SaveData *saveData);
  static DevStatus SaveInterpMapping(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveGData(char *category, char *devClass, char *instance,
      SaveData *saveData);
  static DevStatus SaveWindowLayout(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewAxisLabels(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewActions(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewLinks(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveCursor(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewMappings(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveWindowViews(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewHistory(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveCamera(char *category, char *devClass,
      char *instance, SaveData *saveData);

  static DevStatus SaveParams(SaveData *saveData, char *getCommand,
      char *setCommand, char *arg0, char *arg1 = NULL, char *arg2 = NULL,
      Boolean addBraces = false);

  static DevStatus CallParseAPI(ControlPanelSimple *control, char *&result,
      Boolean splitResult, int &argcOut, char **&argvOut, char *arg0,
      char *arg1 = NULL, char *arg2 = NULL, char *arg3 = NULL);
};

#endif /* _Session_h_ */

/*============================================================================*/
