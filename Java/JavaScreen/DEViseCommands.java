// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains definitions of the various command names.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.17  2001/11/28 21:56:18  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.16  2001/11/19 17:17:02  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.15.2.2  2001/11/19 21:04:02  wenger
// Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
// Squid to be able to force DEVise to re-read all data and update the
// visualization; did some cleanup of JavaScreenCmd.C.
//
// Revision 1.15.2.1  2001/11/13 20:31:35  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.15  2001/10/30 17:29:09  xuk
// reated DEViseClient object for collaborating clients in jspop.
// Added JAVAC_ASK_COLLAB_LEADER and JAVAC_COLLABORATE commands.
//
// Revision 1.14  2001/05/11 20:36:06  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.13  2001/04/21 20:53:31  xuk
// Added functionality to disable collaboration.
// Added JAVAC_DisableCollab command.
//
// Revision 1.12  2001/04/20 21:05:11  wenger
// Changed 3D version of JAVAC_ShowRecords to use the previously-defined
// JAVAC_ShowRecords3D command; DEVise accepts that command but doesn't
// but doesn't actually return records yet; updated JavaScreen version;
// updated command documentation and protocol version.
//
// Revision 1.11  2001/04/01 03:51:18  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.10  2001/03/20 17:49:45  xuk
// Added collaboration for 3D Views.
//
// Revision 1.9  2001/03/03 20:13:11  xuk
// Restore old state if user goes into, then out of, collaboration mode.
// 1. Added JAVAC_SaveCurSession command.
// 2. Added JAVAC_ReopenSession command.
//
// Revision 1.8  2001/02/22 17:09:35  xuk
// Added JAVAC_CollabExit command for closing collaboration JSs.
//
// Revision 1.7  2001/02/21 17:45:34  xuk
// Added JAVAC_SetCollabPass command for setting collaboration passwd.
//
// Revision 1.6  2001/02/19 20:31:42  xuk
// Added command(s) and GUI so that a collaboration leader can find out who is
// collaborating with it.
// Added JAVAC_GET_COLLAB_LIST and JAVAC_COLLAB_STATE commands.
//
// Revision 1.5  2001/02/16 17:47:33  xuk
// Added new command JAVAC_Clients for collaboration JS to collect active client ID list.
//
// Revision 1.4  2001/01/22 17:08:12  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.3  2001/01/08 20:31:51  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.1.4.2  2000/10/18 20:28:10  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.2  2000/09/12 20:51:27  wenger
// Did some cleanup of the command-related code, better error messages from JSS.
//
// Revision 1.1.4.1  2000/10/09 16:26:41  xuk
// Add JAVAC_HEARTBEAT command for heart-beat from JS
//
// Revision 1.1  2000/04/27 20:15:24  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//

// ========================================================================

// DEViseCommands.java

//TEMP package edu.wisc.cs.devise.js.util;
package JavaScreen;

public final class DEViseCommands
{
    // -------------------------------------------------------------------
    // JavaScreen commands.

    // Prefix of all JavaScreen commands.
    public static final String JS_PREFIX = "JAVAC_";

    // The commands.
    public static final String ABORT = JS_PREFIX + "Abort";

    public static final String ACK = JS_PREFIX + "Ack";

    public static final String ASK_COLLAB_LEADER = JS_PREFIX +
      "AskCollabLeader";

    public static final String CHECK_POP = JS_PREFIX + "CheckPop";

    public static final String CLIENTS = JS_PREFIX + "Clients";

    public static final String CLOSE_SESSION = JS_PREFIX + "CloseCurrentSession";

    public static final String COLLAB_EXIT = JS_PREFIX + "CollabExit";

    public static final String COLLABORATE = JS_PREFIX + "Collaborate";

    public static final String COLLAB_STATE = JS_PREFIX + "CollabState";

    public static final String CONNECT = JS_PREFIX + "Connect";

    public static final String CREATE_VIEW = JS_PREFIX + "CreateView";

    public static final String CURSOR_CHANGED = JS_PREFIX + "CursorChanged";

    public static final String DELETE_CHILD_VIEWS = JS_PREFIX +
      "DeleteChildViews";

    public static final String DELETE_VIEW = JS_PREFIX + "DeleteView";

    public static final String DISABLE_COLLAB = JS_PREFIX + "DisableCollab";

    public static final String DONE = JS_PREFIX + "Done";

    public static final String DRAW_CURSOR = JS_PREFIX + "DrawCursor";

    public static final String ERASE_CURSOR = JS_PREFIX + "EraseCursor";

    public static final String ERROR = JS_PREFIX + "Error";

    public static final String EXIT = JS_PREFIX + "Exit";

    public static final String FAIL = JS_PREFIX + "Fail";

    public static final String GET_COLLAB_LIST = JS_PREFIX + "GetCollabList";

    public static final String GET_SERVER_STATE = JS_PREFIX + "GetServerState";

    public static final String GET_SESSION_LIST = JS_PREFIX + "GetSessionList";

    public static final String GET_VIEW_HELP = JS_PREFIX + "GetViewHelp";

    public static final String HEART_BEAT = JS_PREFIX + "HeartBeat";

    public static final String INIT_COLLAB = JS_PREFIX + "InitCollaboration";

    public static final String KEY_ACTION = JS_PREFIX + "KeyAction";

    public static final String MOUSE_RUBBERBAND = JS_PREFIX +
      "MouseAction_RubberBand";

    public static final String OPEN_SESSION = JS_PREFIX + "OpenSession";

    public static final String PROTOCOL_VERSION = JS_PREFIX + "ProtocolVersion";

    public static final String REFRESH_DATA = JS_PREFIX + "RefreshData";

    public static final String RESET_FILTERS = JS_PREFIX + "ResetFilters";

    public static final String REOPEN_SESSION = JS_PREFIX + "ReopenSession";

    public static final String SAVE_CUR_SESSION = JS_PREFIX + "SaveCurSession";

    public static final String SAVE_SESSION = JS_PREFIX + "SaveSession";

    public static final String SET_3D_CONFIG = JS_PREFIX + "Set3DConfig";

    public static final String SET_COLLAB_PASS = JS_PREFIX + "SetCollabPass";

    public static final String SET_DISPLAY_SIZE = JS_PREFIX + "SetDisplaySize";

    public static final String SHOW_RECORDS = JS_PREFIX + "ShowRecords";

    public static final String SHOW_RECORDS3D = JS_PREFIX + "ShowRecords3D";

    public static final String SHOW_VIEW_HELP = JS_PREFIX + "ShowViewHelp";

    public static final String UPDATE_GDATA = JS_PREFIX + "UpdateGData";

    public static final String UPDATE_RECORD_VALUE = JS_PREFIX +
      "UpdateRecordValue";

    public static final String UPDATE_SERVER_STATE = JS_PREFIX +
      "UpdateServerState";

    public static final String UPDATE_SESSION_LIST = JS_PREFIX +
      "UpdateSessionList";

    public static final String UPDATE_VIEW_IMAGE = JS_PREFIX +
      "UpdateViewImage";

    public static final String USER = JS_PREFIX + "User";

    public static final String VIEW_DATA_AREA = JS_PREFIX + "ViewDataArea";

    public static final String UPDATEJS = JS_PREFIX + "UpdateJS";
    // -------------------------------------------------------------------
    // JSS commands.

    // Prefix of all JSS commands.
    public static final String JSS_PREFIX = "JSS_";

    public static final String S_ADD = JSS_PREFIX + "Add";

    public static final String S_QUIT = JSS_PREFIX + "Quit";

    public static final String S_RESTART = JSS_PREFIX + "Restart";
}
