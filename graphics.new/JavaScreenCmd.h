/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of JavaScreenCmd and related classes (handle commands from
  JavaScreen client).
 */

/*
  $Id$

  $Log$
  Revision 1.35  2000/04/06 17:53:47  wenger
  Minor changes for setup on Eldon's laptop.

  Revision 1.34  2000/03/28 21:46:25  wenger
  Started implementation of caching commands, GIFs, and GData to speed up
  session opening.

  Revision 1.33  2000/03/27 21:42:34  wenger
  Removed unused JavaScreenCmd::DrawAllCursors() method.

  Revision 1.32  2000/03/17 14:25:12  wenger
  Removed unused JAVAC_DeleteView command.

  Revision 1.31  2000/03/08 21:23:33  wenger
  Fixed bug 568 (crash when opening some sessions in the JavaScreen):
  reset devised timeout in jspop to a better value; JavaScreenCmd now
  deals better with the possibility of a communications failure.

  Revision 1.30  2000/02/23 18:57:53  wenger
  Added option to do checksum on GIFs and GData sent to JavaScreen.

  Revision 1.29  2000/02/08 22:12:00  wenger
  Added JAVAC_GetViewHelp and JAVAC_ShowViewHelp commands, added color
  edge grid, and type to JAVAC_DrawCursor command, JavaScreen protocol
  version now 4.0; added GUI to edit view help, and commands to save it
  to session files.

  Revision 1.28  2000/01/14 18:23:17  wenger
  Added resetAllFilters and JAVAC_ResetFilters commands to reset all visual
  filters back to the values defined in the session file, without actually
  re-opening the session.

  Revision 1.27  1999/10/27 17:59:54  wenger
  Fixed bug 525 (child views can't send GData to the JavaScreen); started
  on provision for drill-down in 3D views in the JavaScreen.

  Revision 1.26  1999/08/25 14:56:17  wenger
  More improvements to JavaScreen argument handling; assertion failures are
  written to debug log.

  Revision 1.25  1999/08/24 22:01:19  wenger
  JavaScreen support code deals with argument lists better (partly in
  preparation for JS-side axis drawing).

  Revision 1.24  1999/08/19 20:46:37  wenger
  Added JAVAC_ProtocolVersion command.

  Revision 1.23  1999/08/19 13:54:26  wenger
  Changes for JavaScreen support: all 15 shape attributes now sent in
  GData; added zoom in/out argument to JAVAC_MouseAction_RubberBand;
  JAVAC_MouseAction_DoubleClick changed to JAVAC_ShowRecords.

  Revision 1.22  1999/06/22 19:46:59  wenger
  Devised support for JavaScreen improvements: cursors are now drawn in any
  view of a pile; mouse actions can be disabled in views; cursor grid info
  is sent; JAVAC_CursorChanged command sends cursor name instead of view
  name; added dummy mouse grid info (not yet implemented).  Fixed some
  problems with how the devised deals with piles for the JavaScreen.

  Revision 1.21  1999/06/10 19:59:22  wenger
  Devised sends axis type info to JS even if axes aren't drawn (so JS can
  display cursor position properly); added code to send cursor grid info
  and action disabling info (conditionaled out until the JS is ready for it);
  fixed bug with cursor Y grid value in cursor creation.

  Revision 1.20  1999/05/04 17:17:08  wenger
  Merged js_viewsyms_br thru js_viewsyms_br_1 (code for new JavaScreen
  protocol that deals better with view symbols).

  Revision 1.19.4.2  1999/02/24 21:02:49  wenger
  Cursor drawing for JavaScreen is now working again (testing without the
  JavaScreen).

  Revision 1.19.4.1  1999/02/24 17:48:39  wenger
  Changes for view symbols in the JavaScreen are largely working (running
  without the JavaScreen), except for cursor drawing (code is currently
  configured to test JavaScreen support commands with monolithic executable).

  Revision 1.19  1998/12/21 16:33:40  wenger
  Added more support for cursors, and view axes and titles, for the JavaScreen.

  Revision 1.18  1998/12/10 21:53:27  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.17  1998/10/01 17:54:17  wenger
  Implemented the sending of GData to the JavaScreen.

  Revision 1.16  1998/09/15 17:34:07  wenger
  Changed JavaScreen communication protocol so that image data is sent
  after all associated commands (fixes bug 387); made opening a session
  and updating a session more similar to each other; fixed some other
  JavaScreen support bugs.

  Revision 1.15  1998/09/14 14:57:56  wenger
  Reorganized code somewhat for clarity (no change in functionality except
  for some better error checking).

  Revision 1.14  1998/09/08 16:55:14  wenger
  Improved how JavaScreenCmd handles closing sessions -- fixes some problems
  with client switching.

  Revision 1.13  1998/09/01 20:13:19  wenger
  Fixed problems with sometimes sending incorrect cursor coordinates to
  JavaScreen and sending DrawCursor commands before windows are created;
  removed extra braces on error messages to JavaScreen; changed from
  sending x0, y0, x1, y1 to x0, y0, width, height in JAVAC_CreateWindow
  commands.

  Revision 1.12  1998/08/25 20:56:32  wenger
  Implemented support for JavaScreen cursors (not yet fully tested).

  Revision 1.11  1998/08/24 14:51:33  wenger
  Implemented support for JavaScreen drill-down.

  Revision 1.10  1998/08/13 18:14:46  wenger
  More updates to JavaScreen support.

  Revision 1.9  1998/08/03 18:38:40  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.8  1998/06/17 17:20:42  wenger
  Devised now sends "real" session file list to JavaScreen.

  Revision 1.7  1998/06/03 17:09:41  wenger
  Rubberband line in JavaScreen now sends updates of all changed windows
  using the "dirty GIF" flag; updated DEVise version to 1.5.3.

  Revision 1.6  1998/05/29 19:34:40  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.5  1998/05/14 18:21:44  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.4  1998/05/08 17:16:23  taodb
  Added stripping functions for "{}"

  Revision 1.3  1998/05/05 17:07:25  wenger
  Minor improvements to JavaScreenCmd.[Ch].

  Revision 1.2  1998/05/02 09:00:45  taodb
  Added support for JAVA Screen and command logging

  Revision 1.1  1998/04/25 05:45:34  taodb
  Initial Revision

 */
