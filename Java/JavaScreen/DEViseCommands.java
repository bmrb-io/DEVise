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

public final class DEViseCommands
{
    // -------------------------------------------------------------------
    // JavaScreen commands.

    // Prefix of all JavaScreen commands.
    public static final String JS_PREFIX = "JAVAC_";

    // The commands.
    public static final String ABORT = JS_PREFIX + "Abort";

    public static final String ACK = JS_PREFIX + "Ack";

    public static final String CHECK_POP = JS_PREFIX + "CheckPop";

    public static final String CLOSE_SESSION = JS_PREFIX + "CloseCurrentSession";

    public static final String CONNECT = JS_PREFIX + "Connect";

    public static final String CREATE_VIEW = JS_PREFIX + "CreateView";

    public static final String CURSOR_CHANGED = JS_PREFIX + "CursorChanged";

    public static final String DELETE_CHILD_VIEWS = JS_PREFIX +
      "DeleteChildViews";

    public static final String DELETE_VIEW = JS_PREFIX + "DeleteView";

    public static final String DONE = JS_PREFIX + "Done";

    public static final String DRAW_CURSOR = JS_PREFIX + "DrawCursor";

    public static final String ERASE_CURSOR = JS_PREFIX + "EraseCursor";

    public static final String ERROR = JS_PREFIX + "Error";

    public static final String EXIT = JS_PREFIX + "Exit";

    public static final String FAIL = JS_PREFIX + "Fail";

    public static final String GET_SERVER_STATE = JS_PREFIX + "GetServerState";

    public static final String GET_SESSION_LIST = JS_PREFIX + "GetSessionList";

    public static final String GET_VIEW_HELP = JS_PREFIX + "GetViewHelp";

    public static final String HEART_BEAT = JS_PREFIX + "HeartBeat";

    public static final String KEY_ACTION = JS_PREFIX + "KeyAction";

    public static final String MOUSE_RUBBERBAND = JS_PREFIX +
      "MouseAction_RubberBand";

    public static final String OPEN_SESSION = JS_PREFIX + "OpenSession";

    public static final String PROTOCOL_VERSION = JS_PREFIX + "ProtocolVersion";

    public static final String RESET_FILTERS = JS_PREFIX + "ResetFilters";

    public static final String SAVE_SESSION = JS_PREFIX + "SaveSession";

    public static final String SET_DISPLAY_SIZE = JS_PREFIX + "SetDisplaySize";

    public static final String SHOW_RECORDS = JS_PREFIX + "ShowRecords";

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

    public static final String CLIENTS = JS_PREFIX + "Clients";

    // -------------------------------------------------------------------
    // JSS commands.

    // Prefix of all JSS commands.
    public static final String JSS_PREFIX = "JSS_";

    public static final String S_ADD = JSS_PREFIX + "Add";

    public static final String S_QUIT = JSS_PREFIX + "Quit";

    public static final String S_RESTART = JSS_PREFIX + "Restart";
}
