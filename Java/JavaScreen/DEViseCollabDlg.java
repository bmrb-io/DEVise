// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2003
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used for displaying dialogs in collaboration follower
// mode or auto playback mode.  It's used to destroy the dialog automatically
// rather than by the user clicking the OK button.

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

public class DEViseCollabDlg implements Runnable
{
    private jsdevisec jsc = null;
    private int dlg = 0;
    private String[] args = null;
    private String arg;
    private Thread thread = null;

    public static final int DLG_RECORD = 1;
    public static final int DLG_SERVER_STATE = 2;
    public static final int DLG_COLLAB_STATE = 3;

    public DEViseCollabDlg(jsdevisec j, int d, String[] msgs) {
        jsc = j;
        dlg = d;
        args = msgs;

        thread = new Thread(this);
        thread.setName("Collab dialog 1");
        thread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + thread);
        }
    }

    public DEViseCollabDlg(jsdevisec j, int d, String msg) {
        jsc = j;
        dlg = d;
        arg = msg;

        thread = new Thread(this);
        thread.setName("Collab dialog 2");
        thread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + thread);
        }
    }

    public void run()
    {
        switch (dlg) {
        case DLG_RECORD: // showRecordDlg()
            jsc.showRecord(args);
            break;
        case DLG_SERVER_STATE: // showServerState()
            jsc.showServerState(args[1]);
            break;
        case DLG_COLLAB_STATE: // showCollabState()
            jsc.showCollabState(arg);
            break;
        default:
            System.err.println("Illegal dialog type: " + dlg);
        }

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Thread " + thread + " ending");
        }
    }

    public void stop()
    {
        switch (dlg) {
        case DLG_RECORD: // showRecordDlg()
            jsc.recorddlg.close();
            if (jsc.recorddlg != null)
                jsc.recorddlg = null;
            break;
        case DLG_SERVER_STATE: // showServerState()
            jsc.statedlg.close();
            jsc.statedlg = null;
            break;
        case DLG_COLLAB_STATE: // showCollabState()
            jsc.collabstatedlg.close();
            jsc.collabstatedlg = null;
            break;
        default:
            System.err.println("Illegal dialog type: " + dlg);
        }

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Stopping thread " + thread);
        }
        thread.stop();
    }
}
