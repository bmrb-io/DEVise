// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This interface is implemented by Threads to enable the
// DEViseThreadChecker class to check whether they may be hung.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1  2001/04/11 16:49:34  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//

// ========================================================================

// DEViseCheckableThread.java

public interface DEViseCheckableThread 
{
    public long lastRunTime();
    public void intThread();
    public String thread2Str();
}
