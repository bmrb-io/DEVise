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

// $Id$

// $Log$
// Revision 1.3.8.1  2004/05/12 21:43:55  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.3.30.1  2004/05/12 21:27:20  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.3  2001/05/11 20:36:04  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.2  2001/04/12 15:54:06  wenger
// Made some minor improvements to the hang checking.
//
// Revision 1.1  2001/04/11 16:49:34  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//

// ========================================================================

// DEViseCheckableThread.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

public interface DEViseCheckableThread 
{
    public long lastRunTime();
    public void intThread();
    public String thread2Str();
    public boolean isAlive();
}
