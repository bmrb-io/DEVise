// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

//TEMPTEMP
// This class handles the command socket/image socket pairs that
// connect the devised to the jspop, and the jspop to the js.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
