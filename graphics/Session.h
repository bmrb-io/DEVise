/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2003
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
  Revision 1.32  2005/12/06 20:03:08  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.31  2003/01/13 19:25:11  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.30.2.2  2003/06/26 16:53:01  wenger
  Index file names for per-session data sources now include the session
  name (to reduce collisions); fixed a few memory problems relating to
  the data source catalogs.

  Revision 1.30.2.1  2002/12/03 23:58:18  wenger
  *** empty log message ***

  Revision 1.30  2002/03/11 23:06:10  wenger
  DEVised logs current session file on JavaScreen support errors.

  Revision 1.29  2002/01/15 21:49:41  wenger
  Added session postscript capability needed for the latest peptide-cgi
  improvements.

  Revision 1.28  2001/10/04 19:03:41  wenger
  JavaScreen support allows session files without .ds extension; various
  improvements to session file processing.

  Revision 1.27  2001/09/24 15:29:03  wenger
  Added warning if you close or quit with unsaved session changes (note
  that visual filter changes are not considered "changes").

  Revision 1.26  2001/03/23 18:06:31  wenger
  Color palettes are now associated with sessions; added borders to
  color chooser buttons so they're visible even if they're the same
  color as the background; fixed various color-related bugs.

  Revision 1.25  2001/01/08 20:32:42  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.23.2.1  2000/10/18 20:31:53  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.24  2000/10/16 16:11:39  wenger
  Link creation GUI now gives a choice of positive or negative record
  links; fixed bug 622 (record link type not saved in session files);
  fixed bug 623 (record link update problem); other link-related cleanups.

  Revision 1.23  2000/02/23 21:30:59  wenger
  Re-implemented session description capability.

  Revision 1.22  2000/02/08 22:11:48  wenger
  Added JAVAC_GetViewHelp and JAVAC_ShowViewHelp commands, added color
  edge grid, and type to JAVAC_DrawCursor command, JavaScreen protocol
  version now 4.0; added GUI to edit view help, and commands to save it
  to session files.

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

#include <tcl/tcl.h> // for ClientData

#include "DevStatus.h"
#include "Color.h" // for PaletteID
#include "SessionPostscript.h"

class ControlPanel;
class ControlPanelSimple;
class ArgList;
class DataCatalog;

class Session {
public:
  static DevStatus Open(const char *filename);
  static DevStatus Close();
  static DevStatus Save(const char *filename, Boolean asTemplate,
      Boolean asExport, Boolean withData, Boolean selectedView = false,
      Boolean savePostscript = true);
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

  // Get the name of the current session (if any).  Note that the
  // returned string must NOT be freed by the caller.
  static const char *GetCurrentSession() { return _sessionFile; }

  // Add a per-session data source.
  static DevStatus AddDataSource(const char *catName, const char *entry);

  // Show the data source catalog entry (if any) for the given source.
  // Note that the returned string must be freed with FreeString()
  // by the caller.
  static char *ShowDataSource(const char *sourceName);

  // Lists the data catalog, including per-session data sources.
  // Note that the returned string must be freed with delete [] by the caller.
  static char *ListDataCatalog(const char *catName);

  // Returns true iff the given data source is defined in the session data
  // catalog as opposed to the system one.
  static Boolean IsSessionDataSource(const char *sourceName);

  // Session description.
  static void SetDescription(const char *description);
  static const char *GetDescription();
  static DevStatus GetDescription(const char *filename, char * &description);

  static void SetDefaultPalette();
  static DevStatus CreateSessionPalette(const char *colors);

  static void SetDirty() { _dirty = true; }
  static void ClearDirty() { _dirty = false; }
  static Boolean IsDirty() { return _dirty; }
  
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
  static DevStatus DescriptionCommand(ControlPanelSimple *control, int argc,
      char *argv[]);

private:
  typedef DevStatus (*CommandProc)(ControlPanelSimple *control, int argc,
      char *argv[]);
  static DevStatus ReadSession(ControlPanelSimple *control,
      const char *filename, CommandProc cp);
  static DevStatus ReadSession(ControlPanelSimple *control,
      FILE *fp, CommandProc cp);
  static DevStatus CheckHeader(FILE *fp);
  static Boolean ReadCommand(FILE *fp, char buf[], int bufSize,
      DevStatus &status);

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

  static DevStatus SaveCategory(SaveData *saveData, const char *category);
  static DevStatus SaveClass(SaveData *saveData, const char *category,
      const char *devClass);
  static DevStatus SaveInstance(SaveData *saveData, const char *category,
      const char *devClass, const char *instance);

  typedef DevStatus (*InstanceFuncP)(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus ForEachInstance(const char *category, InstanceFuncP function,
      SaveData *saveData);

  static DevStatus SaveView(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveInterpMapping(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveGData(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveWindowLayout(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveViewAxisLabels(const char *category,
      const char *devClass, const char *instance, SaveData *saveData);
  static DevStatus SaveViewActions(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveViewLinks(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveCursor(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveViewMappings(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveWindowViews(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SavePileStack(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveViewHistory(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveCamera(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveStringTables(const char *category, const char *devClass,
      const char *instance, SaveData *saveData);
  static DevStatus SaveRecLinkTypes(SaveData *saveData);

  static DevStatus SaveParams(SaveData *saveData, const char *getCommand,
      const char *setCommand, const char *arg0, const char *arg1 = NULL,
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

  static char *_description;

  static PaletteID _defaultPalette;
  static PaletteID _sessionPalette;

  static Boolean _dirty;

  static SessionPostscript *_postscript;
};

#endif /* _Session_h_ */

/*============================================================================*/
