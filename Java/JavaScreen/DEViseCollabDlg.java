// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// TEMP -- I don't know yet what this class is for.  RKW 2002-05-10.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2002/06/17 19:40:14  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
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

    public DEViseCollabDlg(jsdevisec j, int d, String[] msgs) {
        jsc = j;
	dlg = d;
	args = msgs;

	thread = new Thread(this);
	thread.setName("Collab dialog 1");
	thread.start();
    }

    public DEViseCollabDlg(jsdevisec j, int d, String msg) {
        jsc = j;
	dlg = d;
	arg = msg;

	thread = new Thread(this);
	thread.setName("Collab dialog 2");
	thread.start();
    }

    public void run()
    {
	switch (dlg) {
	case 1: // showRecordDlg()
	    jsc.showRecord(args);
	    break;
	case 2: // showServerState()
	    jsc.showServerState(args[1]);
	    break;
	case 3: // showCollabState()
	    jsc.showCollabState(arg);
	    break;
	}
    }

    public void stop()
    {
	switch (dlg) {
	case 1: // showRecordDlg()
	    jsc.recorddlg.close();
	    if (jsc.recorddlg != null)
		jsc.recorddlg = null;
	    break;
	case 2: // showServerState()
	    jsc.statedlg.close();
	    jsc.statedlg = null;
	    break;  
	case 3: // showCollabState()
	    jsc.collabstatedlg.close();
	    jsc.collabstatedlg = null;
	    break;   
	}

        thread.stop();
    }
}
