// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Utility class.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2005/12/06 20:00:20  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.1.2.1  2003/05/02 17:16:17  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//

// ========================================================================

// DEViseUtils.java

//TEMP package edu.wisc.cs.devise.js.comm;
package JavaScreen;

import  java.io.*;
import  java.net.*;
import  java.util.Date;

public class DEViseUtils
{
    public static void printAllThreads(String msg)
    {
	int count = Thread.activeCount();
	System.out.println(count + " Threads (" + msg + "):");
        Thread[] threads = new Thread[count];
	int count2 = Thread.enumerate(threads);
	for (int index = 0; index < count2; index++) {
	    System.out.println("  " + threads[index]);
	}
    }
}