#ifndef _JAVA_SCREEN_CMD
#define _JAVA_SCREEN_CMD

#include <stdio.h>
#include <string>

#include "DeviseTypes.h"

typedef double TDataVal;
typedef string GDataVal;

class DeviseServer;
class ControlPanel;
class ViewGraph;

class DeviseCursor;


class JavaScreenCmd
{
	friend class View;
	friend class JSArgs;

	public:
		typedef enum 
		{
			GETSESSIONLIST, 
			OPENSESSION, 
			MOUSEACTION_CLICK,
			SHOW_RECORDS, 
			SHOW_RECORDS3D, 
			MOUSEACTION_RUBBERBAND,
			JAVAEXIT,
			CLOSECURRENTSESSION,
			SETDISPLAYSIZE,
			KEYACTION,
			SAVESESSION,
			SERVEREXIT,
			SERVERCLOSESOCKET,
			IMAGECHANNEL,
			CURSORCHANGED,
			PROTOCOLVERSION,
			RESET_FILTERS,
			GET_VIEW_HELP,
			NULL_SVC_CMD
		}ServiceCmdType;

		typedef enum
		{
			UPDATESESSIONLIST = 0,
			UPDATERECORDVALUE,
			UPDATEGDATA,
			DRAWCURSOR,
			ERASECURSOR,
			CREATEVIEW,
			DELETECHILDVIEWS,
			VIEWDATAAREA,
			UPDATEVIEWIMAGE,
			SHOW_VIEW_HELP,

			DONE,
			ERROR,
			FAIL,

			CONTROLCMD_NUM,
			NULL_COMMAND
		}ControlCmdType;

		// argv does not contain the command name!
		JavaScreenCmd(ControlPanel* server,
			ServiceCmdType ctype, int argc, char** argv);
		~JavaScreenCmd();

		int Run();
		static char* JavaScreenCmdName(JavaScreenCmd::ControlCmdType);

	protected:
		// < 0 if error
		int ReturnVal(int argc, char** argv);

	private:
		static char* _controlCmdName[CONTROLCMD_NUM];
		ControlPanel	*_control;
		int				_ctype;
		int				_argc;
		char** 			_argv;

		ControlCmdType	_status;
		const char		*errmsg;

		static Boolean	_postponeCursorCmds;

		Boolean			_recording;	// true iff recording commands and data
		DevStatus		_recordingStatus;
		char *			_commandFileName;
		FILE *			_commandFile;
		char *			_dataFileName;
		int				_dataFile;

		Boolean 		_playingBack; // true iff playing back commands and data

		// JavaScreen->Server Requests
		void GetSessionList();
		void CloseCurrentSession();
		void OpenSession();
		void MouseAction_Click();
		void ShowRecords();
		void ShowRecords3D();
		void MouseAction_RubberBand();
		void SetDisplaySize();
		void KeyAction();
		void SaveSession();
		void ServerExit();
		void ServerCloseSocket();
		void ImageChannel();
		void CursorChanged();
		void JSProtocolVersion();
		void JSResetFilters();
		void GetViewHelp();

		// Server->JavaScreen Control Commands
		ControlCmdType RequestUpdateSessionList(int argc, char** argv);
		ControlCmdType RequestUpdateGData(ViewGraph *view);
		ControlCmdType RequestUpdateRecordValue(int argc, char** argv);

		// Convenience functions
		void CloseJavaConnection();
		ControlCmdType SendWindowData(const char* fileName,
		  Boolean doChecksum, int &checksumValue);
		ControlCmdType SendViewGData(ViewGraph *view);
		int  ControlCmd(ControlCmdType  status);
		void UpdateSessionList(char *dirName);
		void DoCloseSession();
		void DoOpenSession(char *fullpath);

		// < 0 if error
		int CreateView(View *view, View *parent);

		ControlCmdType SendChangedViews(Boolean update);
		void DeleteChildViews(View *view);
		void SendViewDataArea(View *view);
		void UpdateViewImage(View *view, int imageSize);
		void EraseChangedCursors();
		void DrawChangedCursors();

		DevStatus StartPlayingBack(const char *sessionFile);
		DevStatus SendCmd(const char* cmd);
		DevStatus StopPlayingBack();
		DevStatus StartRecording(const char *sessionFile);
		DevStatus StopRecording();
		DevStatus OpenCacheFiles(const char *sessionFile, Boolean
		  writing);
        DevStatus CloseCacheFiles(Boolean deleteFiles);

	protected:
		//TEMPTEMP -- is this the right way to return value?
		static void DrawCursor(View *view, DeviseCursor *cursor,
		  JavaScreenCmd *jsc = NULL);
		static void EraseCursor(View *view, DeviseCursor *cursor);
};


class JSArgs {
public:
  JSArgs(int maxArgs);
  ~JSArgs();

  void FillString(const char *value);
  void FillInt(int value);
  void FillDouble(double value);

  int ReturnVal(JavaScreenCmd *jsCmd) {
    return jsCmd->ReturnVal(_argc, (char **)_argv);
  }

private:
  int _maxArgs;
  const char **_argv;
  int _argc;
  Boolean *_dynamic;
};

#endif
