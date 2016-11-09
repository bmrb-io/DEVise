// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2004
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

// ========================================================================

// DEViseCheckableThread.java

package JavaScreen;

public interface DEViseCheckableThread
{
    public long lastRunTime();
    public void intThread();
    public String thread2Str();
    public boolean isAlive();
}
