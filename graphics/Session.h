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
  Declaration of Session class (for opening and saving sessions).
 */

/*
  $Id$

  $Log$
  Revision 1.21  2000/01/14 18:23:08  wenger
  Added resetAllFilters and JAVAC_ResetFilters commands to reset all visual
  filters back to the values defined in the session file, without actually
  re-opening the session.

  Revision 1.20  1999/12/06 18:40:48  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.19  1999/11/30 22:28:04  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.18  1999/10/18 15:36:31  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.17  1999/10/04 22:36:08  wenger
  Fixed bug 508 (windows move slightly when repeatedly opening and saving
  a session) -- replaced kludgey (incorrect) way of dealing with window
  manager borders with correct way; user is warned if any windows extend
  off-screen when opening or saving a session.

  Revision 1.16  1999/09/23 15:46:24  wenger
  Added per-session data source capability:  data sources defined in a
  session file are added to a separate catalog which is delete when the
  session is closed; the "regular" and the per-session catalog are treated
  as a single catalog while the session is open.

  Revision 1.15  1999/09/08 20:56:22  wenger
  Removed all Tcl dependencies from the devised (main changes are in the
  Session class); changed version to 1.6.5.

  Revision 1.14  1999/03/04 15:10:57  wenger
  Implemented 'automatic filter update' features: views can be designated
  to have their visual filters automatically updated with the 'Update
  Filters' menu selection; alternatively, a session can be opened with
  the 'Open, Update, and Save' selection, which updates the designated
  filters and saves the updated session.

  Revision 1.13  1999/02/11 19:54:34  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.12.2.1  1999/02/11 18:24:02  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.12  1998/11/06 17:59:36  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.11  1998/09/30 17:44:33  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.10  1998/09/08 20:25:59  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

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
class ArgList;
class DataCatalog;

class Session {
public:
  static DevStatus Open(const char *filename);
  static DevStatus Close();
  static DevStatus Save(const char *filename, Boolean asTemplate,
      Boolean asExport, Boolean withData, Boolean selectedView = false);
  static DevStatus Update(const char *filename);
  static DevStatus ResetFilters();

  static DevStatus UpdateFilters();

  static DevStatus CreateTData(const char *name);

  // Whether current session (if any) was opened by JavaScreen.
  static Boolean GetIsJsSession() { return _isJsSession; }
  static void SetIsJsSession(Boolean isJsSession) {
      _isJsSession = isJsSession; }

  // Whether we're currently in the middle of opening a session.
  static Boolean OpeningSession() { return _openingSession; }

  static DevStatus AddDataSource(const char *catName, const char *entry);

  static char *ShowDataSource(const char *sourceName);

  static char *ListDataCatalog(const char *catName);
  
protected:
  friend class DeviseCommand_setOpeningSession;

  static void SetOpeningSession(Boolean openingSession) {
      _openingSession = openingSession; }

  friend class CommandLog;

  static Boolean IsBlankOrComment(const char *str);
  static void RemoveTrailingSemicolons(char str[]);
  static DevStatus RunCommand(ControlPanelSimple *control, int argc,
      char *argv[]);
  static DevStatus FilterCommand(ControlPanelSimple *control, int argc,
      char *argv[]);

private:
  typedef DevStatus (*CommandProc)(ControlPanelSimple *control, int argc,
      char *argv[]);
  static DevStatus ReadSession(ControlPanelSimple *control,
      const char *filename, CommandProc cp);
  static Boolean ReadCommand(FILE *fp, char buf[], int bufSize);

  static DevStatus DEViseCmd(ControlPanel *control, int argc,
      const char * const *argv);
  static DevStatus OpenDataSourceCmd(ControlPanel * control, int argc,
      char *argv[]);
  static DevStatus ScanDerivedSourcesCmd(ControlPanel * control, int argc,
      char *argv[]);
  static DevStatus SetDescriptionCmd(ControlPanel * control, int argc,
      char *argv[]);

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
  static DevStatus SavePileStack(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveViewHistory(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveCamera(char *category, char *devClass,
      char *instance, SaveData *saveData);
  static DevStatus SaveStringTables(char *category, char *devClass,
      char *instance, SaveData *saveData);

  static DevStatus SaveParams(SaveData *saveData, char *getCommand,
      char *setCommand, const char *arg0, const char *arg1 = NULL,
      const char *arg2 = NULL, Boolean addBraces = false);

  static DevStatus CallParseAPI(ControlPanelSimple *control,
      const char *&result, Boolean splitResult, ArgList &args,
	  const char *arg0, const char *arg1 = NULL, const char *arg2 = NULL,
	  const char *arg3 = NULL);

  static DataCatalog *GetDataCatalog();
  static DevStatus SaveDataSources(FILE *fp);
  static DevStatus DeleteDataSources();

  static DevStatus CheckWindowLocations();

  static Boolean _isJsSession;
  static Boolean _openingSession;

  static DataCatalog *_dataCat;
  static char *_catFile;
  static char *_sessionFile;
};

#endif /* _Session_h_ */

/*============================================================================*/
