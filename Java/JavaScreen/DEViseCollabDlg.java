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

// $Id$

// $Log$
// Revision 1.5  2003/01/13 19:23:42  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.4  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.3  2002/06/17 19:40:14  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.1.2.11  2003/05/02 17:16:16  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.1.2.10  2003/04/02 16:19:37  wenger
// Possible fix for hard-to-reproduce null pointer exception in
// DEViseCanvas.checkMousePos(); a little bit of clean up of ugly
// DEViseCollabDlg class.
//
// Revision 1.1.2.9  2002/12/17 23:15:01  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.1.2.8  2002/11/25 21:29:34  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.1.2.7  2002/07/19 16:05:20  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.1.2.6  2002/05/10 14:46:20  wenger
// Added normal header.
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.jsc;
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
