// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
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

// ========================================================================

// DEViseCommands.java

public final class DEViseCommands
{
    // Prefix of all JavaScreen commands.
    public static final String PREFIX = "JAVAC_";

    // The commands.
    public static final String ABORT = PREFIX + "Abort";

    public static final String ACK = PREFIX + "Ack";

    public static final String CLOSE_SESSION = PREFIX + "CloseCurrentSession";

    public static final String CONNECT = PREFIX + "Connect";

    public static final String CREATE_VIEW = PREFIX + "CreateView";

    public static final String CURSOR_CHANGED = PREFIX + "CursorChanged";

    public static final String DELETE_CHILD_VIEWS = PREFIX +
      "DeleteChildViews";

    public static final String DELETE_VIEW = PREFIX + "DeleteView";

    public static final String DONE = PREFIX + "Done";

    public static final String DRAW_CURSOR = PREFIX + "DrawCursor";

    public static final String ERASE_CURSOR = PREFIX + "EraseCursor";

    public static final String ERROR = PREFIX + "Error";

    public static final String EXIT = PREFIX + "Exit";

    public static final String FAIL = PREFIX + "Fail";

    public static final String GET_SERVER_STATE = PREFIX + "GetServerState";

    public static final String GET_SESSION_LIST = PREFIX + "GetSessionList";

    public static final String GET_VIEW_HELP = PREFIX + "GetViewHelp";

    public static final String KEY_ACTION = PREFIX + "KeyAction";

    public static final String MOUSE_RUBBERBAND = PREFIX +
      "MouseAction_RubberBand";

    public static final String OPEN_SESSION = PREFIX + "OpenSession";

    public static final String PROTOCOL_VERSION = PREFIX + "ProtocolVersion";

    public static final String RESET_FILTERS = PREFIX + "ResetFilters";

    public static final String SAVE_SESSION = PREFIX + "SaveSession";

    public static final String SET_DISPLAY_SIZE = PREFIX + "SetDisplaySize";

    public static final String SHOW_RECORDS = PREFIX + "ShowRecords";

    public static final String SHOW_VIEW_HELP = PREFIX + "ShowViewHelp";

    public static final String UPDATE_GDATA = PREFIX + "UpdateGData";

    public static final String UPDATE_RECORD_VALUE = PREFIX +
      "UpdateRecordValue";

    public static final String UPDATE_SERVER_STATE = PREFIX +
      "UpdateServerState";

    public static final String UPDATE_SESSION_LIST = PREFIX +
      "UpdateSessionList";

    public static final String UPDATE_VIEW_IMAGE = PREFIX +
      "UpdateViewImage";

    public static final String USER = PREFIX + "User";

    public static final String VIEW_DATA_AREA = PREFIX + "ViewDataArea";
}
